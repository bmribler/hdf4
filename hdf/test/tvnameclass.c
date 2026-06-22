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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Tests for vgroup's and vdata's name and class name.
 */
#include "hdf.h"
#include "hfile_priv.h"
#include "testhdf.h"
#include "tutils.h"

#define LONGNAMES    "tlongnames.hdf"
#define NONAMECLASS  "tundefined.hdf"
#define VGROUP1      "VGROUP1"
#define VG_LONGNAME  "Vgroup with more than 64 characters in length, 74 characters to be exact!"
#define VG_LONGCLASS "Very long class name to classify all Vgroups with more than 64 characters in name"

static void test_vglongnames(void);

static void
test_vglongnames(void)
{
    int32  file_id;  /* File ID */
    int32  vg1;      /* Vdata ID */
    int32  ref;      /* Vdata ref */
    uint16 name_len; /* Length of a vgroup's name or class name */
    size_t buf_size = 0; /* Size for name or class buffer */
    char  *vgname, *vgclass;
    int    is_internal;
    int32  status; /* Status values from routines */

    /* Open the HDF file. */
    file_id = Hopen(LONGNAMES, DFACC_CREATE, 0);
    CHECK_VOID(file_id, FAIL, "Hopen");

    /* Initialize HDF for subsequent vgroup/vdata access. */
    status = Vstart(file_id);
    CHECK_VOID(status, FAIL, "Vstart");

    /* Create a new vgroup. */
    vg1 = Vattach(file_id, -1, "w");
    CHECK_VOID(vg1, FAIL, "VSattach");

    status = Vsetname(vg1, VG_LONGNAME);
    CHECK_VOID(status, FAIL, "VSsetname");

    status = Vsetclass(vg1, VG_LONGCLASS);
    CHECK_VOID(status, FAIL, "VSsetname");

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    /* Create another vgroup of the same class. */
    vg1 = Vattach(file_id, -1, "w");
    CHECK_VOID(vg1, FAIL, "VSattach");

    status = Vsetname(vg1, VGROUP1);
    CHECK_VOID(status, FAIL, "VSsetname");

    status = Vsetclass(vg1, VG_LONGCLASS);
    CHECK_VOID(status, FAIL, "VSsetname");

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    status = Vend(file_id);
    CHECK_VOID(status, FAIL, "Vend");

    status = Hclose(file_id);
    CHECK_VOID(status, FAIL, "Hclose");

    /* Re-open the HDF file. */
    file_id = Hopen(LONGNAMES, DFACC_RDWR, 0);
    CHECK_VOID(file_id, FAIL, "Hopen");

    /* Initialize HDF for subsequent vgroup/vdata access. */
    status = Vstart(file_id);
    CHECK_VOID(status, FAIL, "Vstart");

    /* Find the long name vgroup. */
    ref = Vfind(file_id, VG_LONGNAME);
    CHECK_VOID(ref, FAIL, "VSfind");

    vg1 = Vattach(file_id, ref, "r");
    CHECK_VOID(vg1, FAIL, "VSattach");

    /* Test Vgisinternal */
    is_internal = Vgisinternal(vg1);
    CHECK_VOID(is_internal, FAIL, "Vgisinternal");
    VERIFY_VOID(is_internal, FALSE, "Vgisinternal");

    /* Get the vgroup's name */
    buf_size = Vgetname(vg1, 0, NULL);
    CHECK_VOID(buf_size, FAIL, "Vgetname");
    VERIFY_VOID(buf_size, strlen(VG_LONGNAME), "Vgetname");

    vgname = (char *)malloc(sizeof(char) * (buf_size + 1));
    CHECK_ALLOC(vgname, "vgname", "test_vglongnames");

    buf_size = Vgetname(vg1, buf_size + 1, vgname);
    CHECK(buf_size, FAIL, "Vgetname:vg1");
    VERIFY_VOID(buf_size, strlen(VG_LONGNAME), "Vgetname");
    VERIFY_CHAR_VOID(vgname, VG_LONGNAME, "Vgetname");

    /* Check deprecated function */
    status = Vgetnamelen(vg1, &name_len);
    CHECK_VOID(status, FAIL, "Vgetnamelen");

    free(vgname);

    /* Get the vgroup's class */
    buf_size = Vgetclass(vg1, 0, NULL);
    CHECK_VOID(buf_size, FAIL, "Vgetclass");
    VERIFY_VOID(buf_size, strlen(VG_LONGNAME), "Vgetclass");

    vgclass = (char *)malloc(sizeof(char) * (buf_size + 1));
    CHECK_ALLOC(vgclass, "vgclass", "test_vglongnames");

    buf_size = Vgetclass(vg1, buf_size + 1, vgclass);
    CHECK(buf_size, FAIL, "Vgetclass:vg1");
    VERIFY_VOID(buf_size, strlen(VG_LONGCLASS), "Vgetclass");
    VERIFY_CHAR_VOID(vgclass, VG_LONGCLASS, "Vgetclass");

    /* Check deprecated function */
    status = Vgetclassnamelen(vg1, &name_len);
    CHECK_VOID(status, FAIL, "Vgetnamelen");

    free(vgclass);

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    /* Find the vgroup VGROUP1. */
    ref = Vfind(file_id, VGROUP1);
    CHECK_VOID(ref, FAIL, "VSfind");

    vg1 = Vattach(file_id, ref, "r");
    CHECK_VOID(vg1, FAIL, "VSattach");

    /* Get the vgroup's name */
    buf_size = Vgetname(vg1, 0, NULL);
    CHECK_VOID(buf_size, FAIL, "Vgetname");
    VERIFY_VOID(buf_size, strlen(VGROUP1), "Vgetname");

    vgname = (char *)malloc(sizeof(char) * (buf_size + 1));
    CHECK_ALLOC(vgname, "vgname", "test_vglongnames");

    buf_size = Vgetname(vg1, buf_size + 1, vgname);
    CHECK(buf_size, FAIL, "Vgetname:vg1");
    VERIFY_VOID(buf_size, strlen(VGROUP1), "Vgetname");
    VERIFY_CHAR_VOID(vgname, VGROUP1, "Vgetname");

    /* Should have the same class */
    buf_size = Vgetclass(vg1, 0, NULL);
    CHECK_VOID(buf_size, FAIL, "Vgetclass");
    VERIFY_VOID(buf_size, strlen(VG_LONGNAME), "Vgetclass");

    vgclass = (char *)malloc(sizeof(char) * (buf_size + 1));
    CHECK_ALLOC(vgclass, "vgclass", "test_vglongnames");

    buf_size = Vgetclass(vg1, buf_size + 1, vgclass);
    CHECK(buf_size, FAIL, "Vgetclass:vg1");
    VERIFY_VOID(buf_size, strlen(VG_LONGCLASS), "Vgetclass");
    VERIFY_CHAR_VOID(vgclass, VG_LONGCLASS, "Vgetclass");

    /* Check deprecated function */
    status = Vgetnamelen(vg1, &name_len);
    CHECK_VOID(status, FAIL, "Vgetnamelen");

    free(vgname);

    /* Check deprecated function */
    status = Vgetclassnamelen(vg1, &name_len);
    CHECK_VOID(status, FAIL, "Vgetnamelen");

    free(vgclass);

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    status = Vend(file_id);
    CHECK_VOID(status, FAIL, "Vend");

    status = Hclose(file_id);
    CHECK_VOID(status, FAIL, "Hclose");

} /* test_vglongnames() */

