/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * IDEA 1: add boolean about on_parallel_boundary vs. interior.
 *         1. Do the parallel boundary work here.
 *            To the least possible amount of work before sending.
 *             * copy_samelevel
 *             * average_ghost_f2c
 *             * fill_physical_ghost
 *               (which subset of cells shall do it?)
 *               (maybe we can skip doing it before sending altogether?)
 *            More specifically, only copy/average across faces
 *            between two parallel boundary patches, not corners.
 *            Argue in paper why no corners needed for 5-point stencils.
 *         2. Start sending: ghost_exchange_begin.
 *         3. Do all the work on interior patches and whatever has not
 *            been computed between parallel boundary patches.
 *            Do the most possible amount of work while communicating.
 *             * copy_samelevel
 *             * average_ghost_f2c
 *             * fill_physical_ghost
 *               (how much of this can we possibly to here: maximize this.)
 *             * interpolate_ghost_c2f for interior patches and
 *               faces between parallel boundary and interior patches.
 *         4. Recieve messages: ghost_exchange_end.
 *         5. Work on receive buffers / parallel patches with remote data.
 *             * copy_samelevel
 *             * average_ghost_f2c
 *             * fill_physical_ghost
 *             * interpolate_ghost_c2f
 *         6. Repeat 5. for whatever parts of patches that are not done yet.
 *            Integrate this with 5. so we don't loop over patches twice.
 *
 * All of this goes into the paper as detailed algorithm with explanations.
 */


/** \file
 * Fill ghost cells.
 *
 *
 **/

#include <fclaw2d_forestclaw.h>
#include <fclaw_timer.h>

#include <fclaw2d_ghost_fill.h>
#include <fclaw2d_patch.h>
#include <fclaw2d_clawpatch.h>
#include <fclaw2d_partition.h>
#include <fclaw2d_exchange.h>
#include <fclaw2d_domain.h>


#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif


/* -------------------------------------------------
   Basic routines - operate on a single level
   ----------------------------------------------- */

static fclaw2d_patch_iterator_t patch_iterator;

struct fclaw2d_ghost_fill_wrap_info
{
    fclaw2d_ghost_fill_parallel_mode_t ghost_mode;
    fclaw2d_patch_callback_t cb_fill;
    void *user;
};

/* these routines filter patches according to whether we are working on
   patches on the parallel boundary only, or on interior patches.
*/
/* Passing in a function pointer is somehow slower ... */
static void cb_parallel_wrap(fclaw2d_domain_t* domain,
                                  fclaw2d_patch_t* this_patch,
                                  int this_block_idx,
                                  int this_patch_idx,
                                  void *user)
{
    struct fclaw2d_ghost_fill_wrap_info w = *((fclaw2d_ghost_fill_wrap_info*) user);
    int on_boundary = fclaw2d_patch_on_parallel_boundary(this_patch);
    if (((w.ghost_mode == FCLAW2D_BOUNDARY_GHOST_ONLY) && on_boundary) ||
        ((w.ghost_mode == FCLAW2D_BOUNDARY_INTERIOR_ONLY) && !on_boundary) ||
        w.ghost_mode == FCLAW2D_BOUNDARY_ALL)
    {
        w.cb_fill(domain,this_patch,this_block_idx,this_patch_idx,w.user);
    }
}

static
void copy2ghost(fclaw2d_domain_t *domain,
                int level,
                int time_interp,
                int read_parallel_patches,
                fclaw2d_ghost_fill_parallel_mode_t ghost_mode);

static
void average2ghost(fclaw2d_domain_t *domain,
                   int coarse_level,
                   int time_interp,
                   int read_parallel_patches,
                   fclaw2d_ghost_fill_parallel_mode_t ghost_mode);
static
void interpolate2ghost(fclaw2d_domain_t *domain,
                       int fine_level,
                       int time_interp,
                       int read_parallel_patches,
                       fclaw2d_ghost_fill_parallel_mode_t ghost_mode);

