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

#ifndef FCLAW2D_DOMAIN_H
#define FCLAW2D_DOMAIN_H

#include <forestclaw2d.h>
#include <fclaw_timer.h>
#include <fclaw2d_partition.h>
#include <fclaw2d_map.h>


#ifdef __cplusplus
extern "C"
{
#if 0
}                               /* need this because indent is dumb */
#endif
#endif



typedef struct fclaw2d_domain_data
{
    /* Debug counters and timers */
    int count_set_clawpatch, count_delete_clawpatch;
    int count_amr_advance, count_ghost_exchange, count_amr_regrid;
    int is_latest_domain;
    fclaw2d_timer_t timers[FCLAW2D_TIMER_COUNT];

    /* Time at start of each subcycled time step */
    double curr_time;

    /* This should not be copied, but needs to be redone for every new domain */
    fclaw2d_domain_exchange_t *domain_exchange;

}
fclaw2d_domain_data_t;

void
init_domain_data(fclaw2d_domain_t *domain);

void
delete_domain_data(fclaw2d_domain_t* domain);

fclaw2d_domain_data_t*
get_domain_data(fclaw2d_domain_t *domain);

void
copy_domain_data(fclaw2d_domain_t *old_domain,
                 fclaw2d_domain_t *new_domain);

fclaw_app_t*
fclaw2d_domain_get_app(fclaw2d_domain_t* domain);

void
fclaw2d_domain_set_app(fclaw2d_domain_t* domain,
                       fclaw_app_t* app);

int
fclaw2d_domain_get_num_patches(fclaw2d_domain_t* domain);

void
set_domain_time(fclaw2d_domain_t *domain, double time);

double
get_domain_time(fclaw2d_domain_t *domain);

int
num_patches(fclaw2d_domain_t *domain, int level,
            int include_shadow);

const amr_options_t*
fclaw2d_forestclaw_get_options(fclaw2d_domain_t *domain);

void*
fclaw2d_domain_get_user_options(fclaw2d_domain_t* domain);

const amr_options_t*
get_domain_parms(fclaw2d_domain_t *domain);

fclaw2d_map_context_t* get_map_context(fclaw2d_domain_t* domain);


#ifdef __cplusplus
#if 0
{                               /* need this because indent is dumb */
#endif
}
#endif



#endif