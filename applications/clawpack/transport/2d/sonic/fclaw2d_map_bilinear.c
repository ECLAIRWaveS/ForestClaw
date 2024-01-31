/* Cartesian grid, tranformed to Ax + b */

#include <fclaw_map.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#define MAPC2M_BILINEAR FCLAW_F77_FUNC(mapc2m_bilinear,MAPC2M_BILINEAR)

void MAPC2M_BILINEAR(int* blockno, double* xc, double *yc,
                     double *xp, double *yp, double *zp,
                     double *center);


static int
fclaw2d_map_query_bilinear (fclaw_map_context_t * cont, int query_identifier)
{
    switch (query_identifier)
    {
    case FCLAW_MAP_QUERY_IS_USED:
        return 1;
    case FCLAW_MAP_QUERY_IS_SCALEDSHIFT:
        return 1;
    case FCLAW_MAP_QUERY_IS_AFFINE:
        return 1;
    case FCLAW_MAP_QUERY_IS_NONLINEAR:
        return 0;
    case FCLAW_MAP_QUERY_IS_GRAPH:
        return 0;
    case FCLAW_MAP_QUERY_IS_PLANAR:
        return 1;
    case FCLAW_MAP_QUERY_IS_ALIGNED:
        return 0;
    case FCLAW_MAP_QUERY_IS_FLAT:
        return 1;
    case FCLAW_MAP_QUERY_IS_DISK:
        return 0;
    case FCLAW_MAP_QUERY_IS_SPHERE:
        return 0;
    case FCLAW_MAP_QUERY_IS_PILLOWDISK:
        return 0;
    case FCLAW_MAP_QUERY_IS_SQUAREDDISK:
        return 0;
    case FCLAW_MAP_QUERY_IS_PILLOWSPHERE:
        return 0;
    case FCLAW_MAP_QUERY_IS_CUBEDSPHERE:
        return 0;
    case FCLAW_MAP_QUERY_IS_FIVEPATCH:
        return 0;
    case FCLAW_MAP_QUERY_IS_BILINEAR:
        return 1;
    case FCLAW_MAP_QUERY_IS_BRICK:
        return 0;
    default:
        printf("\n");
        printf("fclaw2d_map_query_bilinear (fclaw2d_map_bilinear.c) : "\
               "Query id not identified;  Maybe the query is not up to "\
               "date?\nSee fclaw2d_map_bilinear.h.\n");
        printf("Requested query id : %d\n",query_identifier);
        SC_ABORT_NOT_REACHED ();
    }
    return 0;
}


static void
fclaw2d_map_c2m_bilinear(fclaw_map_context_t * cont, int blockno,
                         double xc, double yc,
                         double *xp, double *yp, double *zp)
{
    /* Brick mapping to computational coordinates [0,1]x[0,1] */
    double center[2];

    /* Don't scale brick domain into [0,1].  Instead, mapping is more conveniently
       implemented using native block coordinates, where each block is in 
       [0,1]x[0,1] */
#if 0    
    FCLAW2D_MAP_BRICK2C(&cont,&blockno,&xc,&yc,&xc1,&yc1,&zc1);
    xc1 = xc;
    yc1 = yc;
#endif    


    center[0] = cont->user_double[0];
    center[1] = cont->user_double[1];

    /* Map to unit square in [-1,1] x [-1,1], with middle point shifted to 
       create four bilinear maps. */
    MAPC2M_BILINEAR(&blockno,&xc,&yc,xp,yp,zp,center);

    scale_map(cont, xp,yp,zp);
    /* rotate_map(cont, xp,yp,zp); */
    shift_map(cont, xp,yp,zp);
}


fclaw_map_context_t* fclaw2d_map_new_bilinear(fclaw_map_context_t *brick,
                                                const double scale[],
                                                const double shift[],
                                                const double rotate[],
                                                const double center[])
{
    fclaw_map_context_t *cont;

    cont = FCLAW_ALLOC_ZERO(fclaw_map_context_t, 1);
    cont->query = fclaw2d_map_query_bilinear;
    cont->mapc2m = fclaw2d_map_c2m_bilinear;
    cont->brick = brick;

    cont->user_double[0] = center[0];
    cont->user_double[1] = center[1];

    set_scale(cont,scale);
    set_shift(cont,shift);
    set_rotate(cont,rotate);

    /* NOTE : On coarse grids, moving the center too close to one of the edges can lead
       to problems with the metric terms;  lines will cross and ghost cells can be messed
       up.  For mx=8, center should be within 0.8 of the boundary.  See map_debug2.m */
    FILE* fp = fopen("center.dat","w");
    fprintf(fp,"%24.16f %24.16f",center[0],center[1]);
    fclose(fp);


    return cont;
}

#ifdef __cplusplus
#if 0
{
#endif
}
#endif
