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

#ifndef FCLAW2D_TYPEDEFS_H
#define FCLAW2D_TYPEDEFS_H

/* this header file must come first */
#include "fclaw2d_defs.H"

#include "fclaw_options.h"
#include "forestclaw2d.h"


typedef struct fclaw2d_level_time_data fclaw2d_level_time_data_t;

/* -----------------------------------------------------------
   Data needed for time stepping
   ----------------------------------------------------------- */
struct fclaw2d_level_time_data
{
    /* Single step data. This always has to be set. */
    double dt;
    double t_initial;
    double t_level;
    double t_coarse;

    /* Needed for explicit CFL limited schemes */
    double maxcfl;

    /* Extra data that might be needed for more complicated time stepping.
     * Not always set.
     */
    double alpha;               /* Fraction of coarser dt completed. */
    double dt_coarse;
    bool is_coarsest;
    bool fixed_dt;
};

#endif