static
void setphysical(fclaw2d_domain_t *domain,
                 int level,
                 double sync_time,
                 int time_interp,
                 fclaw2d_ghost_fill_parallel_mode_t ghost_mode);

/* -------------------------------------------------
   Loop over all levels
   ----------------------------------------------- */
static
void copy_samelevel(fclaw2d_domain_t* domain,
                          int minlevel,
                          int maxlevel,
                          int time_interp,
                          int read_parallel_patches,
                          fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    /* Copy between grids that are at the same level. */
    for(int level = maxlevel; level >= minlevel; level--)
    {
        int time_interp = 0;
        copy2ghost(domain,level,time_interp,
                   read_parallel_patches,ghost_mode);
    }
    if (time_interp)
    {
        int time_interp_level = minlevel-1;
        copy2ghost(domain,time_interp_level,time_interp,
                   read_parallel_patches,ghost_mode);
    }

}


/**
 * \ingroup Averaging
 * Fill in coarse grid ghost cells by averaging or copying  from neighboring fine grids.
 **/
static
void average_fine2coarse_ghost(fclaw2d_domain_t *domain,
                               int mincoarse,
                               int maxcoarse,
                               int time_interp,
                               int read_parallel_patches,
                               fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    /* Average fine grids to coarse grid ghost cells */
    for(int level = maxcoarse; level >= mincoarse; level--)
    {
        /* Don't do any time interpolation yet */
        int time_interp = 0;
        average2ghost(domain,level,time_interp,
                      read_parallel_patches,ghost_mode);
    }
    if (time_interp)
    {
        /* Average fine grid to coarse time interpolated level.  Time interpolated
         faces need correct values.  */
        int time_interp_level = mincoarse-1;
        average2ghost(domain,time_interp_level,time_interp,
                      read_parallel_patches,ghost_mode);
    }
}

static
void interpolate_coarse2fine_ghost(fclaw2d_domain_t* domain,
                                   int mincoarse,
                                   int maxcoarse,
                                   int time_interp,
                                   int read_parallel_patches,
                                   fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    /* Interpolate from coarse grid to fine grid ghost */
    for(int level = maxcoarse; level >= mincoarse; level--)
    {
        /* No need to interpolate to coarse time-interpolated grids */
        int time_interp = 0;
        interpolate2ghost(domain,level,time_interp,read_parallel_patches,ghost_mode);
    }
    if (time_interp)
    {
        /* interpolate from time interpolated level */
        int time_interp_level = mincoarse-1;
        interpolate2ghost(domain,time_interp_level,time_interp,
                          read_parallel_patches,ghost_mode);
    }
}

static
void fill_physical_ghost(fclaw2d_domain_t* domain,
                         int minlevel,
                         int maxlevel,
                         double sync_time,
                         int time_interp,
                         fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    for(int level = maxlevel; level >= minlevel; level--)
    {
        int time_interp = 0;
        setphysical(domain,level,sync_time,time_interp,ghost_mode);
    }
    if (time_interp)
    {
        int time_interp_level = minlevel-1;
        setphysical(domain,
                    time_interp_level,
                    sync_time,
                    time_interp,
                    ghost_mode);
    }
}

static
void copy2ghost(fclaw2d_domain_t *domain,
                int level,
                int time_interp,
                int read_parallel_patches,
                fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    struct fclaw2d_ghost_fill_wrap_info parallel_mode;
    fclaw2d_exchange_info_t e_info;
    e_info.exchange_type = FCLAW2D_COPY;
    e_info.grid_type = FCLAW2D_IS_COARSE;
    e_info.time_interp = time_interp;
    e_info.read_parallel_patches = read_parallel_patches;

    parallel_mode.ghost_mode = ghost_mode;
    parallel_mode.user = (void*) &e_info;

    parallel_mode.cb_fill = cb_face_fill;
    patch_iterator(domain, level, cb_parallel_wrap,
                   (void *) &parallel_mode);
    /* corner exchanges */
    parallel_mode.cb_fill = cb_corner_fill;
    patch_iterator(domain, level, cb_parallel_wrap,
                   (void *) &parallel_mode);

}


