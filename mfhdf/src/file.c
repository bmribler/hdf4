/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 1993, University Corporation for Atmospheric Research           *
 * See netcdf/COPYRIGHT file for copying and redistribution conditions.      *
 *                                                                           *
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

#include <errno.h>

#include "nc_priv.h"
#include "herr_priv.h"

#if defined H4_HAVE_WIN32_API && !defined __MINGW32__
typedef int                               pid_t;
#endif

/* obtain the maximum number of open files allowed, at the same time,
   on the current system */
#if defined H4_HAVE_WIN32_API
#define MAX_SYS_OPENFILES _getmaxstdio()
#else
#include <sys/resource.h>
struct rlimit rlim;
#define MAX_SYS_OPENFILES (getrlimit((RLIMIT_NOFILE), (&rlim)), rlim.rlim_cur)
#endif

/* Maximum number of files can be opened at one time; subtract 3 from
   the system allowed to account for stdin, stdout, and stderr */
/* On AIX 6.1 system the limit is 2GB-1; it caused our library to choke.
   For now we will use a cap H4_MAX_AVAIL_OPENFILES on the maximum number
   of files can be open at one time. This limit should probably
   be in hlimits.h file in the future. EIP 2010-02-01*/

#define H4_MAX_AVAIL_OPENFILES 20000
#define MAX_AVAIL_OPENFILES                                                                                  \
    (((MAX_SYS_OPENFILES - 3) > H4_MAX_AVAIL_OPENFILES) ? H4_MAX_AVAIL_OPENFILES : (MAX_SYS_OPENFILES - 3))

static int _curr_opened = 0; /* the number of files currently opened */
/* NOTE: _ncdf might have been the number of files currently opened, yet it
   is not decremented when ANY file is closed but only when the file that
   has the same index as _ncdf-1 is closed.  Thus, it indicates the last
   index in _cdfs instead of the number of files currently opened.  So, I
   added _curr_opened to keep track of the number of files currently opened.
   QAK suggested to use atom as in other interfaces and that would eliminate
   similar issues.  - BMR - 11/03/07 */
static int  _ncdf = 0; /*  high water mark on open cdf's */
static NC **_cdfs;

#define HNDLE(id) (((id) >= 0 && (id) < _ncdf) ? _cdfs[(id)] : NULL)
#define STASH(id) (((id) >= 0 && (id) < _ncdf) ? HNDLE(_cdfs[(id)]->redefid) : NULL)

#ifdef H4_HAVE_WIN32_API
#define SEP '\\' /* this separates path components on Windows */
#endif
#ifndef SEP
#define SEP '/' /* default, unix */
#endif

static int max_NC_open = H4_MAX_NC_OPEN; /* current netCDF default */

/*
 * Resets _cdfs
 */
static void
ncreset_cdflist(void)
{
    if (_cdfs != NULL) {
        free(_cdfs);
        _cdfs = NULL;
    }
}

/*
 *  Allocates _cdfs and returns the allocated size if succeeds;
 *  otherwise return FAIL(-1).
 */
