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

#include <p4est_base.h>

#include "amr_utils.H"
#include "fclaw2d_solvers.H"

int pow_int(int a, int n)
{
    int b = 1;
    for(int i = 0; i < n; i++)
    {
        b *= a;
    }
    return b;
}

static void problem_setup_default(fclaw2d_domain_t* domain)
{
}


/* -----------------------------------------------------------------
   Initialize data
   ----------------------------------------------------------------- */

// Should this maybe be 'allocate_domain_data' instead?  Reserve 'init'
// assigning default values to items?
void init_domain_data(fclaw2d_domain_t *domain)
{
    fclaw2d_domain_data_t* ddata = (fclaw2d_domain_data_t*) domain->user;
    ddata = FCLAW2D_ALLOC_ZERO(fclaw2d_domain_data_t, 1);
    domain->user = (void *) ddata;

    ddata->count_set_clawpatch = ddata->count_delete_clawpatch = 0;
    ddata->count_amr_advance = 0;
    ddata->count_ghost_exchange = 0;
    ddata->count_amr_regrid = 0;
    ddata->is_latest_domain = 0;        /* set 1 by amrinit or rebuild_domain */

    ddata->domain_exchange = NULL;

    ddata->amropts = NULL;
    ddata->curr_time = 0;

    /* I put this here because somehow it is not part of a 'solver' */
    ddata->f_problem_setup = &problem_setup_default;

    fclaw2d_solver_functions_t* solver_functions = FCLAW2D_ALLOC(fclaw2d_solver_functions_t, 1);
    initialize_solver_functions(solver_functions);
    ddata->solver_functions = solver_functions;

    fclaw2d_regrid_functions_t* regrid_functions = FCLAW2D_ALLOC(fclaw2d_regrid_functions_t, 1);
    initialize_regrid_functions(regrid_functions);
    ddata->regrid_functions = regrid_functions;

    fclaw2d_output_functions_t* output_functions = FCLAW2D_ALLOC(fclaw2d_output_functions_t, 1);
    initialize_output_functions(output_functions);
    ddata->output_functions = output_functions;

    fclaw2d_diagnostic_functions_t * diagnostic_functions =
        FCLAW2D_ALLOC(fclaw2d_diagnostic_functions_t, 1);

}

void delete_domain_data(fclaw2d_domain_t* domain)
{
    fclaw2d_domain_data_t* ddata = (fclaw2d_domain_data_t*) domain->user;

    fclaw2d_solver_functions_t *sf = (fclaw2d_solver_functions_t*) ddata->solver_functions;
    if (sf != NULL)
    {
        FCLAW2D_FREE(sf);
        ddata->solver_functions = (fclaw2d_solver_functions_t*) NULL;
    }

    fclaw2d_regrid_functions_t *rf = (fclaw2d_regrid_functions_t*) ddata->regrid_functions;
    if (rf != NULL)
    {
        FCLAW2D_FREE(rf);
        ddata->regrid_functions = (fclaw2d_regrid_functions_t*) NULL;
    }

    fclaw2d_output_functions_t *of = (fclaw2d_output_functions_t*) ddata->output_functions;
    if (of != NULL)
    {
        FCLAW2D_FREE(of);
        ddata->output_functions = (fclaw2d_output_functions_t*) NULL;
    }

    FCLAW2D_FREE (ddata);
    domain->user = NULL;
}

void init_block_data(fclaw2d_block_t *block)
{
    fclaw2d_block_data_t *bdata = FCLAW2D_ALLOC_ZERO (fclaw2d_block_data_t, 1);
    block->user = (void *) bdata;
}


void init_patch_data(fclaw2d_patch_t *patch)
{
    fclaw2d_patch_data_t *pdata = FCLAW2D_ALLOC(fclaw2d_patch_data_t, 1);
    patch->user = (void *) pdata;
}

void delete_patch_data(fclaw2d_patch_t *patch)
{
    fclaw2d_patch_data_t *pd = (fclaw2d_patch_data_t*) patch->user;
    FCLAW2D_FREE(pd);
}

/* -----------------------------------------------------------------
   Work with timers
   ----------------------------------------------------------------- */

double
fclaw2d_timer_wtime (void)
{
    return sc_MPI_Wtime ();
}

void
fclaw2d_timer_init (fclaw2d_timer_t *timer)
{
    memset (timer, 0, sizeof (fclaw2d_timer_t));
}

