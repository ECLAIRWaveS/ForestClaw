/*
Copyright (c) 2012-2022 Carsten Burstedde, Donna Calhoun, Scott Aiton
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

#ifndef FCLAW_DIAGNOSTICS_H
#define FCLAW_DIAGNOSTICS_H

#ifdef __cplusplus
extern "C"
/**
 * @file fclaw_diagnostics.h
 * @brief Header file containing declarations for diagnostic functions and structures.
 */

#ifndef FCLAW_DIAGNOSTICS_H
#define FCLAW_DIAGNOSTICS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct fclaw_diagnostics_accumulator
 * @brief Structure representing the accumulator for diagnostic information.
 */
struct fclaw_diagnostics_accumulator
{
    void* patch_accumulator; /**< Pointer to the patch accumulator */
    void* solver_accumulator; /**< Pointer to the solver accumulator */
    void* user_accumulator; /**< Pointer to the user accumulator */
    void* gauge_accumulator; /**< Pointer to the gauge accumulator */
    void* ray_accumulator; /**< Pointer to the ray accumulator */
};

/**
 * @typedef fclaw_diagnostics_initialize_t
 * @brief Function pointer type for the initialization of diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @param acc Pointer to the accumulator.
 */
typedef void (*fclaw_diagnostics_initialize_t)(struct fclaw_global *glob, void** acc);

/**
 * @typedef fclaw_diagnostics_compute_t
 * @brief Function pointer type for the computation of diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @param acc Pointer to the accumulator.
 */
typedef void (*fclaw_diagnostics_compute_t)(struct fclaw_global *glob, void* acc);

/**
 * @typedef fclaw_diagnostics_gather_t
 * @brief Function pointer type for the gathering of diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @param acc Pointer to the accumulator.
 * @param init_flag Flag indicating whether the gathering is for initialization.
 */
typedef void (*fclaw_diagnostics_gather_t)(struct fclaw_global *glob, void* acc, int init_flag);

/**
 * @typedef fclaw_diagnostics_reset_t
 * @brief Function pointer type for the resetting of diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @param acc Pointer to the accumulator.
 */
typedef void (*fclaw_diagnostics_reset_t)(struct fclaw_global *glob, void* acc);

/**
 * @typedef fclaw_diagnostics_finalize_t
 * @brief Function pointer type for the finalization of diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @param acc Pointer to the accumulator.
 */
typedef void (*fclaw_diagnostics_finalize_t)(struct fclaw_global *glob, void** acc);

/**
 * @struct fclaw_diagnostics_vtable
 * @brief Structure representing the virtual table for diagnostic functions.
 */
