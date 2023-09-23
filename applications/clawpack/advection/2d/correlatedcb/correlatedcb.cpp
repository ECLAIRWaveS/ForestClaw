/*
Copyright (c) 2012-2023 Carsten Burstedde, Donna Calhoun, Scott Aiton
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

#include "correlatedcb_user.h"

void create_domain(fclaw2d_global_t *glob)
{
    const fclaw_options_t* fclaw_opt = fclaw2d_get_options(glob);

    /* Used locally */
    double pi = M_PI;

    double rotate[2];
    rotate[0] = pi*fclaw_opt->theta/180.0;
    rotate[1] = 0;

    fclaw2d_map_context_t *cont = NULL;
    fclaw2d_domain_t *domain = NULL;


    const user_options_t *user_opt = correlatedcb_get_options(glob);
    switch (user_opt->mapping) {
    case 0:
        domain =
            fclaw2d_domain_new_cubedsphere (glob->mpicomm,
                                            fclaw_opt->minlevel);
        cont = fclaw2d_map_new_cubedsphere (fclaw_opt->scale, rotate);
        break;
    case 1:
        domain =
            fclaw2d_domain_new_twosphere (glob->mpicomm, fclaw_opt->minlevel);
        cont = fclaw2d_map_new_pillowsphere (fclaw_opt->scale, rotate);
        break;
    default:
        SC_ABORT_NOT_REACHED (); /* must be checked in torus_checkparms */
    }
    /* Store mapping in the glob */
    fclaw2d_global_store_map (glob, cont);            

    /* Store the domain in the glob */
    fclaw2d_global_store_domain(glob, domain);

    /* print out some info */
    fclaw2d_domain_list_levels(domain, FCLAW_VERBOSITY_ESSENTIAL);
    fclaw2d_domain_list_neighbors(domain, FCLAW_VERBOSITY_DEBUG);
}


static
void run_program(fclaw2d_global_t* glob)
{
    /* ---------------------------------------------------------------
       Set domain data.
       --------------------------------------------------------------- */
    fclaw2d_domain_data_new(glob->domain);

    const user_options_t  *user_opt = correlatedcb_get_options(glob);

    /* Initialize virtual table for ForestClaw */
    fclaw2d_vtables_initialize(glob);

    /* Initialize virtual tables for solvers */
    if (user_opt->claw_version == 4)
    {
        fc2d_clawpack46_solver_initialize(glob);
    }
    else if (user_opt->claw_version == 5)
    {
        fc2d_clawpack5_solver_initialize(glob);
    }

    correlatedcb_link_solvers(glob);

    /* ---------------------------------------------------------------
       Run
       --------------------------------------------------------------- */
    fclaw2d_initialize(glob);
    fclaw2d_run(glob);
    fclaw2d_finalize(glob);
}


int
main (int argc, char **argv)
{
    /* Initialize application */
    fclaw_app_t *app = fclaw_app_new (&argc, &argv, NULL);

    fclaw2d_clawpatch_options_t *clawpatch_opt;
    fc2d_clawpack46_options_t   *claw46_opt;
    fc2d_clawpack5_options_t    *claw5_opt;
    fclaw_options_t *fclaw_opt;
    user_options_t *user_opt; 

    fclaw_opt =      fclaw_options_register(app,  NULL, "fclaw_options.ini");
    clawpatch_opt =   fclaw2d_clawpatch_options_register(app, "clawpatch",  "fclaw_options.ini");
    claw46_opt =        fc2d_clawpack46_options_register(app, "clawpack46", "fclaw_options.ini");
    claw5_opt =          fc2d_clawpack5_options_register(app, "clawpack5",  "fclaw_options.ini");
    user_opt =             correlatedcb_options_register(app, "fclaw_options.ini");  

    /* Create new options packages */
    sc_options_t *options = fclaw_app_get_options (app);

    /* Read configuration file(s) and command line, and process options */
    int retval = fclaw_options_read_from_file(options);

    int first_arg;
    fclaw_exit_type_t vexit;
    vexit =  fclaw_app_options_parse (app, &first_arg,"fclaw_options.ini.used");

    if (!vexit)
    {
        /* Options have been checked and are valid */
        /* Create glob */
        int size, rank;
        sc_MPI_Comm mpicomm = fclaw_app_get_mpi_size_rank (app, &size, &rank);
        fclaw2d_global_t *glob = fclaw2d_global_new_comm (mpicomm, size, rank);

        /* Store option packages in glob */
        fclaw2d_options_store           (glob, fclaw_opt);
        fclaw2d_clawpatch_options_store (glob, clawpatch_opt);
        fc2d_clawpack46_options_store   (glob, claw46_opt);
        fc2d_clawpack5_options_store    (glob, claw5_opt);
        correlatedcb_options_store      (glob, user_opt);

        /* Create domain and store domain in glob */
        create_domain(glob);

        /* Run the program */
        run_program(glob);
        
        fclaw2d_global_destroy(glob);
    }
    
    fclaw_app_destroy (app);

    return 0;
}


