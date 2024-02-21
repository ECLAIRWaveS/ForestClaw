/*
Copyright (c) 2012-2021 Carsten Burstedde, Donna Calhoun
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

#ifndef TRANSPORT_USER_FORT_H
#define TRANSPORT_USER_FORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
/* Fix syntax highlighting */
#endif

/* ------------------------------------ SETAUX ---------------------------------------- */

#define TRANSPORT46_SETAUX_MANIFOLD FCLAW_F77_FUNC(transport46_setaux_manifold, \
                                                   TRANSPORT46_SETAUX_MANIFOLD)

void TRANSPORT46_SETAUX_MANIFOLD(const int* blockno, const int* mx, const int* my,
                                 const int* mbc, 
                                 const double* xlower, const double* ylower,
                                 const double* dx, const double* dy, 
                                 double area[],double edgelengths[],
                                 double aux[],const int* maux);

#define TRANSPORT5_SETAUX_MANIFOLD FCLAW_F77_FUNC(transport5_setaux_manifold, \
                                                   TRANSPORT5_SETAUX_MANIFOLD)

void TRANSPORT5_SETAUX_MANIFOLD(const int* blockno, const int* mx, const int* my,
                                 const int* mbc, 
                                 const double* xlower, const double* ylower,
                                 const double* dx, const double* dy, 
                                 double area[],double edgelengths[],
                                 double aux[],const int* maux);


/* ------------------------------- SET_VELOCITY --------------------------------------- */

#define TRANSPORT46_SET_VELOCITY FCLAW_F77_FUNC(transport46_set_velocity, \
                                             TRANSPORT46_SET_VELOCITY)

void TRANSPORT46_SET_VELOCITY(const int* blockno, const int* mx, const int* my,
                              const int* mbc, const double* dx, const double* dy,
                              const double* xlower, const double* ylower,
                              const double *t, double xp[], double yp[], 
                              double zp[], double xnormals[],double ynormals[],
                              double surfnormals[], double aux[],const int* maux);




#define TRANSPORT5_SET_VELOCITY FCLAW_F77_FUNC(transport5_set_velocity, \
                                             TRANSPORT5_SET_VELOCITY)

void TRANSPORT5_SET_VELOCITY(const int* blockno, const int* mx, const int* my,
                              const int* mbc, const double* dx, const double* dy,
                              const double* xlower, const double* ylower,
                              const double *t, double xp[], double yp[], 
                              double zp[], double xnormals[],double ynormals[],
                              double surfnormals[], double aux[],const int* maux);



/* ------------------------------- Riemann solvers ------------------------------------ */

#define RPN2CONS_UPDATE_MANIFOLD FCLAW_F77_FUNC(rpn2cons_update_manifold, \
                                                RPN2CONS_UPDATE_MANIFOLD)

void RPN2CONS_UPDATE_MANIFOLD(const int* meqn, const int* maux, 
                              const int* idir, const int* iface,
                              double q[], double aux_center[], 

                              double aux_edge[], double flux[]);

#define CLAWPACK46_RPN2CONS_FW_MANIFOLD FCLAW_F77_FUNC( \
                          clawpack46_rpn2cons_fw_manifold, \
                          CLAWPACK46_RPN2CONS_FW_MANIFOLD)
void CLAWPACK46_RPN2CONS_FW_MANIFOLD(const int* ixy, const int* maxm, const int* meqn, 
                          const int* mwaves, const int* maux,
                          const int* mbc, const int* mx, double ql[], double qr[],
                          double auxl[], double auxr[], double fwave[],
                          double s[], double amdq[], double apdq[]);


#define CLAWPACK46_RPT2CONS_MANIFOLD FCLAW_F77_FUNC( \
                       clawpack46_rpt2cons_manifold, \
                       CLAWPACK46_RPT2CONS_MANIFOLD)

void CLAWPACK46_RPT2CONS_MANIFOLD(const int* ixy, const int* maxm, 
                                  const int* meqn, const int* mwaves,
                                  const int* maux, const int* mbc, const int* mx, 
                                  double ql[], double qr[],
                                  double aux1[], double aux2[], 
                                  double aux3[], const int* imp,
                                  double dsdq[], double bmasdq[], double bpasdq[]);

#define CLAWPACK5_RPN2CONS_MANIFOLD    FCLAW_F77_FUNC( \
                            clawpack5_rpn2cons_manifold,\
                            CLAWPACK5_RPN2CONS_MANIFOLD)
void CLAWPACK5_RPN2CONS_MANIFOLD(const int* ixy,
                                 const int* maxm, const int* meqn,
                                 const int* mwaves, const int* maux,
                                 const int* mbc,const int* mx,
                                 double ql[], double qr[], 
                                 double auxl[], double auxr[],
                                 double wave[], double s[],
                                 double amdq[], double apdq[]);

#define CLAWPACK5_RPT2CONS_MANIFOLD  FCLAW_F77_FUNC( \
                     clawpack5_rpt2cons_manifold,  \
                     CLAWPACK5_RPT2CONS_MANIFOLD)
void CLAWPACK5_RPT2CONS_MANIFOLD(const int* ixy, const int* imp,
                                 const int* maxm, const int* meqn,
                                 const int* mwaves, const int* maux,
                                 const int* mbc,const int* mx,
                                 double ql[], double qr[],
                                 double aux1[], double aux2[],
                                 double aux3[],  double asdq[],
                                 double bmasdq[], double bpasdq[]);


/* ------------------------------------ Tagging  -------------------------------------- */

#define USER_EXCEEDS_THRESHOLD FCLAW_F77_FUNC(user_exceeds_threshold, \
                                              USER_EXCEEDS_THRESHOLD)

int USER_EXCEEDS_THRESHOLD(int* blockno,
                           double qval[], 
                           double* qmin, double *qmax,
                           double quad[], 
                           double *dx, double *dy, 
                           double *xc, double *yc, 
                           int* tag_threshold, 
                           int* init_flag,
                           int* is_ghost);

#ifdef __cplusplus
}
#endif

#endif