/* req_max - requested max to allocate */
int
NC_reset_maxopenfiles(int req_max)
{
    int  sys_limit = MAX_AVAIL_OPENFILES;
    int  alloc_size;
    NC **newlist;
    int  i;
    int  ret_value = SUCCEED;

    /* Verify arguments */
    if (req_max < 0) {
        NCadvise(NC_EINVAL, "Invalid request: %d for maximum files", req_max);
        HGOTO_DONE(-1);
    }

    /* If requested max is 0, allocate _cdfs with the default,
    max_NC_open, if _cdfs is not yet allocated, otherwise, keep
    _cdfs as is and return the current max */
    if (req_max == 0) {
        if (!_cdfs) {
            _cdfs = malloc(sizeof(NC *) * (max_NC_open));

            /* If allocation fails, return 0 for no allocation */
            if (_cdfs == NULL) {
                /* NC_EINVAL is Invalid Argument, but must decide if
                we just want to return 0 without error or not */
                NCadvise(NC_EINVAL, "Unable to allocate a cdf list of %d elements", max_NC_open);
                HGOTO_DONE(-1);
            }
            else
                HGOTO_DONE(max_NC_open);
        }
        else /* return the current limit */
            HGOTO_DONE(max_NC_open);
    } /* if req_max == 0 */

    /* If the requested max is less than the current max but there are
    more than the requested max number of files opened, do not reset
    the current max, since this will cause information lost. */
    if (req_max < max_NC_open && req_max <= _ncdf)
        HGOTO_DONE(max_NC_open);

    /* If the requested max exceeds system limit, only allocate up
    to system limit */
    if (req_max > sys_limit)
        alloc_size = sys_limit;
    else
        alloc_size = req_max;

    /* Allocate a new list */
    newlist = malloc(sizeof(NC *) * alloc_size);

    /* If allocation fails, return 0 for no allocation */
    if (newlist == NULL) {
        /* NC_EINVAL is Invalid Argument, but must decide if
        we just want to return 0 without error or not */
        NCadvise(NC_EINVAL, "Unable to allocate a cdf list of %d elements", alloc_size);
        HGOTO_DONE(-1);
    }

    /* If _cdfs is already allocated, transfer pointers over to the
    new list and deallocate the old list of pointers */
    if (_cdfs != NULL) {
        for (i = 0; i < _ncdf; i++)
            newlist[i] = _cdfs[i];
        free(_cdfs);
    }

    /* Set _cdfs to the new list */
    _cdfs   = newlist;
    newlist = NULL;

    /* Reset current max files opened allowed in HDF to the new max */
    max_NC_open = alloc_size;

    HGOTO_DONE(max_NC_open);

done:
    return ret_value;
} /* NC_reset_maxopenfiles */

/*
 *  Returns the current # of open files allowed
 */
int
NC_get_maxopenfiles(void)
{
    return max_NC_open;
} /* NC_get_maxopenfiles */

/*
 *  Returns the maximum number of open files the system allows.
 */
int
NC_get_systemlimit(void)
{
    return MAX_AVAIL_OPENFILES;
} /* NC_get_systemlimit */

/*
 *  Returns the number of files currently being opened.
 */
int
NC_get_numopencdfs(void)
{
    return _curr_opened;
} /* NC_get_numopencdfs */

/*
 *  Check validity of cdf handle, return pointer to NC struct or
 * NULL on error.
 */
NC *
NC_check_id(int cdfid)
{
    NC *handle;

    handle = (cdfid >= 0 && cdfid < _ncdf) ? _cdfs[cdfid] : NULL;
    if (handle == NULL) {
        NCadvise(NC_EBADID, "%d is not a valid cdfid", cdfid);
        return NULL;
    }
    return handle;
}

/*
 *  Check to see if in define mode.
 * If 'iserr' arg is true, advise.
 */
bool_t
NC_indefine(int cdfid, bool_t iserr) /* Should be a Macro ? */
{
    bool_t ret;
    ret = (cdfid >= 0 && cdfid < _ncdf) ? (bool_t)(_cdfs[cdfid]->flags & NC_INDEF) : FALSE;
    if (!ret && iserr) {
        if (cdfid < 0 || cdfid >= _ncdf)
            NCadvise(NC_EBADID, "%d is not a valid cdfid", cdfid);
        else
            NCadvise(NC_ENOTINDEFINE, "%s Not in define mode", _cdfs[cdfid]->path);
    }
    return ret;
}

/*
 *  Common code for ncopen and nccreate.
 */