static
void average2ghost(fclaw2d_domain_t *domain,
                   int coarse_level,
                   int time_interp,
                   int read_parallel_patches,
                   fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    struct fclaw2d_ghost_fill_wrap_info parallel_mode;

    fclaw2d_exchange_info_t e_info;
    e_info.time_interp = time_interp; /* Does this matter here? */
    e_info.read_parallel_patches = read_parallel_patches;
    e_info.exchange_type = FCLAW2D_AVERAGE;
    e_info.has_fine_grid_neighbor = 0;

    /* Only update ghost cells at local boundaries */
    e_info.grid_type = FCLAW2D_IS_COARSE;

    parallel_mode.user = (void*) &e_info;
    parallel_mode.ghost_mode = ghost_mode;

    parallel_mode.cb_fill = cb_face_fill;
    patch_iterator(domain, coarse_level,
                   cb_parallel_wrap, (void *) &parallel_mode);

    /* Corner average */
    parallel_mode.cb_fill = cb_corner_fill;
    patch_iterator(domain, coarse_level, cb_parallel_wrap,
                   (void *) &parallel_mode);

#if 0
    /* Used in time interpolation */
    int y = parallel_mode.e_info.has_fine_grid_neighbor;
    fclaw2d_clawpatch_set_finegrid_neighbors(domain,this_patch,y);
#endif

    if (read_parallel_patches)
    {
        /* Second pass : average from local fine grids to remote coarse grids. These
           coarse grids might be needed for interpolation later. */
        e_info.grid_type = FCLAW2D_IS_FINE;

        int fine_level = coarse_level + 1;

        /* Face average */
        parallel_mode.cb_fill = cb_face_fill;
        fclaw2d_domain_iterate_level(domain, fine_level,
                                     cb_parallel_wrap, (void *) &parallel_mode);

        /* Corner average :
           We can skip the corner update, since we don't need the corner ghost cell
           values for doing interpolation (at least not yet) */
#if 0
        parallel_mode.cb_fill = cb_corner_fill;
        fclaw2d_domain_iterate_level(domain, fine_level, cb_corner_wrap,
                                     (void *) &parallel_mode);
#endif
    }
}

static
void interpolate2ghost(fclaw2d_domain_t *domain,
                       int coarse_level,
                       int time_interp,
                       int read_parallal_patches,
                       fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    struct fclaw2d_ghost_fill_wrap_info parallel_mode;
    fclaw2d_exchange_info_t e_info;
    e_info.time_interp = time_interp;
#if 0
    e_info.level = coarse_level;
#endif
    e_info.level = -1;
    e_info.exchange_type = FCLAW2D_INTERPOLATE;

    /* ----------------------------------------------------------
       First pass - look for fine grids to interpolate to. This
       should include include the time interpolated level.
       ---------------------------------------------------------- */

    e_info.grid_type = FCLAW2D_IS_COARSE;
    e_info.read_parallel_patches = read_parallal_patches;

    parallel_mode.ghost_mode = ghost_mode;
    parallel_mode.user = (void*) &e_info;

    /* Face interpolate */
    parallel_mode.cb_fill = cb_face_fill;
    patch_iterator(domain,coarse_level, cb_parallel_wrap,
                                         (void *) &parallel_mode);

    /* Corner interpolate */
    parallel_mode.cb_fill = cb_corner_fill;
    patch_iterator(domain,coarse_level, cb_parallel_wrap,
                  (void *) &parallel_mode);
    /* -----------------------------------------------------
       Second pass - Iterate over local fine grids, looking
       for remote coarse grids we can use to fill in BCs at
       fine grid ghost cells along the parallel boundary
       ----------------------------------------------------- */

    e_info.grid_type = FCLAW2D_IS_FINE;

    /* Interpolate to faces at parallel boundaries from coarse grid ghost
       patches */
    int fine_level = coarse_level + 1;

    parallel_mode.cb_fill = cb_face_fill;
    fclaw2d_domain_iterate_level(domain, fine_level, cb_parallel_wrap,
                                 (void *) &parallel_mode);

    /* Interpolate to corners at parallel boundaries from coarse grid
       ghost patches */
    parallel_mode.cb_fill = cb_corner_fill;
    fclaw2d_domain_iterate_level(domain, fine_level, cb_parallel_wrap,
                                 (void *) &parallel_mode);
}

