!! # ----------------------------------------------------------
!! # Exchange routines - (i,j,mq) ordering
!! # ----------------------------------------------------------
!! # exchange_face_ghost
!! # exchange_corner_ghost
!! # exchange_phys_corner_ghost
!! # ----------------------------------------------------------


!! # Exchange edge ghost data with neighboring grid at same level.

subroutine fclaw3dx_clawpatch46_fort_copy_face(mx,my,mz,mbc, & 
           meqn,qthis, qneighbor, iface, transform_ptr)

    implicit none

    integer mx,my,mz,mbc,meqn,iface
    integer*8 transform_ptr
    double precision     qthis(1-mbc:mx+mbc,1-mbc:my+mbc,1-mbc:mz+mbc,meqn)
    double precision qneighbor(1-mbc:mx+mbc,1-mbc:my+mbc,1-mbc:mz+mbc,meqn)

    integer mq, k
    integer i1,j1, i2, j2

    integer a(2,2), f(2)

    call fclaw3dx_clawpatch_build_transform_same(transform_ptr, a, f)

    if (iface .eq. 0) then
       do mq = 1,meqn
          do k = 1,mz
            do j1 = 1,my
               do i1 = 1-mbc,0
                  ! Lower side
                  i2 = a(1,1)*i1 + a(1,2)*j1 + f(1)
                  j2 = a(2,1)*i1 + a(2,2)*j1 + f(2)
                  qthis(i1,j1,k,mq) = qneighbor(i2,j2,k,mq)
               enddo
            enddo
          enddo
       enddo
    else if (iface .eq. 1) then
       do mq = 1,meqn
          do k = 1,mz
            do j1 = 1,my
               do i1 = mx+1,mx+mbc
                  ! Upper side
                  i2 = a(1,1)*i1 + a(1,2)*j1 + f(1)
                  j2 = a(2,1)*i1 + a(2,2)*j1 + f(2)
                  qthis(i1,j1,k,mq) = qneighbor(i2,j2,k,mq)
               enddo
            enddo
          enddo
       enddo
    else if (iface .eq. 2) then
       do mq = 1,meqn
          do k = 1,mz
            do j1 = 1-mbc,0
               do i1 = 1,mx
                  ! left side
                  i2 = a(1,1)*i1 + a(1,2)*j1 + f(1)
                  j2 = a(2,1)*i1 + a(2,2)*j1 + f(2)
                  qthis(i1,j1,k,mq) = qneighbor(i2,j2,k,mq)
               enddo
            enddo
          enddo
       enddo
    else if (iface .eq. 3) then
       do mq = 1,meqn
          do k = 1,mz
            do j1 = my+1,my+mbc
               do i1 = 1,mx
                  ! right side
                  i2 = a(1,1)*i1 + a(1,2)*j1 + f(1)
                  j2 = a(2,1)*i1 + a(2,2)*j1 + f(2)
                  qthis(i1,j1,k,mq) = qneighbor(i2,j2,k,mq)
               enddo
            enddo
          enddo
       enddo
    endif



end subroutine fclaw3dx_clawpatch46_fort_copy_face

subroutine fclaw3dx_clawpatch46_fort_copy_corner(mx,my,mz,mbc,meqn, &
    qthis, qneighbor, this_icorner,transform_ptr)
    implicit none

    integer mx, my, mz, mbc, meqn, this_icorner
    integer*8 transform_ptr
    double precision     qthis(1-mbc:mx+mbc,1-mbc:my+mbc,1-mbc:mz+mbc,meqn)
    double precision qneighbor(1-mbc:mx+mbc,1-mbc:my+mbc,1-mbc:mz+mbc,meqn)

    integer mq, ibc, jbc
    integer i1, j1, i2, j2, k

    !! # Do exchanges for all corners
    mq_loop : do mq = 1,meqn
        k_loop : do k = 1,mz
            do ibc = 1,mbc
                do jbc = 1,mbc
                    if (this_icorner .eq. 0) then
                        i1 = 1-ibc
                        j1 = 1-jbc
                    elseif (this_icorner .eq. 1) then
                        i1 = mx+ibc
                        j1 = 1-jbc
                    elseif (this_icorner .eq. 2) then
                        i1 = 1 -ibc
                        j1 = my+jbc
                    else
                        i1 = mx+ibc
                        j1 = my+jbc
                    endif

                    !! # this routine is not yet complete, but the complete one
                    !! # can now be dropped in.
                    call fclaw2d_clawpatch_transform_corner(i1,j1,i2,j2, transform_ptr)
                    qthis(i1,j1,k,mq) = qneighbor(i2,j2,k,mq)
                end do
            end do
        end do k_loop
    end do mq_loop

end subroutine fclaw3dx_clawpatch46_fort_copy_corner