/* path - file name */
static int
NC_open(const char *path, int mode)
{
    NC *handle    = NULL;
    int cdfid     = -1;
    int cdfs_size = -1;

    /* Allocate _cdfs, if it is already allocated, nothing will be done */
    if (_cdfs == NULL) {
        if (FAIL == (cdfs_size = NC_reset_maxopenfiles(0))) {
            NCadvise(NC_ENFILE, "Could not reset max open files limit");
            return -1;
        }
    }

    /* find first available id */
    for (cdfid = 0; cdfid < _ncdf; cdfid++)
        if (_cdfs[cdfid] == NULL)
            break;

    /* if application attempts to open more files than the current max
    allows, increase the current max to the system limit, if it's
    not at the system limit yet */
    if (cdfid == _ncdf && _ncdf >= max_NC_open) {
        /* if the current max already reaches the system limit, fail */
        if (max_NC_open == MAX_AVAIL_OPENFILES) {
            NCadvise(NC_ENFILE, "maximum number of open cdfs allowed already reaches system limit %d",
                     MAX_AVAIL_OPENFILES);
            return -1;
        }
        /* otherwise, increase the current max to the system limit */
        if (FAIL == NC_reset_maxopenfiles(MAX_AVAIL_OPENFILES)) {
            NCadvise(NC_ENFILE, "Could not reset max open files limit");
            return -1;
        }
    }

    handle = NC_new_cdf(path, mode);
    if (handle == NULL) {
        /* if the failure was due to "too many open files," simply return */
        if (errno == EMFILE) {
            nc_serror("maximum number of open files allowed has been reached\"%s\"", path);
            return -1;
        }

        if ((mode & 0x0f) == NC_CLOBBER) {
            /* only attempt to remove the file if it's not currently
            in use - bugzilla #376 */
            if (!HPisfile_in_use(path))
                if (remove(path) != 0)
                    nc_serror("couldn't remove filename \"%s\"", path);
        }
        return -1;
    }

    (void)strncpy(handle->path, path, FILENAME_MAX);
    _cdfs[cdfid] = handle;
    if (cdfid == _ncdf)
        _ncdf++;
    _curr_opened++;
    return cdfid;
} /* NC_open */

int
nccreate(const char *path, int cmode)
/* path - file name */
{
    cdf_routine_name = "nccreate";

    if (cmode & NC_CREAT) {
        return NC_open(path, cmode);
    }
    NCadvise(NC_EINVAL, "Bad Flag");
    return -1;
}

int
ncopen(const char *path, int mode)
/* path - file name */
{
    cdf_routine_name = "ncopen";
    if (mode & NC_CREAT) {
        NCadvise(NC_EINVAL, "Bad Flag");
        return -1;
    }
    return NC_open(path, mode);
}

int
ncsync(int cdfid)
{
    NC *handle;

    cdf_routine_name = "ncsync";

    handle = NC_check_id(cdfid);
    if (handle == NULL)
        return -1;

    if (handle->flags & NC_INDEF) {
        NCadvise(NC_EINDEFINE, "Unfinished definition");
        return -1;
    }

    if (handle->flags & NC_RDWR) {
        handle->xdrs->x_op = XDR_ENCODE;
        if (handle->flags & NC_HDIRTY) {
            if (!xdr_cdf(handle->xdrs, &handle))
                return -1;
            handle->flags &= ~(NC_NDIRTY | NC_HDIRTY);
        }
        else if (handle->flags & NC_NDIRTY) {
            if (!xdr_numrecs(handle->xdrs, handle))
                return -1;
            if (handle->file_type != HDF_FILE)
                handle->flags &= ~(NC_NDIRTY);
        }
    }
    else /* read only */
    {
        /* assert(handle->xdrs->x_op == XDR_DECODE) ; */
        /* free the stuff in handle that xdr_cdf allocates */
        handle->xdrs->x_op = XDR_FREE;
        (void)xdr_cdf(handle->xdrs, &handle);
        handle->xdrs->x_op = XDR_DECODE;

        if (!xdr_cdf(handle->xdrs, &handle)) {
            nc_serror("xdr_cdf");
            NC_free_cdf(handle); /* ?? what should we do now? */

            return -1;
        }
        if (NC_computeshapes(handle) == -1)
            return -1;
    }

    (void)NCxdrfile_sync(handle->xdrs);

    return 0;
}