static void
test_undefined(void)
{
    int32  file_id;     /* File ID */
    int32  vg1;         /* Vdata ID */
    int32  ref;         /* Vdata ref */
    int    is_internal; /* to test Vgisinternal */
    uint16 name_len;    /* Length of a vgroup's name or class name */
    char  *vgname = NULL,
          *vgclass = NULL;
    int32  status;      /* Status values from routines */
    int    statusint = SUCCEED; /* Status values from functions returning int */

    /* Open the HDF file. */
    file_id = Hopen(NONAMECLASS, DFACC_CREATE, 0);
    CHECK_VOID(file_id, FAIL, "Hopen");

    /* Initialize HDF for subsequent vgroup/vdata access. */
    status = Vstart(file_id);
    CHECK_VOID(status, FAIL, "Vstart");

    /* Create a vgroup which will have name set but not class */
    vg1 = Vattach(file_id, -1, "w");
    CHECK_VOID(vg1, FAIL, "VSattach");

    status = Vsetname(vg1, VG_LONGNAME);
    CHECK_VOID(status, FAIL, "VSsetname");

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    /* Create another vgroup which will have class but not name */
    vg1 = Vattach(file_id, -1, "w");
    CHECK_VOID(vg1, FAIL, "VSattach");

    status = Vsetclass(vg1, VG_LONGCLASS);
    CHECK_VOID(status, FAIL, "VSsetname");

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    status = Vend(file_id);
    CHECK_VOID(status, FAIL, "Vend");

    status = Hclose(file_id);
    CHECK_VOID(status, FAIL, "Hclose");

    /* Re-open the HDF file. */
    file_id = Hopen(NONAMECLASS, DFACC_RDWR, 0);
    CHECK_VOID(file_id, FAIL, "Hopen");

    /* Initialize HDF for subsequent vgroup/vdata access. */
    status = Vstart(file_id);
    CHECK_VOID(status, FAIL, "Vstart");

    /* Find the long name vgroup. */
    ref = Vfind(file_id, VG_LONGNAME);
    CHECK_VOID(ref, FAIL, "VSfind");

    vg1 = Vattach(file_id, ref, "r");
    CHECK_VOID(vg1, FAIL, "VSattach");

    /* Test Vgisinternal */
    is_internal = Vgisinternal(vg1);
    CHECK_VOID(is_internal, FAIL, "Vgisinternal");
    VERIFY_VOID(is_internal, FALSE, "Vgisinternal");

    /* Test Vgetclass on vgroup with no class */
    statusint = Vgetclass(vg1, NULL, &name_len);
    CHECK_VOID(statusint, FAIL, "Vgetclass");
    VERIFY_VOID(name_len, 0, "VSgetclass");

    /* The length of the class name should be 0 - deprecated function */
    status = Vgetclassnamelen(vg1, &name_len);
    CHECK_VOID(status, FAIL, "Vgetclassnamelen");
    VERIFY_VOID(name_len, 0, "VSgetclassnamelen");

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    /* Find the vgroup with class VG_LONGCLASS.  This vgroup doesn't have a name */
    ref = Vfindclass(file_id, VG_LONGCLASS);
    CHECK_VOID(ref, FAIL, "VSfindclass");

    vg1 = Vattach(file_id, ref, "r");
    CHECK_VOID(vg1, FAIL, "VSattach");

    /* Test Vgetname on vgroup with no name */
    buf_size = Vgetname(vg1, 0, NULL);
    CHECK_VOID(buf_size, FAIL, "Vgetname");
    VERIFY_VOID(buf_size, 0, "Vgetname");

    /* The length of the name should be 0 - deprecated function */
    status = Vgetnamelen(vg1, &name_len);
    CHECK_VOID(status, FAIL, "Vgetnamelen");
    VERIFY_VOID(name_len, 0, "Vgetnamelen");

    status = Vdetach(vg1);
    CHECK_VOID(status, FAIL, "Vdetach");

    status = Vend(file_id);
    CHECK_VOID(status, FAIL, "Vend");

    status = Hclose(file_id);
    CHECK_VOID(status, FAIL, "Hclose");

} /* test_undefined() */

