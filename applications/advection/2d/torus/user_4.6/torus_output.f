      subroutine torus_fort_write_header(matname1,matname2,
     &      time,meqn,ngrids)
      implicit none

      integer iframe,meqn,ngrids

      character*10 matname1
      character*10 matname2
      double precision time
      integer matunit1, matunit2
      integer mfields

c     # Just to make sure this file gets replaced,
c     # and not appended.
      matunit1 = 10
      open(unit=matunit1,file=matname1,status='replace')
      close(matunit1)


      matunit2 = 10
      open(unit=matunit2,file=matname2)

c     # Write out error as an extra field (why not just do this in C?)
      mfields = meqn + 2
      write(matunit2,1000) time,mfields,ngrids
 1000 format(e30.20,'    time', /,
     &      i5,'                 mfields'/,
     &      i5,'                 ngrids')

      close(matunit2)

      end


      subroutine torus_fort_write_file(matname1,
     &      mx,my,meqn,mbc, xlower,ylower, dx,dy,
     &      q,error,t,patch_num,level,blockno,mpirank)

      implicit none

      character*10 matname1
      integer meqn,mbc,mx,my
      integer patch_num, level, blockno, mpirank
      double precision xlower, ylower,dx,dy,t
      double precision xc,yc,qc,qexact

      double precision q(1-mbc:mx+mbc,1-mbc:my+mbc,meqn)
      double precision error(1-mbc:mx+mbc,1-mbc:my+mbc,meqn)

      integer matunit1
      integer i,j,mq

      matunit1 = 10
      open(matunit1,file=matname1,position='append');


      call fc2d_clawpack46_fort_write_grid_header(matunit1,
     &      mx,my,xlower,ylower, dx,dy,patch_num,level,
     &      blockno,mpirank)

      if (meqn .gt. 5) then
         write(6,'(A,A,A)')
     &         'Warning (fclaw2d_fort_write_grid_header.f) ',
     &         ': meqn > 5; change format statement 120.'
         stop
      endif

      do j = 1,my
         do i = 1,mx
            do mq = 1,meqn
               if (abs(q(i,j,mq)) .lt. 1d-99) then
                  q(i,j,mq) = 0.d0
               endif
            enddo
            xc = xlower + (i-0.5)*dx
            yc = ylower + (j-0.5)*dy
            qc = qexact(blockno,xc, yc,t)
            if (abs(qc) .lt. 1d-99) then
               qc = 0.d0
            endif
            if (abs(error(i,j,1)) .lt. 1d-99) then
               error(i,j,1) = 0.d0
            endif
            write(matunit1,120) (q(i,j,mq),mq=1,meqn),
     &            error(i,j,1), qc
         enddo
         write(matunit1,*) ' '
      enddo
c     # This statement is checked above (meqn <= 5)
  120 format (5E26.16)

      close(matunit1)

      end