/*
 *  In data mode, same as ncclose ;
 * In define mode, restore previous definition ;
 * In create, remove the file ;
 */
int
ncabort(int cdfid)
{
    NC      *handle;
    char     path[FILENAME_MAX + 1];
    unsigned flags;
    int      file_type;

    cdf_routine_name = "ncabort";

    handle = NC_check_id(cdfid);
    if (handle == NULL)
        return -1;

    flags = handle->flags; /* need to save past free_cdf */

    /* NC_CREAT implies NC_INDEF, in both cases need to remove handle->path */
    if (flags & (NC_INDEF | NC_CREAT)) {
        (void)strncpy(path, handle->path, FILENAME_MAX); /* stash path */
        if (!(flags & NC_CREAT))                         /* redef */
        {
            NC_free_cdf(STASH(cdfid));

            _cdfs[handle->redefid] = NULL;
            if (handle->redefid == _ncdf - 1)
                _ncdf--;
            handle->redefid = -1;
            _curr_opened--; /* one less file currently opened */

            /* if the _cdf list is empty, deallocate and reset it to NULL */
            if (_ncdf == 0)
                ncreset_cdflist();
        }
    }
    else if (handle->flags & NC_RDWR) {
        handle->xdrs->x_op = XDR_ENCODE;
        if (handle->flags & NC_HDIRTY) {
            if (!xdr_cdf(handle->xdrs, &handle))
                return -1;
        }
        else if (handle->flags & NC_NDIRTY) {
            if (!xdr_numrecs(handle->xdrs, handle))
                return -1;
        }
    }

    file_type = handle->file_type;
    NC_free_cdf(handle); /* calls fclose */

    switch (file_type) {
        case netCDF_FILE:
            if (flags & (NC_INDEF | NC_CREAT)) {
                if (remove(path) != 0)
                    nc_serror("couldn't remove filename \"%s\"", path);
            }
            break;
        case HDF_FILE:
            if (flags & NC_CREAT) {
                if (remove(path) != 0)
                    nc_serror("couldn't remove filename \"%s\"", path);
            }
            break;
    }

    _cdfs[cdfid] = NULL; /* reset pointer */

    /* if current file is at the top of the list, adjust the water mark */
    if (cdfid == _ncdf - 1)
        _ncdf--;
    _curr_opened--; /* one less file currently being opened */

    /* if the _cdf list is empty, deallocate and reset it to NULL */
    if (_ncdf == 0)
        ncreset_cdflist();

    return 0;
} /* ncabort */

/*
 * Deprecated function ;
 */
int
ncnobuf(int cdfid)
{
    NC *handle;

    cdf_routine_name = "ncnobuf";

    handle = NC_check_id(cdfid);
    if (handle == NULL)
        return -1;
    /* NOOP */
    return 0;
}

/*
 * Given the path to a file "proto",
 * we replace the filename component with
 * a name like one would get from tmpnam(3S).
 * (Many implementations of tmpnam insist on giving us
 * a directory like /usr/tmp as well. Since we are making a copy which we
 * will eventually rename() back to proto, we want the return of NCtempname
 * and proto to dwell on the same filesystem.)
 */
