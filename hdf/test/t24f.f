C****************************************************************************
C* NCSA HDF                                                                 *
C* Software Development Group                                               *
C* National Center for Supercomputing Applications                          *
C* University of Illinois at Urbana-Champaign                               *
C* 605 E. Springfield, Champaign IL 61820                                   *
C*                                                                          *
C* For conditions of distribution and use, see the accompanying             *
C* hdf/COPYING file.                                                        *
C*                                                                          *
C****************************************************************************
C
C $Id$
C
      program tdf24F

C
C Test Program: 
C     		Writes 24-bit raster images with specified interlace 
C		   code to a file.
C		Reads the images and their dimensions from the file.
C Input file: none
C Output file: tdf24f.hdf
C
C
      integer d2setil, d2reqil, d2pimg, d2aimg
      integer d2gdims, d2gimg, d2first

      integer d1, d2, il
      character*80 TESTFILE
      character*1 CR
      character buf(3, 2, 2), buf1(2, 3, 2), buf2(2, 2, 3)
      character in(3,2,2), in1(2, 3, 2), in2(2, 2, 3)
      integer i, j, k, ret, number_failed

      TESTFILE = 'tdf24f.hdf'
      CR = char(10)
      number_failed = 0

      do 150 i=1, 2
          do 2 j=1, 2
              buf(1, j, i) = char(i+j)
              buf(2, j, i) = char(i+j)
              buf(3, j, i) = char(i+j)
              buf1(j, 1, i) = char(i-j)
              buf1(j, 2, i) = char(i-j)
              buf1(j, 3, i) = char(i-j)
              buf2(j, i, 1) = char(2*i - j)
              buf2(j, i, 2) = char(2*i - j)
              buf2(j, i, 3) = char(2*i - j)
2       continue
150   continue
      print *, 'Setting il to 0'
      ret = d2setil(0)
      call RESULT(ret, 'd2setil',number_failed)
      print *, ' Putting buffer 1'
      ret = d2pimg(TESTFILE, buf, 2, 2)
      call RESULT(ret, 'd2pimg',number_failed)
      print *, 'Setting il to 1'
      ret = d2setil(1)
      call RESULT(ret, 'd2setil',number_failed)
      print *, 'Adding buf1'
      ret = d2aimg(TESTFILE, buf1, 2, 2)
      call RESULT(ret, 'd2aimg',number_failed)
      print *, 'Setting il to 2'
      ret = d2setil(2)
      call RESULT(ret, 'd2setil',number_failed)
      print *, 'Adding buf2'
      ret = d2aimg(TESTFILE, buf2, 2, 2)
      call RESULT(ret, 'd2aimg',number_failed)
      print *, 'Restarting file'
      ret = d2first()
      call RESULT(ret, 'd2first',number_failed)
      print *, 'Req il 0'
      ret = d2reqil(0)
      call RESULT(ret, 'd2reqil',number_failed)
      print *, 'Getting dims'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *, 'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE, in, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 200 i=1, 2
          do 180 j=1, 2
              do 160 k=1,3
                  if (in(k,j,i) .ne. buf(k,j,i)) then
                  print *, 'Error at  ', k, j, i
                  endif
160           continue
180       continue
200   continue
  
      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 250 i=1, 2
          do 220 j=1, 2
              do 210 k = 1, 3
                  if (in(k,j,i) .ne. buf1(j,k,i) ) then
                      print *, 'Error at  ', k, j, i
                  endif
210           continue
220       continue
250   continue

      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 300 i=1, 2
          do 280 j=1, 2
              do 260 k = 1, 3
                  if (in(k,j,i) .ne. buf2(j,i,k)) then
                      print *, 'Error at  ', k, j, i
                  endif
260           continue
280       continue
300   continue

      print *, 'Restarting file'
      ret = d2first()
      call RESULT(ret, 'd2first',number_failed)
      print *, 'Req il 1'
      ret = d2reqil(1)
      call RESULT(ret, 'd2reqil',number_failed)
      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in1, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 350 i=1, 2
          do 320 j=1, 2
              do 310 k=1,3
                 if (in1(j,k,i) .ne. buf(k,j,i)) then
                     print *, 'Error at  ', k, j, i
                 endif
310           continue
320       continue
350   continue

      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in1, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 400 i=1, 2
          do 380 j=1, 2
              do 360 k = 1, 3
                  if (in1(j,k,i) .ne. buf1(j,k,i)) then
                      print *, 'Error at  ', k,j,i
                  endif
360           continue
380       continue
400   continue

      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in1, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 450 i=1, 2
          do 420 j=1, 2
              do 410 k =1, 3  
                  if (in1(j,k,i) .ne. buf2(j,i,k)) then
                      print *, 'Error at  ', k, j, i
                  endif
410           continue
420       continue
450   continue

      print *, 'Restarting file'
      ret = d2first()
      call RESULT(ret, 'd2first',number_failed)
      print *, 'Req il 2'
      ret = d2reqil(2)
      call RESULT(ret, 'd2reqil',number_failed)
      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in2, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 500 i=1, 2
          do 480 j=1, 2
              do 460 k=1, 3
                  if (in2(j,i,k) .ne. buf(k,j,i)) then
                      print *, 'Error at  ', k, j, i 
                  endif
460           continue
480       continue
500   continue

      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in2, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 550 i=1, 2
          do 520 j=1, 2
              do 510 k = 1,3
                  if (in2(j,i,k) .ne. buf1(j,k,i)) then
                      print *, 'Error at  ', k, j, i
                  endif
510           continue
520       continue
550   continue

      print *, 'Getting dimensions'
      ret = d2gdims(TESTFILE, d1, d2, il)
      call RESULT(ret, 'd2gdims',number_failed)
      print *,  'd1:', d1,' d2:', d2, ' il:', il
      print *, 'Getting image'
      ret = d2gimg(TESTFILE,  in2, 2, 2)
      call RESULT(ret, 'd2gimg',number_failed)
      do 600 i=1, 2
          do 580 j=1, 2
              do 560 k = 1, 3
                  if (in2(k,j,i) .ne. buf2(k,j,i)) then
                      print *, 'Error at  ', k, j, i
                  endif
560           continue
580       continue
600   continue

      print *, CR, CR
      if (number_failed .eq. 0) then 
          print *, '****** ALL TESTS SUCCESSFUL ******'
      else
          print *, '****** ', number_failed, ' TESTS FAILES  ******'
      endif

      stop 
      end

