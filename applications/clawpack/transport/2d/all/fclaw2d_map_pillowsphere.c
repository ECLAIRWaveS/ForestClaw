/* Pillow grid surface.  Matches p4est_connectivity_new_pillow (). */

#include <fclaw_map.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif


#define SPHERE_BASIS_COMPLETE FCLAW_F77_FUNC(sphere_basis_complete, \
                            SPHERE_BASIS_COMPLETE)

void SPHERE_BASIS_COMPLETE(const double* x, const double *y,
                              double t[], double tinv[], double uderivs[], 
                              const int* flag);

#define MAP2COMP FCLAW_F77_FUNC(map2comp,MAP2COMP)

void MAP2COMP(double* xp, double* yp, double* zp, double* xc1, double* yc1);


static int
fclaw2d_map_query_pillowsphere (fclaw_map_context_t * cont, int query_identifier)
{
    switch (query_identifier)
    {
    case FCLAW_MAP_QUERY_IS_USED:
        return 1;
        /* no break necessary after return statement */
    case FCLAW_MAP_QUERY_IS_SCALEDSHIFT:
        return 0;
    case FCLAW_MAP_QUERY_IS_AFFINE:
        return 0;
    case FCLAW_MAP_QUERY_IS_NONLINEAR:
        return 1;
    case FCLAW_MAP_QUERY_IS_CART:
        return 0;
    case FCLAW_MAP_QUERY_IS_GRAPH:
        return 0;
    case FCLAW_MAP_QUERY_IS_PLANAR:
        return 0;
    case FCLAW_MAP_QUERY_IS_ALIGNED:
        return 0;
    case FCLAW_MAP_QUERY_IS_FLAT:
        return 0;
    case FCLAW_MAP_QUERY_IS_DISK:
        return 0;
    case FCLAW_MAP_QUERY_IS_SPHERE:
        return 1;
    case FCLAW_MAP_QUERY_IS_PILLOWDISK:
        return 0;
    case FCLAW_MAP_QUERY_IS_SQUAREDDISK:
        return 0;
    case FCLAW_MAP_QUERY_IS_PILLOWSPHERE:
        return 1;
    case FCLAW_MAP_QUERY_IS_CUBEDSPHERE:
        return 0;
    case FCLAW_MAP_QUERY_IS_BRICK:
        return 0;
    default:
        printf("\n");
        printf("fclaw2d_map_query_pillowsphere (fclaw2d_map.c) : Query id not "\
               "identified;  Maybe the query is not up to date?\nSee "  \
               "fclaw2d_map_query_defs.h.\n");
        printf("Requested query id : %d\n",query_identifier);
        SC_ABORT_NOT_REACHED ();
    }
    return 0;
}



static void
fclaw2d_map_c2m_basis_pillowsphere(fclaw_map_context_t * cont,
                                   double xc, double yc, 
                                   double *t, double *tinv, 
                                   double *tderivs, int flag)
{
    SPHERE_BASIS_COMPLETE(&xc, &yc, t, tinv, tderivs, &flag);
}


static void
fclaw2d_map_c2m_pillowsphere (fclaw_map_context_t * cont, int blockno,
                              double xc, double yc,
                              double *xp, double *yp, double *zp)
{
    FCLAW_MAP_2D_C2M_PILLOWSPHERE(&blockno,&xc,&yc,xp,yp,zp);

#if 0
    scale_map(cont,xp,yp,zp); 
    rotate_map(cont,xp,yp,zp);
#endif    
}

fclaw_map_context_t *
    fclaw2d_map_new_pillowsphere(const double scale[],const double rotate[])
{
    fclaw_map_context_t *cont;

    cont = FCLAW_ALLOC_ZERO (fclaw_map_context_t, 1);
    cont->query = fclaw2d_map_query_pillowsphere;
    cont->mapc2m = fclaw2d_map_c2m_pillowsphere;
    cont->basis = fclaw2d_map_c2m_basis_pillowsphere;
    
    set_scale(cont,scale);
    set_rotate(cont,rotate);

    return cont;
}

#ifdef __cplusplus
#if 0
{
#endif
}
#endif