void setphysical(fclaw2d_domain_t *domain,
                 int level,
                 double sync_time,
                 int time_interp,
                 fclaw2d_ghost_fill_parallel_mode_t ghost_mode)
{
    struct fclaw2d_ghost_fill_wrap_info parallel_mode;

    fclaw2d_physical_time_info_t t_info;
    t_info.level_time = sync_time;
    t_info.time_interp = time_interp;

    parallel_mode.ghost_mode = ghost_mode;
    parallel_mode.user = (void*) &t_info;

    parallel_mode.cb_fill = cb_fclaw2d_physical_set_bc;
    fclaw2d_domain_iterate_level(domain, level, cb_parallel_wrap,
                                 (void *) &parallel_mode);
}




/* -----------------------------------------------------------------------
   Public interface
   ---------------------------------------------------------------------*/

/**
 * <summary>Complete exchange of all ghost patches at all levels.</summary>
 * <remarks>All parallel ghost patches are also exchanged at all
 * levels.</remarks>
 * <list>
 *    <item>Every level exchanges ghost cells with other patches
 *       at that level</item>
 *    <item>Every finer level exchanges with a coarser level</item>
 *    <item> All levels will be updated in next update step, regardless of
 *       whether we are in the subcycled or non-subcycled case.</item>
 *       </list>
 *   The reason for two separate ghost cell exchange routines is that
 *   the logic here is considerably simpler than for the partial
 *   update used in intermediate steps in the subcycled case.
 **/
void fclaw2d_ghost_update(fclaw2d_domain_t* domain,
                          int minlevel,
                          int maxlevel,
                          double sync_time,
                          int time_interp,
                          fclaw2d_timer_names_t running)
{
    const amr_options_t *gparms = get_domain_parms(domain);
    fclaw2d_domain_data_t *ddata = fclaw2d_domain_get_data(domain);
    if (running != FCLAW2D_TIMER_NONE) {
        fclaw2d_timer_stop (&ddata->timers[running]);
    }
    fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXCHANGE]);

    fclaw_global_infof("Exchanging ghost patches across all levels\n");

#if 0
    /* uncomment this if debugging ghost cell interpolation */
    fclaw_global_essentialf("WARNING : compute_slopes set to average\n");
#endif

#if 0
    /* For debugging */
    fclaw2d_clawpatch_set_boundary_to_nan(domain,minlevel,maxlevel,
                                          time_interp);
#endif

    /* ---------------------------------------------------------
       Get coarse grid ghost cells ready to use for interpolation.
       Coarse ghost cells on ghost patches are not updated in
       this step.  Ghost patches do not yet have valid data, and
       so boundary patches will have to be updated after the exchange.
       ---------------------------------------------------------- */

    if (time_interp)
    {
        int time_interp_level = minlevel - 1;
        fclaw_global_infof("Time interpolated level is %d\n",   \
                           time_interp_level);
    }
    else
    {
        FCLAW_ASSERT(gparms->minlevel == minlevel);
        FCLAW_ASSERT(gparms->maxlevel == maxlevel);

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA4]);
        fclaw2d_face_neighbors_set_type(domain);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA4]);

    }

    /* If minlevel == maxlevel, then maxcoarse < mincoase. In this
       case, loops involving averaging and interpolation will be
       skipped and we only copy between patches on
       same levels. */

    int mincoarse = minlevel;
    int maxcoarse = maxlevel-1;   /* maxlevel >= minlevel */

    /* This works, but is much slower than just interpolating to all
       time interpolated grids */
    if (!time_interp)
    {
        /* Identify fine grid neighbors for future use */
        fclaw2d_clawpatch_finegrid_neighbors(domain);
    }

