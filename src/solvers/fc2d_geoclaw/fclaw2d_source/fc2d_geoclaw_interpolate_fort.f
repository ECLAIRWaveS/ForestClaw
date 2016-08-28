c     # ----------------------------------------------------------
c     # Interpolation routines - (i,j,mq) ordering
c     # ----------------------------------------------------------
c     # interpolate_face_ghost
c     # interpolate_corner_ghost
c     # interpolate_to_fine_patch
c     #
c     # Other routines :
c     # compute_slopes (for limited function reconstruction)
c     # fixcapaq (to preserve conservation)
c     #
c     # Note that fixcapaq is only used when regridding;  ghost
c     # cell interpolation is not conservative in the mapped case.
c     # (Should it be?  We are going to correct the flux mixmatch
c     # anyhow, so maybe the accuracy of the ghost cell values is
c     # more important.)
c     # ----------------------------------------------------------


c     # ----------------------------------------------------------
c     # This routine is used for both mapped and non-mapped
c     # cases.
c     # ----------------------------------------------------------
      subroutine fc2d_geoclaw_fort_interpolate_face(mx,my,mbc,meqn,
     &      qcoarse,qfine,maux,aux_coarse,aux_fine,mbathy,
     &      idir,iface_coarse,num_neighbors,refratio,igrid,
     &      transform_ptr)

      implicit none
      integer mx,my,mbc,meqn,refratio,igrid,idir,iface_coarse
      integer maux, mbathy
      integer num_neighbors
      integer*8 transform_ptr
      double precision qfine(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
      double precision qcoarse(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)

      double precision aux_coarse(maux,1-mbc:mx+mbc,1-mbc:my+mbc)
      double precision aux_fine(maux,1-mbc:mx+mbc,1-mbc:my+mbc)

      integer mq,r2, m
      integer i, ic1, ic2, ibc, ifine,i1
      integer j, jc1, jc2, jbc, jfine,j1
      integer ic_add, jc_add, ic, jc, mth
      double precision gradx, grady, qc, sl, sr, value
      double precision compute_slopes

c     # This should be refratio*refratio.
      integer rr2
      parameter(rr2 = 4)
      integer i2(0:rr2-1),j2(0:rr2-1)
      logical is_valid_interp
      logical skip_this_grid

      integer a(2,2)
      integer ii,jj,dc(2),df(2,0:rr2-1),iff,jff
      double precision shiftx(0:rr2-1),shifty(0:rr2-1)

      mth = 5
      r2 = refratio*refratio
      if (r2 .ne. rr2) then
         write(6,*) 'average_face_ghost (claw2d_utils.f) ',
     &         '  Refratio**2 is not equal to rr2'
         stop
      endif


      call build_transform(transform_ptr,a)

c     # This needs to be written for refratios .ne. 2.
      m = 0
      do jj = 0,1
         do ii = 0,1
c           # Direction on coarse grid
            dc(1) = ii
            dc(2) = jj

c           # Direction on fine grid (converted using metric). Divide
c           # by refratio to scale length to unit vector
            df(1,m) = (a(1,1)*dc(1) + a(1,2)*dc(2))/refratio
            df(2,m) = (a(2,1)*dc(1) + a(2,2)*dc(2))/refratio

c           # Map (0,1) to (-1/4,1/4) (locations of fine grid points)
            shiftx(m) = (ii-0.5d0)/2.d0
            shifty(m) = (jj-0.5d0)/2.d0
            m = m + 1
         enddo
      enddo
c     # Create map :

      do mq = 1,meqn
         if (idir .eq. 0) then
c           # this ensures that we get 'hanging' corners

            if (iface_coarse .eq. 0) then
               ic = 1
            elseif (iface_coarse .eq. 1) then
               ic = mx
            endif
            do jc = 1,mx
               i1 = ic
               j1 = jc
               call fclaw2d_transform_face_half(i1,j1,i2,j2,
     &               transform_ptr)
               skip_this_grid = .false.
               do m = 0,r2-1
                  if (.not. is_valid_interp(i2(m),j2(m),mx,my,mbc))
     &                  then
                     skip_this_grid = .true.
                     exit
                  endif
               enddo
               if (.not. skip_this_grid) then
                  qc = qcoarse(mq,ic,jc) + aux_coarse(mbathy,ic,jc)
c                 # Compute limited slopes in both x and y. Note we are not
c                 # really computing slopes, but rather just differences.
c                 # Scaling is accounted for in 'shiftx' and 'shifty', below.
                  sl = (qc - qcoarse(mq,ic-1,jc) -
     &                  aux_coarse(mbathy,ic-1,jc))
                  sr = (qcoarse(mq,ic+1,jc) +
     &                  aux_coarse(mbathy,ic+1,jc) - qc)
                  gradx = compute_slopes(sl,sr,mth)

                  sl = (qc - qcoarse(mq,ic,jc-1) -
     &                  aux_coarse(mbathy,ic,jc-1))
                  sr = (qcoarse(mq,ic,jc+1)+
     &                  aux_coarse(mbathy,ic,jc+1) - qc)
                  grady = compute_slopes(sl,sr,mth)

                  do m = 0,rr2-1
                     iff = i2(0) + df(1,m)
                     jff = j2(0) + df(2,m)
                     value = qc + gradx*shiftx(m) + grady*shifty(m)
     &                       -aux_fine(mbathy,iff,jff)
                     qfine(mq,iff,jff) = value
                  enddo
               endif
            enddo
         else
            if (iface_coarse .eq. 2) then
               jc = 1
            elseif (iface_coarse .eq. 3) then
               jc = my
            endif
            do ic = 1,mx
    1          i1 = ic
               j1 = jc
               call fclaw2d_transform_face_half(i1,j1,i2,j2,
     &               transform_ptr)
c              # ---------------------------------------------
c              # Two 'half-size' neighbors will be passed into
c              # this routine.  Only half of the coarse grid ghost
c              # indices will be valid for the particular grid
c              # passed in.  We skip those ghost cells that will
c              # have to be filled in by the other half-size
c              # grid.
c              # ---------------------------------------------
               skip_this_grid = .false.
               do m = 0,r2-1
                  if (.not. is_valid_interp(i2(m),j2(m),mx,my,mbc))
     &                  then
                     skip_this_grid = .true.
                     exit
                  endif
               enddo
               if (.not. skip_this_grid) then
                  qc = qcoarse(mq,ic,jc) + aux_coarse(mbathy,ic,jc)
c                 # Compute limited slopes in both x and y. Note we are not
c                 # really computing slopes, but rather just differences.
c                 # Scaling is accounted for in 'shiftx' and 'shifty', below.
                  sl = (qc - qcoarse(mq,ic-1,jc) -
     &                  aux_coarse(mbathy,ic-1,jc))
                  sr = (qcoarse(mq,ic+1,jc) +
     &                  aux_coarse(mbathy,ic+1,jc) - qc)
                  gradx = compute_slopes(sl,sr,mth)

                  sl = (qc - qcoarse(mq,ic,jc-1) -
     &                  aux_coarse(mbathy,ic,jc-1))
                  sr = (qcoarse(mq,ic,jc+1)+
     &                  aux_coarse(mbathy,ic,jc+1) - qc)
                  grady = compute_slopes(sl,sr,mth)

                  do m = 0,rr2-1
                     iff = i2(0) + df(1,m)
                     jff = j2(0) + df(2,m)
                     value = qc + gradx*shiftx(m) + grady*shifty(m)
     &                       -aux_fine(mbathy,iff,jff)
                     qfine(mq,iff,jff) = value
                  enddo
               endif                    !! Don't skip this grid
            enddo                       !! i loop
         endif                          !! end idir branch
      enddo                             !! endo mq loop

      end

      logical function check_indices(iff,jff,i2,j2)
      implicit none

      integer iff,jff,i2(0:3),j2(0:3)
      integer m
      logical found_iff, found_jff

      found_iff = .false.
      do m = 0,3
         if (i2(m) .eq. iff) then
            found_iff = .true.
            exit
         endif
      enddo

      found_jff = .false.
      do m = 0,3
         if (j2(m) .eq. jff) then
            found_jff = .true.
            exit
         endif
      enddo

      check_indices = found_iff .and. found_jff


      end

      logical function is_valid_interp(i,j,mx,my,mbc)
      implicit none
      integer i,j,mx, my, mbc

      logical i1, i2
      logical j1, j2

      i1 = 1-mbc .le. i .and. i .le. mx+mbc
      j1 = 1-mbc .le. j .and. j .le. my+mbc

      is_valid_interp = i1 .and. j1

      end

      subroutine fc2d_geoclaw_fort_interpolate_corner(mx,my,mbc,meqn,
     &      refratio,qcoarse,qfine,maux,aux_coarse,aux_fine,mbathy,
     &      icorner_coarse,transform_ptr)

      implicit none

      integer mx,my,mbc,meqn,icorner_coarse,refratio
      integer mbathy,maux
      integer*8 transform_ptr
      double precision qcoarse(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
      double precision qfine(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)

      double precision aux_coarse(maux,1-mbc:mx+mbc,1-mbc:my+mbc)
      double precision aux_fine(maux,1-mbc:mx+mbc,1-mbc:my+mbc)      

      integer ic, jc, mq, ibc,jbc, mth,i,j
      double precision qc, sl, sr, gradx, grady
      double precision compute_slopes, value

c     # This should be refratio*refratio.
      integer i1,j1,m, r2
      integer rr2
      parameter(rr2 = 4)
      integer i2(0:rr2-1),j2(0:rr2-1)

      integer a(2,2)
      integer ii,jj,iff,jff,dc(2),df(2,0:rr2-1)
      double precision shiftx(0:rr2-1), shifty(0:rr2-1)
      logical check_indices

      r2 = refratio*refratio
      if (r2 .ne. rr2) then
         write(6,*) 'average_corner_ghost (claw2d_utils.f) ',
     &         '  Refratio**2 is not equal to rr2'
         stop
      endif

      call build_transform(transform_ptr,a)

      m = 0
      do jj = 0,1
         do ii = 0,1
c           # Direction on coarse grid
            dc(1) = ii
            dc(2) = jj

c           # Direction on fine grid (converted using metric). Divide
c           # by 2 (refratio) to scale length to unit vector
            df(1,m) = (a(1,1)*dc(1) + a(1,2)*dc(2))/2
            df(2,m) = (a(2,1)*dc(1) + a(2,2)*dc(2))/2

c           # Map (0,1) to (-1/4,1/4) (locations of fine grid points)
            shiftx(m) = (ii-0.5d0)/2.d0
            shifty(m) = (jj-0.5d0)/2.d0
            m = m + 1
         enddo
      enddo


      mth = 5

      if (icorner_coarse .eq. 0) then
         ic = 1
         jc = 1
      elseif (icorner_coarse .eq. 1) then
         ic = mx
         jc = 1
      elseif (icorner_coarse .eq. 2) then
         ic = 1
         jc = my
      elseif (icorner_coarse .eq. 3) then
         ic = mx
         jc = my
      endif

c     # Interpolate coarse grid corners to fine grid corner ghost cells
      i1 = ic
      j1 = jc
      call fclaw2d_transform_corner_half(i1,j1,i2,j2,
     &      transform_ptr)

      do mq = 1,meqn
         qc = qcoarse(mq,ic,jc) + aux_coarse(mbathy,ic,jc)
c                 # Compute limited slopes in both x and y. Note we are not
c                 # really computing slopes, but rather just differences.
c                 # Scaling is accounted for in 'shiftx' and 'shifty', below.
         sl = (qc - qcoarse(mq,ic-1,jc) -
     &                  aux_coarse(mbathy,ic-1,jc))
         sr = (qcoarse(mq,ic+1,jc) +
     &         aux_coarse(mbathy,ic+1,jc) - qc)
         gradx = compute_slopes(sl,sr,mth)

         sl = (qc - qcoarse(mq,ic,jc-1) -
     &         aux_coarse(mbathy,ic,jc-1))
         sr = (qcoarse(mq,ic,jc+1)+
     &         aux_coarse(mbathy,ic,jc+1) - qc)
         grady = compute_slopes(sl,sr,mth)

         do m = 0,rr2-1
            iff = i2(0) + df(1,m)
            jff = j2(0) + df(2,m)
            value = qc + gradx*shiftx(m) + grady*shifty(m)
     &                       -aux_fine(mbathy,iff,jff)
            qfine(mq,iff,jff) = value
         enddo

      enddo

      end