static char *
NCtempname(const char *proto)
{
#define TN_NACCES  1
#define TN_NDIGITS 4
    unsigned int pid; /* OS/2 DOS (MicroSoft Lib) allows "negative" int pids */

    static char seed[] = {'a', 'a', 'a', '\0'};
#define TN_NSEED (sizeof(seed) - 1)
    static char tnbuf[FILENAME_MAX + 1];
    char       *begin, *cp, *sp;

    /* assert(TN_NSEED > 0) ; */
    strcpy(tnbuf, proto);

#ifdef SEP
    if ((begin = strrchr(tnbuf, SEP)) == NULL)
        begin = tnbuf;
    else
        begin++;

    if (&tnbuf[FILENAME_MAX] - begin <= TN_NSEED + TN_NACCES + TN_NDIGITS) {
        /* not big enough */
        tnbuf[0] = '\0';
        return tnbuf;
    }
#else
    begin = tnbuf;
#endif /* SEP */

    *begin = '\0';
    (void)strcat(begin, seed);

    cp  = begin + TN_NSEED + TN_NACCES + TN_NDIGITS;
    *cp = '\0';
    pid = getpid();
    while (--cp >= begin + TN_NSEED + TN_NACCES) {
        *cp = (pid % 10) + '0';
        pid /= 10;
    }

    /* update seed for next call */
    sp = seed;
    while (*sp == 'z')
        *sp++ = 'a';
    if (*sp != '\0')
        ++*sp;

    for (*cp = 'a'; access(tnbuf, 0) == 0;) {
        if (++*cp > 'z') {
            /* ran out of tries */
            tnbuf[0] = '\0';
            return tnbuf;
        }
    }

    return tnbuf;
}

int
ncredef(int cdfid)
{
    NC *handle;
    NC *new;
    int   id;
    char *scratchfile;

    cdf_routine_name = "ncredef";

    handle = NC_check_id(cdfid);
    if (handle == NULL)
        return -1;
    if (handle->flags & NC_INDEF) /* in define mode already */
    {
        NC *stash = STASH(cdfid);
        if (stash)
            NCadvise(NC_EINDEFINE, "%s: in define mode already", stash->path);
        return -1;
    }
    if (!(handle->flags & NC_RDWR)) {
        NCadvise(NC_EPERM, "%s: NC_NOWRITE", handle->path);
        return -1;
    }

    if (handle->file_type == HDF_FILE) {
        handle->flags |= NC_INDEF;
        handle->redefid = TRUE;
        return 0;
    }

    /* find first available id */
    for (id = 0; id < _ncdf; id++)
        if (_cdfs[id] == NULL)
            break;

    if (id == _ncdf && _ncdf >= max_NC_open) /* will need a new one */
    {
        NCadvise(NC_ENFILE, "maximum number of open cdfs %d exceeded", _ncdf);
        return -1;
    }

    if (ncopts & NC_NOFILL) {
        /* fill last record */
        handle->xdrs->x_op = XDR_ENCODE;
        if (handle->flags & NC_NDIRTY) {
            if (!xdr_numrecs(handle->xdrs, handle))
                return -1;
            handle->flags &= ~(NC_NDIRTY);
        }
    }

    scratchfile = NCtempname(handle->path);

    new = NC_dup_cdf(scratchfile, NC_NOCLOBBER, handle);
    if (new == NULL) {
        return -1;
    }

    handle->flags |= NC_INDEF;
    (void)strncpy(new->path, scratchfile, FILENAME_MAX);

    /* put the old handle in the new id */
    _cdfs[id] = handle;
    if (id == _ncdf)
        _ncdf++;
    _curr_opened++;

    /* put the new handle in old id */
    _cdfs[cdfid] = new;

    new->redefid = id;

    return 0;
}

/*
 * Compute offsets and put into the header
 */
static void
NC_begins(NC *handle)
{
    unsigned long index = 0;
    NC_var      **vpp;
    NC_var       *last = NULL;

    if (handle->vars == NULL)
        return;

    index = NC_xlen_cdf(handle);

    /* loop thru vars, first pass is for the 'non-record' vars */
    vpp = (NC_var **)handle->vars->values;
    for (unsigned ii = 0; ii < handle->vars->count; ii++, vpp++) {
        if (IS_RECVAR(*vpp)) {
            continue; /* skip record variables on this pass */
        }

        (*vpp)->begin = index;
        index += (*vpp)->len;
    }

    handle->begin_rec = index;
    handle->recsize   = 0;

    /* loop thru vars, second pass is for the 'non-record' vars */
    vpp = (NC_var **)handle->vars->values;
    for (unsigned ii = 0; ii < handle->vars->count; ii++, vpp++) {
        if (!IS_RECVAR(*vpp)) {
            continue; /* skip non-record variables on this pass */
        }

        (*vpp)->begin = index;
        index += (*vpp)->len;
        handle->recsize += (*vpp)->len;
        last = (*vpp);
    }
    /*
     * for special case of exactly one record variable, pack values
     */
    if (last != NULL && handle->recsize == last->len)
        handle->recsize = *last->dsizes;
    handle->numrecs = 0;
}