#if (_OPENMP)
    /* Multi-thread only in single processor case. */
    patch_iterator = &fclaw2d_domain_iterate_level_mthread;
#else
    patch_iterator = &fclaw2d_domain_iterate_level;
#endif

    if (domain->mpisize == 0)
    {
        /* This seems to be equivalent to the other branch of this loop when
           run on a single processor (but is about 20% faster on a
           single processor) */
        fclaw2d_ghost_fill_parallel_mode_t parallel_mode =
            FCLAW2D_BOUNDARY_ALL;
        int read_parallel_patches = 0;

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);
        copy_samelevel(domain,minlevel,maxlevel,time_interp,
                       read_parallel_patches,parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);
        average_fine2coarse_ghost(domain,mincoarse,maxcoarse,
                                  time_interp,
                                  read_parallel_patches,
                                  parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);

        fill_physical_ghost(domain,
                            mincoarse,
                            maxcoarse,
                            sync_time,
                            time_interp,
                            parallel_mode);

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA3]);
        interpolate_coarse2fine_ghost(domain,mincoarse, maxcoarse,
                                      time_interp,read_parallel_patches,
                                      parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA3]);

        fill_physical_ghost(domain,
                            minlevel,
                            maxlevel,
                            sync_time,
                            time_interp,
                            parallel_mode);
    }
    else
    {
        /* --------------------------------------------------------------
           Do work we can do before sending
           ------------------------------------------------------------*/
        /* Copy/average ghost cells in local patches at the parallel boundary.
           This is needed so that when these boundary patches get sent to other
           processors as ghost patches, they have valid ghost cells if needed
           for interpolation.*/
        fclaw2d_ghost_fill_parallel_mode_t parallel_mode =
            FCLAW2D_BOUNDARY_GHOST_ONLY;
        int read_parallel_patches = 0;

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        /* Copy */
        copy_samelevel(domain,minlevel,maxlevel,time_interp,
                       read_parallel_patches,parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);

        /* Average */
        average_fine2coarse_ghost(domain,mincoarse,maxcoarse,
                                  time_interp,
                                  read_parallel_patches,
                                  parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);

        /* This is needed when the parallel boundary intersects the physical
           boundary.  In this case, a coarse grid ghost patch might
           need to have physical boundary conditions in order to interpolate
           to a fine grid local patch. Note that we don't need to fill in
           ghost cells on the finest level grids, since these will never be
           used for interpolation. */
        fill_physical_ghost(domain,
                            mincoarse,
                            maxcoarse,
                            sync_time,
                            time_interp,
                            parallel_mode);

        /* --------------------------------------------------------------
           Start send ...
           ------------------------------------------------------------*/
        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_GHOSTCOMM]);
        fclaw2d_exchange_ghost_patches_begin(domain,minlevel,maxlevel,time_interp);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_GHOSTCOMM]);

        /* --------------------------------------------------------------
           Finish exchanges in the interior of the grid.
           ------------------------------------------------------------*/

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_GHOST_HIDE]);
        parallel_mode = FCLAW2D_BOUNDARY_INTERIOR_ONLY;

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        /* Copy */
        copy_samelevel(domain,minlevel,maxlevel,time_interp,
            read_parallel_patches,parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);

        /* Average */
        average_fine2coarse_ghost(domain,mincoarse,maxcoarse,
            time_interp,
            read_parallel_patches,
            parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);


        /* Physical ghost */
        fill_physical_ghost(domain,
                            mincoarse,
                            maxcoarse,
                            sync_time,
                            time_interp,
                            parallel_mode);

        /* Fine grids that are adjacent to boundary patches don't get
           ghost regions filled in that overlap boundary patch */
        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA3]);

        /* Interpolate */
        interpolate_coarse2fine_ghost(domain,mincoarse, maxcoarse,
                                      time_interp,
                                      read_parallel_patches,
                                      parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA3]);

        /* minfine to maxfine?  */
        int minfine = mincoarse+1;
        int maxfine = maxlevel;

        /* Physical ghost */
        fill_physical_ghost(domain,
                            minfine,
                            maxfine,
                            sync_time,
                            time_interp,
                            parallel_mode);


        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_GHOST_HIDE]);

        /* -------------------------------------------------------------
           Receive ghost patches ...
           ------------------------------------------------------------- */

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_GHOSTCOMM]);
        fclaw2d_exchange_ghost_patches_end(domain,minlevel,maxlevel,
                                           time_interp);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_GHOSTCOMM]);

        /* -------------------------------------------------------------
           Loop over ghost patches to find indirect neighbors and do
           any necessary face exchanges.
           ------------------------------------------------------------- */
        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_GHOST_EXCHANGE]);
        fclaw2d_face_neighbor_ghost(domain,minlevel,maxlevel,time_interp);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_GHOST_EXCHANGE]);

        /* -------------------------------------------------------------
           Repeat above, but now with parallel ghost cells.
           ------------------------------------------------------------- */

        parallel_mode = FCLAW2D_BOUNDARY_GHOST_ONLY;
        read_parallel_patches = 1;

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        /* Copy */
        copy_samelevel(domain,minlevel,maxlevel,time_interp,
            read_parallel_patches,parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA1]);

        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);

        /* Average */
        average_fine2coarse_ghost(domain,mincoarse,maxcoarse, time_interp,
            read_parallel_patches,parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA2]);

        /* Physical */
        fill_physical_ghost(domain,
                            minlevel,
                            maxlevel,
                            sync_time,
                            time_interp,
                            parallel_mode);


        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_EXTRA3]);

        /* Interpolate */
        interpolate_coarse2fine_ghost(domain,mincoarse, maxcoarse,
                                      time_interp,read_parallel_patches,
                                      parallel_mode);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXTRA3]);

        /* This needs to be over all patches.  The situation that can arise is
           that a coarse grid boundary patch adjacent to a fine grid non-boundary
           patch. Both have a physical boundary.  In this case, the following steps
           happens at the coarse/fine interface :

           (1) Ghost only (before exchange) : Fine grid averages to coarse grid;
           Physical BC applied to coarse grid.

           (2) Interior only : Fine grid is not picked up in an interpolation step,
           since in interpolation, we only sweep over coarse grids, looking for fine
           grid neighbors.  Applying physical BC to this fine grid leaves invalid values
           in fine grid ghost cells at boundary.

           (3) Ghost only (after exchange) : Coarse grid boundary patch interpolates
           to adjacent fine grid.  But now, we need to apply physical BCs to fine grid.
           So we sweep over all grids to apply phys. BCs.  This is overkill, since
           only those fine grids with neighboring coarse grid patches on the parallel boundary
           are affected, but it is hard to see how to avoid this without some tedious
           checking.
        */

        /* Physical */
        fill_physical_ghost(domain,
                            minlevel,
                            maxlevel,
                            sync_time,
                            time_interp,
                            FCLAW2D_BOUNDARY_ALL);
    }
    // Stop timing
    fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_EXCHANGE]);
    if (running != FCLAW2D_TIMER_NONE)
    {
        fclaw2d_timer_start (&ddata->timers[running]);
    }
}


#ifdef __cplusplus
#if 0
{
#endif
}
#endif