void
fclaw2d_timer_start (fclaw2d_timer_t *timer)
{
    if (!timer->running) {
        timer->started = fclaw2d_timer_wtime ();
        timer->stopped = 0.;
        timer->running = 1;
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

void
fclaw2d_timer_stop (fclaw2d_timer_t *timer)
{
    if (timer->running) {
        timer->stopped = fclaw2d_timer_wtime ();
        timer->cumulative += timer->stopped - timer->started;
        timer->running = 0;
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

// -----------------------------------------------------------------
// Return pointer to user data
// -----------------------------------------------------------------
fclaw2d_domain_data_t *get_domain_data(fclaw2d_domain_t *domain)
{
    return (fclaw2d_domain_data_t *) domain->user;
}


fclaw2d_block_data_t *get_block_data(fclaw2d_block_t *block)
{
    return (fclaw2d_block_data_t *) block->user;
}


fclaw2d_patch_data_t *get_patch_data(fclaw2d_patch_t *patch)
{
    return (fclaw2d_patch_data_t *) patch->user;
}

fclaw2d_map_context_t* get_map_context(fclaw2d_domain_t* domain)
{
    fclaw2d_map_context_t* cont;
  cont = (fclaw2d_map_context_t*)
         fclaw2d_domain_attribute_access (domain, "fclaw_map_context", NULL);
  FCLAW_ASSERT (cont != NULL);
  return cont;
}


// -----------------------------------------------------------------
// Set user data with user defined variables, etc.
// -----------------------------------------------------------------

void copy_domain_data(fclaw2d_domain_t *old_domain, fclaw2d_domain_t *new_domain)
{
    fclaw2d_domain_data_t *ddata_old = get_domain_data(old_domain);

    /* Has the data already been allocated? */
    fclaw2d_domain_data_t *ddata_new = get_domain_data(new_domain);


    /* Move timers over to the new domain */
    ddata_old->is_latest_domain = 0;
    memcpy (ddata_new->timers, ddata_old->timers,
            sizeof (fclaw2d_timer_t) * FCLAW2D_TIMER_COUNT);
    ddata_new->is_latest_domain = 1;
    ddata_new->count_amr_advance = ddata_old->count_amr_advance;
    ddata_new->count_ghost_exchange = ddata_old->count_ghost_exchange;
    ddata_new->count_amr_regrid = ddata_old->count_amr_regrid;


    /*
       We don't need to copy domain_exchange, since it is rebuilt whenever
       we create a new domain.
    */

    /* Copy data members */
    ddata_new->amropts = ddata_old->amropts;
    ddata_new->clawpack_parms = ddata_old->clawpack_parms;
    ddata_new->manyclaw_parms = ddata_old->manyclaw_parms;

    ddata_new->curr_time = ddata_old->curr_time;

    ddata_new->f_problem_setup = ddata_old->f_problem_setup;

    copy_solver_functions(ddata_old->solver_functions,ddata_new->solver_functions);
    copy_regrid_functions(ddata_old->regrid_functions,ddata_new->regrid_functions);
    copy_output_functions(ddata_old->output_functions,ddata_new->output_functions);
}



void set_block_data(fclaw2d_block_t *block, const int mthbc[])
{
    fclaw2d_block_data_t *bdata = get_block_data(block);
    for(int i = 0; i < 4; i++)
    {
        bdata->mthbc[i] = mthbc[i];
    }
}


void set_patch_data(fclaw2d_patch_t *patch, ClawPatch* cp)
{
    fclaw2d_patch_data_t *pdata = get_patch_data(patch);
    pdata->cp = cp;
}


// -----------------------------------------------------------------
// Some lazy helper functions that really do make things easier..
// -----------------------------------------------------------------
void init_block_and_patch_data(fclaw2d_domain_t *domain)
{
    fclaw2d_block_t *block;
    fclaw2d_patch_t *patch;

    // init_domain_data(domain);

    for (int i = 0; i < domain->num_blocks; i++)
    {
        block = &domain->blocks[i];
        init_block_data(block);
        for (int j = 0; j < block->num_patches; j++)
        {
            patch = &block->patches[j];
            init_patch_data(patch);
        }
    }
}


void link_problem_setup(fclaw2d_domain_t* domain, fclaw2d_problem_setup_t f_problem_setup)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    ddata->f_problem_setup = f_problem_setup;
}

const amr_options_t* get_domain_parms(fclaw2d_domain_t *domain)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    return ddata->amropts;
}

void set_domain_parms(fclaw2d_domain_t *domain, const amr_options_t* gparms)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    ddata->amropts = gparms;
}

void set_domain_time(fclaw2d_domain_t *domain, double time)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    ddata->curr_time = time;
}

double get_domain_time(fclaw2d_domain_t *domain)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    return ddata->curr_time;
}

fclaw2d_domain_exchange_t*
    get_domain_exchange_data(fclaw2d_domain_t* domain)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    return ddata->domain_exchange;
}

void set_domain_exchange_data(fclaw2d_domain_t* domain,fclaw2d_domain_exchange_t *e)
{
    fclaw2d_domain_data_t *ddata = get_domain_data (domain);
    ddata->domain_exchange = e;
}





// Will change the name of this to 'get_clawpatch' eventually
ClawPatch* get_clawpatch(fclaw2d_patch_t *patch)
{
    fclaw2d_patch_data_t *pdata = (fclaw2d_patch_data_t *) patch->user;

    return pdata->cp;
}

/* end of helper functions */


static void cb_num_patches(fclaw2d_domain_t *domain,
	fclaw2d_patch_t *patch, int block_no, int patch_no, void *user)
{
  (*(int *) user)++;
}

int num_patches(fclaw2d_domain_t *domain, int level, int include_shadow)
{
    int count = 0;
    if (include_shadow == 0)
    {
        fclaw2d_domain_iterate_level(domain, level,
                                     cb_num_patches,
                                     &count);
    }
    else
    {
        // Include shadow patches
    }
    return count;
}

/* Functions with C prototypes to use forestclaw from C code */

void
fclaw_mpi_init (int * argc, char *** argv, sc_MPI_Comm mpicomm, int lp)
{
#ifdef P4EST_MPI
    int mpiret;

    //mpiret = sc_MPI_Init (argc, argv);
    //SC_CHECK_MPI (mpiret);

    int provided;
    mpiret = sc_MPI_Init_thread (argc, argv, sc_MPI_THREAD_FUNNELED, &provided);
    if (provided != sc_MPI_THREAD_FUNNELED) printf("Recieved mpi_init_thread level %d\n", provided);
    SC_CHECK_MPI (mpiret);

    sc_init (mpicomm, 0, 0, NULL, lp);
    p4est_init (NULL, lp);
#endif
}

void
fclaw_mpi_finalize (void)
{
    int mpiret;

    sc_finalize ();

    mpiret = sc_MPI_Finalize ();
    SC_CHECK_MPI (mpiret);
}