/*
 * Copy nbytes bytes from source to target.
 * Streams target and source should be positioned before the call.
 * opaque I/O, no XDR conversion performed (or needed).
 */
bool_t
NC_dcpy(XDR *target, XDR *source, long nbytes)
{
/* you may wish to tune this: big on a cray, small on a PC? */
#define NC_DCP_BUFSIZE 8192
    char buf[NC_DCP_BUFSIZE];

    while (nbytes > sizeof(buf)) {
        if (!h4_xdr_getbytes(source, buf, sizeof(buf)))
            goto err;
        if (!h4_xdr_putbytes(target, buf, sizeof(buf)))
            goto err;
        nbytes -= sizeof(buf);
    }
    /* we know nbytes <= sizeof(buf) at this point */
    if (!h4_xdr_getbytes(source, buf, nbytes))
        goto err;
    if (!h4_xdr_putbytes(target, buf, nbytes))
        goto err;
    return TRUE;
err:
    NCadvise(NC_EXDR, "NC_dcpy");
    return FALSE;
}

/*
 * XDR the data of varid in old, target is the new xdr strm
 */
static bool_t
NC_vcpy(XDR *target, NC *old, int varid)
{
    NC_var **vpp;
    vpp = (NC_var **)old->vars->values;
    vpp += varid;

    if (!h4_xdr_setpos(old->xdrs, (*vpp)->begin)) {
        NCadvise(NC_EXDR, "NC_vcpy: h4_xdr_setpos");
        return FALSE;
    }

    return NC_dcpy(target, old->xdrs, (*vpp)->len);
}

/*
 * XDR the data of (varid, recnum) in old, target is the new xdr strm
 */
static bool_t
NC_reccpy(XDR *target, NC *old, int varid, int recnum)
{
    NC_var **vpp;
    vpp = (NC_var **)old->vars->values;
    vpp += varid;

    if (!h4_xdr_setpos(old->xdrs, (*vpp)->begin + old->recsize * recnum)) {
        NCadvise(NC_EXDR, "NC_reccpy: h4_xdr_setpos");
        return FALSE;
    }

    return NC_dcpy(target, old->xdrs, (*vpp)->len);
}

/*
 *  Common code for ncendef, ncclose(endef)
 */
