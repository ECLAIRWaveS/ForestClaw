double precision function fdisc(blockno,xc,yc)
    implicit none

    double precision xc,yc
    integer blockno

    double precision pi, pi2, deg2rad
    common /compi/ pi, pi2, deg2rad

    integer init_cond
    common /swe_initcond/ init_cond

    double precision ring_inner, ring_outer, center(3)
    common /swe_initcond_parms2/ ring_inner, ring_outer, center

    integer ring_units
    common /swe_initcond_parms3/ ring_units

    integer*8 cont, fclaw_map_get_context

    double precision xp, yp, zp, rp, a, m, d
    double precision phi, ri, ro


    cont = fclaw_map_get_context()

    call fclaw_map_2d_c2m(cont,blockno,xc,yc,xp,yp,zp)

    rp = sqrt(xp**2 + yp**2 + zp**2)

    ri = ring_inner
    ro = ring_outer
    if (ring_units .gt. 0) then
        if (ring_units .eq. 1) then
            !! Convert degrees to radians
            ri = ri*deg2rad
            ro = ro*deg2rad
        elseif (ring_units .eq. 2) then
            !! Compute Gaussian distance to radians
            ri = ri/rp
            ro = ro/rp
        endif
    endif

    d = center(1)*xp + center(2)*yp + center(3)*zp
    if (abs(d) > 1) then
        write(6,*) 'fdisc : problem taking acos()'
        stop
    endif
    phi = acos(d)  
    a = abs(ro - ri)/2.0   !! Half-width of the ring
    m = (ro + ri)/2.0   !! Midpoint of the ring
    fdisc = abs(phi-m) - a
end