/****************************************************************************
 * test_vgisinternal - tests the API function Vgisinternal
 *   - Use an existing GR file created during the period when GR vgroup had no
 *	class name, and had name set to GR_NAME
 *   - Get each vgroup, verify that it is internal or not
 * Jan 6, 2012 -BMR
 ****************************************************************************/

#define GR_FILE "test_files/grtdfui83.hdf"
static void
test_vgisinternal()
{
    int32       fid, vgroup_id;
    int         is_internal = FALSE;
    int32       vref        = -1;
    int         ii, status;
    const char *testfile           = get_srcdir_filename(GR_FILE);
    char        internal_array2[2] = {TRUE, TRUE};

    /* Use a GR file to test Vgisinternal on internal vgroups */

    /* Open the old GR file and initialize the V interface */
    fid = Hopen(testfile, DFACC_READ, 0);
    CHECK_VOID(fid, FAIL, "Hopen: grtdfui83.hdf");
    status = Vstart(fid);
    CHECK_VOID(status, FAIL, "Vstart");

    ii = 0;
    while ((vref = Vgetid(fid, vref)) != FAIL) { /* until no more vgroups */
        vgroup_id = Vattach(fid, vref, "r");     /* attach to vgroup */

        /* Test that the current vgroup is or is not internal as specified
           in the array internal_array2 */
        is_internal = Vgisinternal(vgroup_id);
        CHECK_VOID(is_internal, FAIL, "Vgisinternal");
        VERIFY_VOID(is_internal, internal_array2[ii], "Vgisinternal");

        status = Vdetach(vgroup_id);
        CHECK_VOID(status, FAIL, "Vdetach");

        ii++; /* increment vgroup index */
    }

    /* Terminate access to the V interface and close the file */
    status = Vend(fid);
    CHECK_VOID(status, FAIL, "Vend");
    status = Hclose(fid);
    CHECK_VOID(status, FAIL, "Hclose");
} /* test_vgisinternal */

void
test_vnameclass(void)
{
    /* test Vgroups with name and class that have more than 64 characters */
    test_vglongnames();

    /* test Vgetname and Vgetclass when either name or class is not defined. */
    test_undefined();

    /* test Vgisinternal when there is no class name */
    test_vgisinternal();
} /* test_vnameclass */