static int
NC_endef(int cdfid, NC *handle)
{
    XDR     *xdrs;
    unsigned ii;
    unsigned jj = 0;
    NC_var **vpp;
    NC      *stash = STASH(cdfid); /* faster rvalue */

    if (handle->file_type != HDF_FILE)
        NC_begins(handle);

    xdrs       = handle->xdrs;
    xdrs->x_op = XDR_ENCODE;

    if (!xdr_cdf(xdrs, &handle)) {
        nc_serror("xdr_cdf");
        return -1;
    }

    /* Get rid of the temporary buffer allocated for I/O */
    SDPfreebuf();

    if (handle->file_type == HDF_FILE) {
        handle->flags &= ~(NC_CREAT | NC_INDEF | NC_NDIRTY | NC_HDIRTY);
        return 0;
    }

    if (handle->vars == NULL)
        goto done;

    /* loop thru vars, first pass is for the 'non-record' vars */
    vpp = (NC_var **)handle->vars->values;
    for (ii = 0; ii < handle->vars->count; ii++, vpp++) {
        if (IS_RECVAR(*vpp)) {
            continue; /* skip record variables on this pass */
        }

        if (!(handle->flags & NC_CREAT) && stash->vars != NULL && ii < stash->vars->count) {
            /* copy data */
            if (!NC_vcpy(xdrs, stash, ii))
                return -1;
            continue;
        } /* else */

        if (!(handle->flags & NC_NOFILL))
            if (!xdr_NC_fill(xdrs, *vpp))
                return -1;
    }

    if (!(handle->flags & NC_CREAT)) /* after redefinition */
    {
        for (jj = 0; jj < stash->numrecs; jj++) {
            vpp = (NC_var **)handle->vars->values;
            for (ii = 0; ii < handle->vars->count; ii++, vpp++) {
                if (!IS_RECVAR(*vpp)) {
                    continue; /* skip non-record variables on this pass */
                }
                if (stash->vars != NULL && ii < stash->vars->count) {
                    /* copy data */
                    if (!NC_reccpy(xdrs, stash, ii, jj))
                        return -1;
                    continue;
                } /* else */
                if (!(handle->flags & NC_NOFILL))
                    if (!xdr_NC_fill(xdrs, *vpp))
                        return -1;
            }
        }
        handle->numrecs = stash->numrecs;
        if (!xdr_numrecs(handle->xdrs, handle))
            return -1;
    }

    if (!(handle->flags & NC_CREAT)) /* redefine */
    {
        char realpath[FILENAME_MAX + 1];
        strcpy(realpath, stash->path);

        /* close stash */
/*                NC_free_cdf(stash) ; */
#ifdef H4_HAVE_WIN32_API
        h4_xdr_destroy(handle->xdrs); /* close handle */
        if (remove(realpath) != 0)
            nc_serror("couldn't remove filename \"%s\"", realpath);
#endif
        if (rename(handle->path, realpath) != 0) {
            nc_serror("rename %s -> %s failed", handle->path, realpath);
            /* try to restore state prior to redef */
            _cdfs[cdfid]           = stash;
            _cdfs[handle->redefid] = NULL;
            if (handle->redefid == _ncdf - 1)
                _ncdf--;
            _curr_opened--; /* one less file currently opened */
            NC_free_cdf(handle);

            /* if the _cdf list is empty, deallocate and reset it to NULL */
            if (_ncdf == 0)
                ncreset_cdflist();

            return -1;
        }
        (void)strncpy(handle->path, realpath, FILENAME_MAX);
#ifdef H4_HAVE_WIN32_API
        if (NCxdrfile_create(handle->xdrs, handle->path, NC_WRITE) < 0)
            return -1;
#endif
        NC_free_cdf(stash);
        _cdfs[handle->redefid] = NULL;
        if (handle->redefid == _ncdf - 1)
            _ncdf--;
        _curr_opened--; /* one less file currently opened */
        handle->redefid = -1;

        /* if the _cdf list is empty, deallocate and reset it to NULL */
        if (_ncdf == 0)
            ncreset_cdflist();
    }

done:
    handle->flags &= ~(NC_CREAT | NC_INDEF | NC_NDIRTY | NC_HDIRTY);
    return 0;
}

int
ncendef(int cdfid)
{
    NC *handle;

    cdf_routine_name = "ncendef";

    handle = NC_check_id(cdfid);
    if (handle == NULL)
        return -1;
    if (!NC_indefine(cdfid, TRUE))
        return -1;
    return NC_endef(cdfid, handle);
}

/*
 * This routine is called by SDend()? -GV
 */