struct fclaw_diagnostics_vtable
{
    fclaw_diagnostics_initialize_t patch_init_diagnostics; /**< Function pointer for patch diagnostic initialization */
    fclaw_diagnostics_compute_t patch_compute_diagnostics; /**< Function pointer for patch diagnostic computation */
    fclaw_diagnostics_gather_t patch_gather_diagnostics; /**< Function pointer for patch diagnostic gathering */
    fclaw_diagnostics_reset_t patch_reset_diagnostics; /**< Function pointer for patch diagnostic resetting */
    fclaw_diagnostics_finalize_t patch_finalize_diagnostics; /**< Function pointer for patch diagnostic finalization */
    fclaw_diagnostics_initialize_t solver_init_diagnostics; /**< Function pointer for solver diagnostic initialization */
    fclaw_diagnostics_compute_t solver_compute_diagnostics; /**< Function pointer for solver diagnostic computation */
    fclaw_diagnostics_gather_t solver_gather_diagnostics; /**< Function pointer for solver diagnostic gathering */
    fclaw_diagnostics_reset_t solver_reset_diagnostics; /**< Function pointer for solver diagnostic resetting */
    fclaw_diagnostics_finalize_t solver_finalize_diagnostics; /**< Function pointer for solver diagnostic finalization */
    fclaw_diagnostics_initialize_t gauges_init_diagnostics; /**< Function pointer for gauge diagnostic initialization */
    fclaw_diagnostics_compute_t gauges_compute_diagnostics; /**< Function pointer for gauge diagnostic computation */
    fclaw_diagnostics_gather_t gauges_gather_diagnostics; /**< Function pointer for gauge diagnostic gathering */
    fclaw_diagnostics_reset_t gauges_reset_diagnostics; /**< Function pointer for gauge diagnostic resetting */
    fclaw_diagnostics_finalize_t gauges_finalize_diagnostics; /**< Function pointer for gauge diagnostic finalization */
    fclaw_diagnostics_initialize_t ray_init_diagnostics; /**< Function pointer for ray diagnostic initialization */
    fclaw_diagnostics_compute_t ray_compute_diagnostics; /**< Function pointer for ray diagnostic computation */
    fclaw_diagnostics_gather_t ray_gather_diagnostics; /**< Function pointer for ray diagnostic gathering */
    fclaw_diagnostics_reset_t ray_reset_diagnostics; /**< Function pointer for ray diagnostic resetting */
    fclaw_diagnostics_finalize_t ray_finalize_diagnostics; /**< Function pointer for ray diagnostic finalization */
    fclaw_diagnostics_initialize_t user_init_diagnostics; /**< Function pointer for user-defined diagnostic initialization */
    fclaw_diagnostics_compute_t user_compute_diagnostics; /**< Function pointer for user-defined diagnostic computation */
    fclaw_diagnostics_gather_t user_gather_diagnostics; /**< Function pointer for user-defined diagnostic gathering */
    fclaw_diagnostics_reset_t user_reset_diagnostics; /**< Function pointer for user-defined diagnostic resetting */
    fclaw_diagnostics_finalize_t user_finalize_diagnostics; /**< Function pointer for user-defined diagnostic finalization */
    int is_set; /**< Flag indicating whether the virtual table is set */
};

/**
 * @brief Retrieves the virtual table for diagnostic functions.
 * @param glob Pointer to the fclaw_global structure.
 * @return Pointer to the fclaw_diagnostics_vtable structure.
 */
fclaw_diagnostics_vtable_t* fclaw_diagnostics_vt(struct fclaw_global* glob);

/**
 * @brief Retrieves the accumulator for diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @return Pointer to the fclaw_diagnostics_accumulator structure.
 */
fclaw_diagnostics_accumulator_t* fclaw_diagnostics_get_acc(struct fclaw_global* glob);

/**
 * @brief Initializes the virtual table for diagnostic functions.
 * @param glob Pointer to the fclaw_global structure.
 */
void fclaw_diagnostics_vtable_initialize(struct fclaw_global* glob);

/**
 * @brief Computes the global minimum value within a domain.
 * @param domain Pointer to the fclaw_domain structure.
 * @param d The value to compute the minimum for.
 * @return The global minimum value.
 */
double fclaw_domain_global_minimum(struct fclaw_domain* domain, double d);

/**
 * @brief Initializes the diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 */
void fclaw_diagnostics_initialize(struct fclaw_global* glob);

/**
 * @brief Gathers the diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 * @param init_flag Flag indicating whether the gathering is for initialization.
 */
void fclaw_diagnostics_gather(struct fclaw_global* glob, int init_flag);

/**
 * @brief Resets the diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 */
void fclaw_diagnostics_reset(struct fclaw_global* glob);

/**
 * @brief Finalizes the diagnostic information.
 * @param glob Pointer to the fclaw_global structure.
 */
void fclaw_diagnostics_finalize(struct fclaw_global* glob);

#ifdef __cplusplus
}
#endif

