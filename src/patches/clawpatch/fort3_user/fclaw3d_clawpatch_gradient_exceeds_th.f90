!> @file
!! check to see if value exceeds threshold

!  --------------------------------------------------------------
!> @brief Check if the gradient exceeds the threshold
!!
!! @param[in] blockno the block number
!! @param[in] qval the 
!! @param[in] qmin the minimum q value
!! @param[in] qmax the maximum q value
!! @param[in] quad the value and adjacent values of q
!! @param[in] dx, dy, dz the spacing in the x and y directions
!! @param[in] xc, yc, zc the coordinate of the cell
!! @param[in] threshold the threshold
!! @param[in] init_flag true if in init stage
!! @param[in] is_ghost true if cell is a ghost cell
!! @return 1 if exceeds threshold, 0 if not, -1 if inconclusive.
!  --------------------------------------------------------------
integer function fclaw3d_clawpatch_gradient_exceeds_th(blockno, meqn, & 
                                     qval,qmin,qmax,quad, & 
                                     dx,dy,dz, xc,yc,zc, ivar_threshold, threshold, &
                                     init_flag, is_ghost)
    implicit none
    
    integer :: meqn, ivar_threshold
    double precision :: qval(meqn),qmin(meqn),qmax(meqn),threshold
    double precision :: quad(-1:1,-1:1,-1:1,meqn)
    double precision :: dx,dy, dz, xc, yc, zc
    integer :: blockno, init_flag
    logical(kind=4) :: is_ghost

    double precision :: dqx, dqy, dqz
    double precision :: grad(3), dx2, dy2, dz2, ds


    double precision :: clawpatch_gradient_dot3
    integer :: refine
    integer :: mq

    mq = ivar_threshold


    if (is_ghost) then
!!      # quad may have uninitialized values.  Test is inconclusive
        fclaw3d_clawpatch_gradient_exceeds_th = -1
        return
    endif

    dx2 = 2*dx
    dy2 = 2*dy
    dz2 = 2*dz

    !! Only compute gradients for non-mapped grids
    dqx = (quad(1,0,0,mq) - quad(-1,0,0,mq))/dx2
    dqy = (quad(0,1,0,mq) - quad(0,-1,0,mq))/dy2
    dqz = (quad(0,0,1,mq) - quad(0,0,-1,mq))/dz2

    refine = 0
    grad(1) = dqx
    grad(2) = dqy
    grad(3) = dqz

    ds = sqrt(clawpatch_gradient_dot3(grad,grad))

    if (ds .gt. threshold) then
        refine = 1
    endif

    fclaw3d_clawpatch_gradient_exceeds_th = refine

end function fclaw3d_clawpatch_gradient_exceeds_th

!  --------------------------------------------------------------
!> @brief Dot product of two vector
!!
!! @param[in] u, v the vectors
!! @return the dot product
!  --------------------------------------------------------------
double precision function clawpatch_gradient_dot3(u,v)
    implicit none

    double precision :: u(3), v(3)

    clawpatch_gradient_dot3 = u(1)*v(1) + u(2)*v(2) + u(3)*v(3)

end function clawpatch_gradient_dot3