int
ncclose(int cdfid)
{
    NC *handle;

    cdf_routine_name = "ncclose";

    handle = NC_check_id(cdfid);
    if (handle == NULL)
        return -1;

    if (handle->flags & NC_INDEF) {
        if (NC_endef(cdfid, handle) == -1) {
            return ncabort(cdfid);
        }
    }
    else if (handle->flags & NC_RDWR) {
        handle->xdrs->x_op = XDR_ENCODE;
        if (handle->flags & NC_HDIRTY) {
            if (!xdr_cdf(handle->xdrs, &handle))
                return -1;
        }
        else if (handle->flags & NC_NDIRTY) {
            if (!xdr_numrecs(handle->xdrs, handle))
                return -1;
        }
    }

    if (handle->file_type == HDF_FILE)
        hdf_close(handle);

    NC_free_cdf(handle); /* calls fclose */

    _cdfs[cdfid] = NULL; /* reset pointer */

    if (cdfid == _ncdf - 1)
        _ncdf--;
    _curr_opened--; /* one less file currently opened */

    /* if the _cdf list is empty, deallocate and reset it to NULL */
    if (_ncdf == 0)
        ncreset_cdflist();
    return 0;
}

int
ncsetfill(int id, int fillmode)
{
    NC *handle;
    int ret = 0;

    cdf_routine_name = "ncsetfill";

    handle = NC_check_id(id);
    if (handle == NULL)
        return -1;

    if (!(handle->flags & NC_RDWR)) {
        /* file isn't writable */
        NCadvise(NC_EPERM, "%s is not writable", handle->path);
        return -1;
    }

    ret = (handle->flags & NC_NOFILL) ? NC_NOFILL : NC_FILL;

    if (fillmode == NC_NOFILL)
        handle->flags |= NC_NOFILL;
    else if (fillmode == NC_FILL) {
        if (handle->flags & NC_NOFILL) {
            /*
             * We are changing back to fill mode
             * so do a sync
             */
            /* save the original x_op  */
            enum xdr_op xdr_op = handle->xdrs->x_op;

            if (handle->flags & NC_RDWR)         /* make sure we can write */
                handle->xdrs->x_op = XDR_ENCODE; /*  to the file */
            if (handle->flags & NC_HDIRTY) {
                if (!xdr_cdf(handle->xdrs, &handle))
                    return -1;
                handle->flags &= ~(NC_NDIRTY | NC_HDIRTY);
            }
            else if (handle->flags & NC_NDIRTY) {
                if (!xdr_numrecs(handle->xdrs, handle))
                    return -1;
                if (handle->file_type != HDF_FILE)
                    handle->flags &= ~(NC_NDIRTY);
            }
            handle->flags &= ~NC_NOFILL;
            /* re-store the x_op  */
            handle->xdrs->x_op = xdr_op;
        }
    }
    else {
        NCadvise(NC_EINVAL, "Bad fillmode");
        return -1;
    }

    return ret;
}

int
NCxdrfile_sync(XDR *xdrs)
{
    return h4_xdr_sync(xdrs);
}

int
NCxdrfile_create(XDR *xdrs, const char *path, int ncmode)
{
    int         fmode;
    int         fd;
    enum xdr_op op;

    switch (ncmode & 0x0f) {
        case NC_NOCLOBBER:
            fmode = O_RDWR | O_CREAT | O_EXCL;
            break;
        case NC_CLOBBER:
            fmode = O_RDWR | O_CREAT | O_TRUNC;
            break;
        case NC_WRITE:
            fmode = O_RDWR;
            break;
        case NC_NOWRITE:
            fmode = O_RDONLY;
            break;
        default:
            NCadvise(NC_EINVAL, "Bad flag %0x", ncmode & 0x0f);
            return -1;
    }

#ifdef H4_HAVE_WIN32_API
    /* Set default mode to binary to suppress the expansion of 0x0f into CRLF */
    _fmode |= O_BINARY;
#endif

    fd = open(path, fmode, 0666);
    if (fd == -1) {
        nc_serror("filename \"%s\"", path);
        return -1;
    }

    if (ncmode & NC_CREAT) {
        op = XDR_ENCODE;
    }
    else {
        op = XDR_DECODE;
    }

    if (h4_xdr_create(xdrs, fd, fmode, op) < 0)
        return -1;
    else
        return fd;
}
