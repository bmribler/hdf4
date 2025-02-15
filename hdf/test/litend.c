/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF.  The full HDF copyright notice, including       *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF/releases/.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "testhdf.h"

/* Internal Variables */
#define CDIM_X 7
#define CDIM_Y 9

#define FILENAME "test_files/litend.dat"
#define TMPFILE  "temp.hdf"

/* for those machines with imprecise IEEE<-> conversions, this should be */
/* close enough */
#define FLOAT64_FUDGE ((float64)0.00000001)

static int8    cdata_i8[CDIM_Y][CDIM_X];
static uint8   cdata_u8[CDIM_Y][CDIM_X];
static int16   cdata_i16[CDIM_Y][CDIM_X];
static uint16  cdata_u16[CDIM_Y][CDIM_X];
static int32   cdata_i32[CDIM_Y][CDIM_X];
static uint32  cdata_u32[CDIM_Y][CDIM_X];
static float32 cdata_f32[CDIM_Y][CDIM_X];
static float64 cdata_f64[CDIM_Y][CDIM_X];

static void init_cdata(void);
static void wrapup_cdata(void);
static void test_little_read(void);
static void test_little_write(void);

static void
init_cdata(void)
{
    int i, j;

    for (i = 0; i < CDIM_Y; i++)
        for (j = 0; j < CDIM_X; j++) {
            cdata_i8[i][j]  = (int8)(i * 10 + j);
            cdata_u8[i][j]  = (uint8)(i * 10 + j);
            cdata_i16[i][j] = (int16)(i * 10 + j);
            cdata_u16[i][j] = (uint16)(i * 10 + j);
            cdata_i32[i][j] = (int32)(i * 10 + j);
            cdata_u32[i][j] = (uint32)(i * 10 + j);
            cdata_f32[i][j] = (float32)(i * 10 + j);
            cdata_f64[i][j] = (float64)(i * 10 + j);
        } /* end for */
} /* end init_cdata() */

static void
wrapup_cdata(void)
{
} /* end wrapup_cdata() */

