#ifdef RCSID
static char RcsId[] = "@(#)$Revision$";
#endif
/*
$Header$

$Log$
Revision 1.3  1992/05/28 15:12:18  chouck
Made all test files have thr string '.hdf' somewhere in their name

 * Revision 1.2  1992/04/28  18:26:38  dilg
 * Changed absolute path to relative path for include files.
 *
 * Revision 1.1  1992/04/27  17:31:08  sxu
 * Initial revision
 *
 * Revision 1.2  1992/03/24  20:42:04  sxu
 * Changed output file names
 *
 * Revision 1.1  1992/02/26  20:07:41  mfolk
 * Initial revision
 *
*/
#include "hdf.h"
#include "dfsd.h"

float64 f64[10][10], tf64[10][10];
float32 f32[10][10], tf32[10][10];
int8 i8[10][10], ti8[10][10];
uint8 ui8[10][10], tui8[10][10];
int16 i16[10][10], ti16[10][10];
uint16 ui16[10][10], tui16[10][10];
int32 i32[10][10], ti32[10][10];
uint32 ui32[10][10], tui32[10][10];

int main()
{
    int i, j, err;
    int rank;
    int dims[2];
    int number_failed = 0;

    rank = 2;
    dims[0] = 10;
    dims[1] = 10;

    printf("Creating arrays...\n");

    for (i=0; i<10; i++) {
        for (j=0; j<10; j++) {

	    f64[i][j] = (i * 10) + j;	/* range: 0 ~ 4-billion */

	    f32[i][j] = (i * 10) + j;	/* range: 0 ~ 4-billion */

	    i8[i][j] = (i * 10) + j;    /* range: 0 ~ 100 */
	    ui8[i][j] = (i * 20) + j;	/* range: 0 ~ 200 */

	    i16[i][j] = (i * 10) + j;	/* range: 0 ~ 30000 */
	    ui16[i][j] = (i * 20) + j;	/* range: 0 ~ 60000 */

	    i32[i][j] = (i * 10L) + j;	/* range: 0 ~ 2-billion */
	    ui32[i][j] = (i * 20) + j;	/* range: 0 ~ 4-billion */
	}
    }

    DFSDsetdims(rank, dims);

    /* individual files */
    printf("Testing arrays in individual files...\n");

    DFSDsetNT(DFNT_NFLOAT64);
    err = DFSDadddata("con.hdf.00", rank, dims, f64);
    printf("Write: %d     ", err);		
    err = DFSDgetdata("con.hdf.00", rank, dims, tf64);
    HEprint(stderr, 0);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++) {
	    if (f64[i][j] != tf64[i][j])
	        err = 1;
	    tf64[i][j] = 0.0;
	}
    if (err == 1) {
	printf(">>> Test failed for float64 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for float64 array.\n");	
    
    DFSDsetNT(DFNT_NFLOAT32);
    err = DFSDadddata("con.hdf.0", rank, dims, f32);
    printf("Write: %d     ", err);		
    err = DFSDgetdata("con.hdf.0", rank, dims, tf32);
    HEprint(stderr, 0);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++) {
	    if (f32[i][j] != tf32[i][j])
	        err = 1;
	    tf32[i][j] = 0.0;
	}
    if (err == 1) {
	printf(">>> Test failed for float32 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for float32 array.\n");	
    

    DFSDsetNT(DFNT_NINT8);
    err = DFSDadddata("con.hdf.1", rank, dims, i8);
    printf("Write: %d     ", err);
    err = DFSDgetdata("con.hdf.1", rank, dims, ti8);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++) {
	    if (i8[i][j] != ti8[i][j])
	        err = 1;
	    ti8[i][j] = 0;
	}
    if (err == 1) {
	printf(">>> Test failed for int8 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for int8 array.\n");

    DFSDsetNT(DFNT_NUINT8);
    err = DFSDadddata("con.hdf.2", rank, dims, ui8);
    printf("Write: %d     ", err);
    err = DFSDgetdata("con.hdf.2", rank, dims, tui8);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++) {
	  for (j=0; j<10; j++) {
	    if (ui8[i][j] != tui8[i][j])
	        err = 1;
	    tui8[i][j] = 0;
      }
	}
    if (err == 1 ) {
	printf(">>> Test failed for uint8 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for uint8 array.\n");

    DFSDsetNT(DFNT_NINT16);
    err = DFSDadddata("con.hdf.3", rank, dims, i16);
    printf("Write: %d     ", err);
    err = DFSDgetdata("con.hdf.3", rank, dims, ti16);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++) {
	    if (i16[i][j] != ti16[i][j])
	        err = 1;
	    ti16[i][j] = 0;
	}
    if (err == 1 ) {
	printf(">>> Test failed for int16 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for int16 array.\n");

    DFSDsetNT(DFNT_NUINT16);
    err = DFSDadddata("con.hdf.4", rank, dims, ui16);
    printf("Write: %d     ", err);
    err = DFSDgetdata("con.hdf.4", rank, dims, tui16);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++) {
	    if (ui16[i][j] != tui16[i][j])
	        err = 1;
	    tui16[i][j] = 0;
	}
    if (err == 1 ) {
	printf(">>> Test failed for uint16 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for uint16 array.\n");

    DFSDsetNT(DFNT_NINT32);
    err = DFSDadddata("con.hdf.5", rank, dims, i32);
    printf("Write: %d     ", err);
    err = DFSDgetdata("con.hdf.5", rank, dims, ti32);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++) {
	    if (i32[i][j] != ti32[i][j])
	        err = 1;
	    ti32[i][j] = 0;
	}
    if (err == 1 ) {
	printf(">>> Test failed for int32 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for int32 array.\n");

    DFSDsetNT(DFNT_NUINT32);
    err = DFSDadddata("con.hdf.6", rank, dims, ui32);
    printf("Write: %d     ", err);
    err = DFSDgetdata("con.hdf.6", rank, dims, tui32);
    printf("Read: %d\n", err);
    err = 0;
    for (i=0; i<10; i++) {
	  for (j=0; j<10; j++) {
	    if (ui32[i][j] != tui32[i][j])
	        err = 1;
	    tui32[i][j] = 0;
      }
	}
    if (err == 1 ) {
	printf(">>> Test failed for uint32 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for uint32 array.\n");


    printf("Writing arrays to single file... ");

    DFSDsetNT(DFNT_NFLOAT64);
    err = DFSDadddata("con.hdf", rank, dims, f64);
    printf("%d  ", err);

    DFSDsetNT(DFNT_NFLOAT32);
    err = DFSDadddata("con.hdf", rank, dims, f32);
    printf("%d  ", err);
    DFSDsetNT(DFNT_NINT8);
    err = DFSDadddata("con.hdf", rank, dims, i8);
    printf("%d  ", err);
    DFSDsetNT(DFNT_NUINT8);
    err = DFSDadddata("con.hdf", rank, dims, ui8);
    printf("%d  ", err);
    DFSDsetNT(DFNT_NINT16);
    err = DFSDadddata("con.hdf", rank, dims, i16);
    printf("%d  ", err);
    DFSDsetNT(DFNT_NUINT16);
    err = DFSDadddata("con.hdf", rank, dims, ui16);
    printf("%d  ", err);
    DFSDsetNT(DFNT_NINT32);
    err = DFSDadddata("con.hdf", rank, dims, i32);
    printf("%d  ", err);
    DFSDsetNT(DFNT_NUINT32);
    err = DFSDadddata("con.hdf", rank, dims, ui32);
    printf("%d\n\n", err);

    printf("Reading arrays from single file... ");

    err = DFSDgetdata("con.hdf", rank, dims, tf64);
    printf("%d  ", err);

    err = DFSDgetdata("con.hdf", rank, dims, tf32);
    printf("%d  ", err);
    err = DFSDgetdata("con.hdf", rank, dims, ti8);
    printf("%d  ", err);
    err = DFSDgetdata("con.hdf", rank, dims, tui8);
    printf("%d  ", err);
    err = DFSDgetdata("con.hdf", rank, dims, ti16);
    printf("%d  ", err);
    err = DFSDgetdata("con.hdf", rank, dims, tui16);
    printf("%d  ", err);
    err = DFSDgetdata("con.hdf", rank, dims, ti32);
    printf("%d  ", err);
    err = DFSDgetdata("con.hdf", rank, dims, tui32);
    printf("%d\n", err);

    printf("Checking arrays from single file...\n\n");
    
    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (f64[i][j] != tf64[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for float64 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for float64 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (f32[i][j] != tf32[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for float32 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for float32 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (i8[i][j] != ti8[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for int8 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for int8 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (ui8[i][j] != tui8[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for uint8 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for uint8 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (i16[i][j] != ti16[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for int16 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for int16 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (ui16[i][j] != tui16[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for uint16 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for uint16 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (i32[i][j] != ti32[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for int32 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for int32 array.\n");

    err = 0;
    for (i=0; i<10; i++)
	for (j=0; j<10; j++)
	   if (ui32[i][j] != tui32[i][j])
	       err = 1;
    if (err == 1 ) {
	printf(">>> Test failed for uint32 array.\n");
    number_failed++;
    }
    else
	printf("Test passed for uint32 array.\n");

    if (number_failed > 0 ) {
	printf("\n\t>>> %d TESTS FAILED <<<\n\n", number_failed);
    }
    else
	printf("\n\t>>> ALL TESTS PASSED <<<\n\n");

}
