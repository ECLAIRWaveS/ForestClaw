!> @file
!! check to see if value exceeds threshold

!  --------------------------------------------------------------
!> @brief Check if the difference exceeds the threshold
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
integer function fclaw3d_clawpatch_difference_exceeds_th(blockno, meqn, & 
                                     qval,qmin,qmax,quad, & 
                                     dx,dy,dz,xc,yc,zc,ivar_threshold, threshold,&
                                     init_flag, is_ghost)
    implicit none
    
    integer :: meqn, ivar_threshold
    double precision :: qval(meqn),qmin(meqn),qmax(meqn),threshold
    double precision :: quad(-1:1,-1:1,-1:1,meqn)
    double precision :: dx,dy, dz, xc, yc, zc
    integer :: blockno, init_flag
    logical(kind=4) :: is_ghost

    double precision :: dqx, dqy, dqz, dq
    integer :: refine
    integer :: mq

    if (is_ghost) then
!!      # quad may have uninitialized values;  test inconclusive
        fclaw3d_clawpatch_difference_exceeds_th = -1
        return
    endif

    mq = ivar_threshold

    dqx = abs(quad(1,0,0,mq) - quad(-1,0,0,mq))
    dqy = abs(quad(0,1,0,mq) - quad(0,-1,0,mq))
    dqz = abs(quad(0,0,1,mq) - quad(0,0,-1,mq))
    dq  = max(dqx, dqy, dqz)

    refine = 0
    if (dq .gt. threshold) then
        refine = 1
    endif

    fclaw3d_clawpatch_difference_exceeds_th = refine

end function fclaw3d_clawpatch_difference_exceeds_th
