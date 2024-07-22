double precision function heat_fort_eval_bc(iface,t,x,y)
    implicit none

    integer iface
    double precision x,y,t

    integer bctype(0:3)
    common /comm_bc/ bctype

    integer normals(0:3,2)
    double precision q,grad(2), qn, a,b

    data normals /-1,1,0,0,0,0,-1,1/

    call heat_qexact_gradient(x,y,q,grad)

    !! q_n = grad q \cdot n
    qn = normals(iface,1)*grad(1) + normals(iface,2)*grad(2)

    !! bc_type is set in options .ini file as [multigrid] boundary_conditions
    if (bctype(iface) .eq. 1) then
        a = 1
        b = 0
    elseif (bctype(iface) .eq. 2) then
        a = 0
        b = 1
    else
        write(6,*) 'heat_eval_bc : Invalid bctype'
        stop
    endif

    heat_fort_eval_bc = a*q + b*qn
    !!heat_fort_eval_bc = 0

    return
    
end function heat_fort_eval_bc


double precision function heat_neumann(iface,t,x,y)
    implicit none

    integer iface
    double precision t,x,y

    if (iface .eq. 0) then
        heat_neumann = 0
    elseif (iface .eq. 1) then
        heat_neumann = 0
    else
        heat_neumann = 0
    endif

end function heat_neumann