static void
test_little_read(void)
{
    int      rank;
    int32    dimsizes[2] = {-1, -1};
    int32    numbertype;
    int8    *data_i8;
    uint8   *data_u8;
    int16   *data_i16;
    uint16  *data_u16;
    int32   *data_i32;
    uint32  *data_u32;
    float32 *data_f32;
    float64 *data_f64;
    int      ret;

    const char *filename = get_srcdir_filename(FILENAME);

    MESSAGE(5, printf("Testing Little-Endian Read Routines\n"););

    MESSAGE(10, printf("Testing Little-Endian INT8 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);

    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for INT8 data were incorrect\n");
        num_errs++;
    }
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LINT8) {
            fprintf(stderr, "Numbertype for INT8 data were incorrect\n");
            num_errs++;
        }
        else {
            data_i8 = (int8 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(int8));
            ret     = DFSDgetdata(filename, rank, dimsizes, (void *)data_i8);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_i8, data_i8, CDIM_X * CDIM_Y * sizeof(int8))) {
                fprintf(stderr, "INT8 data was incorrect\n");
                num_errs++;
            }
            free(data_i8);
        }
    }

    MESSAGE(10, printf("Testing Little-Endian UINT8 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for UINT8 data were incorrect\n");
        num_errs++;
    }
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LUINT8) {
            fprintf(stderr, "Numbertype for UINT8 data were incorrect\n");
            num_errs++;
        }
        else {
            data_u8 = (uint8 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(uint8));
            ret     = DFSDgetdata(filename, rank, dimsizes, (void *)data_u8);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_u8, data_u8, CDIM_X * CDIM_Y * sizeof(uint8))) {
                fprintf(stderr, "UINT8 data was incorrect\n");
                num_errs++;
            }
            free(data_u8);
        }
    }

    MESSAGE(10, printf("Testing Little-Endian INT16 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for INT16 data were incorrect\n");
        num_errs++;
    }
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LINT16) {
            fprintf(stderr, "Numbertype for INT16 data were incorrect\n");
            num_errs++;
        }
        else {
            data_i16 = (int16 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(int16));
            ret      = DFSDgetdata(filename, rank, dimsizes, (void *)data_i16);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_i16, data_i16, CDIM_X * CDIM_Y * sizeof(int16))) {
                fprintf(stderr, "INT16 data was incorrect\n");
                num_errs++;
            }
            free(data_i16);
        }
    }

    MESSAGE(10, printf("Testing Little-Endian UINT16 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for UINT16 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LUINT16) {
            fprintf(stderr, "Numbertype for UINT16 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_u16 = (uint16 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(uint16));
            ret      = DFSDgetdata(filename, rank, dimsizes, (void *)data_u16);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_u16, data_u16, CDIM_X * CDIM_Y * sizeof(uint16))) {
                fprintf(stderr, "UINT16 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_u16);
        } /* end else */
    }     /* end else */

    MESSAGE(10, printf("Testing Little-Endian INT32 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for INT32 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LINT32) {
            fprintf(stderr, "Numbertype for INT32 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_i32 = (int32 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(int32));
            ret      = DFSDgetdata(filename, rank, dimsizes, (void *)data_i32);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_i32, data_i32, CDIM_X * CDIM_Y * sizeof(int32))) {
                fprintf(stderr, "INT32 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_i32);
        } /* end else */
    }     /* end else */

    MESSAGE(10, printf("Testing Little-Endian UINT32 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for UINT32 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LUINT32) {
            fprintf(stderr, "Numbertype for UINT32 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_u32 = (uint32 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(uint32));
            ret      = DFSDgetdata(filename, rank, dimsizes, (void *)data_u32);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_u32, data_u32, CDIM_X * CDIM_Y * sizeof(uint32))) {
                fprintf(stderr, "UINT32 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_u32);
        } /* end else */
    }     /* end else */

    MESSAGE(10, printf("Testing Little-Endian FLOAT32 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for FLOAT32 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LFLOAT32) {
            fprintf(stderr, "Numbertype for FLOAT32 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_f32 = (float32 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(float32));
            ret      = DFSDgetdata(filename, rank, dimsizes, (void *)data_f32);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_f32, data_f32, CDIM_X * CDIM_Y * sizeof(float32))) {
                fprintf(stderr, "FLOAT32 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_f32);
        } /* end else */
    }     /* end else */

    MESSAGE(10, printf("Testing Little-Endian FLOAT64 Reading Routines\n"););

    ret = DFSDgetdims(filename, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for FLOAT64 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LFLOAT64) {
            fprintf(stderr, "Numbertype for FLOAT64 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_f64 = (float64 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(float64));
            ret      = DFSDgetdata(filename, rank, dimsizes, (void *)data_f64);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_f64, data_f64, CDIM_X * CDIM_Y * sizeof(float64))) {
                fprintf(stderr, "FLOAT64 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_f64);
        } /* end else */
    }     /* end else */
} /* end test_little_read */

static void
test_little_write(void)
{
    int      rank;
    int32    dimsizes[2];
    int32    numbertype;
    int8    *data_i8;
    uint8   *data_u8;
    int16   *data_i16;
    uint16  *data_u16;
    int32   *data_i32;
    uint32  *data_u32;
    float32 *data_f32;
    float64 *data_f64;
    int      ret;

    MESSAGE(5, printf("Testing Little-Endian Write Routines\n"););

    rank        = 2;
    dimsizes[0] = CDIM_Y;
    dimsizes[1] = CDIM_X;

    MESSAGE(10, printf("Testing Little-Endian INT8 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LINT8);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_i8);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian UINT8 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LUINT8);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_u8);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian INT16 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LINT16);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_i16);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian UINT16 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LUINT16);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_u16);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian INT32 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LINT32);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_i32);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian UINT32 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LUINT32);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_u32);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian FLOAT32 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LFLOAT32);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_f32);
    RESULT("DFSDadddata");

    MESSAGE(10, printf("Testing Little-Endian FLOAT64 Writing Routines\n"););

    ret = DFSDsetdims(2, dimsizes);
    RESULT("DFSDsetdims");
    ret = DFSDsetNT(DFNT_LFLOAT64);
    RESULT("DFSDsetNT");
    ret = DFSDadddata(TMPFILE, rank, dimsizes, (void *)cdata_f64);
    RESULT("DFSDadddata");

    ret = DFSDrestart();
    RESULT("DFSDrestart");

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for INT8 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LINT8) {
            fprintf(stderr, "Numbertype for INT8 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_i8 = (int8 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(int8));
            ret     = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_i8);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_i8, data_i8, CDIM_X * CDIM_Y * sizeof(int8))) {
                fprintf(stderr, "INT8 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_i8);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for UINT8 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LUINT8) {
            fprintf(stderr, "Numbertype for UINT8 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_u8 = (uint8 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(uint8));
            ret     = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_u8);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_u8, data_u8, CDIM_X * CDIM_Y * sizeof(uint8))) {
                fprintf(stderr, "UINT8 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_u8);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for INT16 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LINT16) {
            fprintf(stderr, "Numbertype for INT16 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_i16 = (int16 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(int16));
            ret      = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_i16);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_i16, data_i16, CDIM_X * CDIM_Y * sizeof(int16))) {
                fprintf(stderr, "INT16 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_i16);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for UINT16 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LUINT16) {
            fprintf(stderr, "Numbertype for UINT16 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_u16 = (uint16 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(uint16));
            ret      = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_u16);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_u16, data_u16, CDIM_X * CDIM_Y * sizeof(uint16))) {
                fprintf(stderr, "UINT16 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_u16);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for INT32 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LINT32) {
            fprintf(stderr, "Numbertype for INT32 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_i32 = (int32 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(int32));
            ret      = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_i32);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_i32, data_i32, CDIM_X * CDIM_Y * sizeof(int32))) {
                fprintf(stderr, "INT32 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_i32);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for UINT32 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LUINT32) {
            fprintf(stderr, "Numbertype for UINT32 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_u32 = (uint32 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(uint32));
            ret      = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_u32);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_u32, data_u32, CDIM_X * CDIM_Y * sizeof(uint32))) {
                fprintf(stderr, "UINT32 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_u32);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for FLOAT32 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LFLOAT32) {
            fprintf(stderr, "Numbertype for FLOAT32 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_f32 = (float32 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(float32));
            ret      = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_f32);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_f32, data_f32, CDIM_X * CDIM_Y * sizeof(float32))) {
                fprintf(stderr, "FLOAT32 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_f32);
        } /* end else */
    }     /* end else */

    ret = DFSDgetdims(TMPFILE, &rank, dimsizes, 2);
    RESULT("DFSDgetdims");
    if (dimsizes[0] != CDIM_Y || dimsizes[1] != CDIM_X) {
        fprintf(stderr, "Dimensions for FLOAT64 data were incorrect\n");
        num_errs++;
    } /* end if */
    else {
        ret = DFSDgetNT(&numbertype);
        RESULT("DFSDgetNT");
        if (numbertype != DFNT_LFLOAT64) {
            fprintf(stderr, "Numbertype for FLOAT64 data were incorrect\n");
            num_errs++;
        } /* end if */
        else {
            data_f64 = (float64 *)malloc((size_t)(dimsizes[0] * dimsizes[1]) * sizeof(float64));
            ret      = DFSDgetdata(TMPFILE, rank, dimsizes, (void *)data_f64);
            RESULT("DFSDgetdata");

            if (memcmp(cdata_f64, data_f64, CDIM_X * CDIM_Y * sizeof(float64))) {
                fprintf(stderr, "FLOAT64 data was incorrect\n");
                num_errs++;
            } /* end if */
            free(data_f64);
        } /* end else */
    }     /* end else */
} /* end test_little_write */

void
test_litend(void)
{
    init_cdata();

    test_little_read();
    test_little_write();

    wrapup_cdata();
} /* end test_litend() */