#endif /* FCLAW_DIAGNOSTICS_H */
{
#endif

struct fclaw_global;
struct fclaw_domain;

typedef struct fclaw_diagnostics_vtable  fclaw_diagnostics_vtable_t;

typedef struct fclaw_diagnostics_accumulator fclaw_diagnostics_accumulator_t;

struct fclaw_diagnostics_accumulator
{
    void* patch_accumulator;
    void* solver_accumulator;
    void* user_accumulator;
    void* gauge_accumulator;
    void* ray_accumulator;
};

/* Diagnostic information */
typedef void (*fclaw_diagnostics_initialize_t)(struct fclaw_global *glob,
                                                 void** acc);

typedef void (*fclaw_diagnostics_compute_t)(struct fclaw_global *glob,
                                              void* acc);

typedef void (*fclaw_diagnostics_gather_t)(struct fclaw_global *glob,
                                             void* acc,
                                             int init_flag);

typedef void (*fclaw_diagnostics_reset_t)(struct  fclaw_global *glob,
                                            void* acc);

typedef void (*fclaw_diagnostics_finalize_t)(struct  fclaw_global *glob,
                                               void** acc);

struct fclaw_diagnostics_vtable
{
    /* patch diagnostic functions (error, conservation, area, etc) */
    fclaw_diagnostics_initialize_t     patch_init_diagnostics;
    fclaw_diagnostics_compute_t        patch_compute_diagnostics;
    fclaw_diagnostics_gather_t         patch_gather_diagnostics;
    fclaw_diagnostics_reset_t          patch_reset_diagnostics;
    fclaw_diagnostics_finalize_t       patch_finalize_diagnostics;

    /* gauge diagnostic functions  */
    fclaw_diagnostics_initialize_t     solver_init_diagnostics;
    fclaw_diagnostics_compute_t        solver_compute_diagnostics;
    fclaw_diagnostics_gather_t         solver_gather_diagnostics;
    fclaw_diagnostics_reset_t          solver_reset_diagnostics;
    fclaw_diagnostics_finalize_t       solver_finalize_diagnostics;

    /* solver diagnostic functions (other solver functions) */
    fclaw_diagnostics_initialize_t     gauges_init_diagnostics;
    fclaw_diagnostics_compute_t        gauges_compute_diagnostics;
    fclaw_diagnostics_gather_t         gauges_gather_diagnostics;
    fclaw_diagnostics_reset_t          gauges_reset_diagnostics;
    fclaw_diagnostics_finalize_t       gauges_finalize_diagnostics;

    /* ray defined diagnostics */
    fclaw_diagnostics_initialize_t     ray_init_diagnostics;
    fclaw_diagnostics_compute_t        ray_compute_diagnostics;
    fclaw_diagnostics_gather_t         ray_gather_diagnostics;
    fclaw_diagnostics_reset_t          ray_reset_diagnostics;
    fclaw_diagnostics_finalize_t       ray_finalize_diagnostics;

    /* user defined diagnostics */
    fclaw_diagnostics_initialize_t     user_init_diagnostics;
    fclaw_diagnostics_compute_t        user_compute_diagnostics;
    fclaw_diagnostics_gather_t         user_gather_diagnostics;
    fclaw_diagnostics_reset_t          user_reset_diagnostics;
    fclaw_diagnostics_finalize_t       user_finalize_diagnostics;

    int is_set;
};

fclaw_diagnostics_vtable_t* fclaw_diagnostics_vt(struct fclaw_global* glob);


/**
 * @brief get the accumulator for diagnostic information
 *
 * @param glob global context 
 * @return fclaw_diagnostics_accumulator_t accumulator
 */
fclaw_diagnostics_accumulator_t* fclaw_diagnostics_get_acc(struct fclaw_global* glob);

void fclaw_diagnostics_vtable_initialize(struct fclaw_global* glob);

double fclaw_domain_global_minimum (struct fclaw_domain* domain, double d);

void fclaw_diagnostics_initialize(struct fclaw_global *glob);

void fclaw_diagnostics_gather(struct fclaw_global *glob, int init_flag);

void fclaw_diagnostics_reset(struct fclaw_global *glob);

void fclaw_diagnostics_finalize(struct fclaw_global *glob);

#ifdef __cplusplus
}
#endif

#endif
