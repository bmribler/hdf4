/****************************************************************************
 * NCSA HDF                                                                 *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

/* $Id$ */

#ifndef __FORTEST_H
#define __FORTEST_H
#include "hdf.h"

/* Verbosity Environment Variable */
#define FOR_VERB    "HDF_FOR_VERBOSITY"

#  define ngetverb      H4_F77_FUNC(getverb, GETVERB)
#  define nhisystem     H4_F77_FUNC(hisystem, HISYSTEM)
#  define nfixnamec     H4_F77_FUNC(fixnamec, FIXNAMEC) 

/* FORTRAN support C-stubs for FORTRAN interface tests */

HDFFCLIBAPI FRETVAL(intf) ngetverb(void);
HDFFCLIBAPI FRETVAL(intf) nhisystem(_fcd cmd, intf *cmdlen);
HDFFCLIBAPI FRETVAL(intf) nfixnamec(_fcd name, intf *name_len, _fcd name_out, intf *name_len_out);

#endif /* __FORTEST_H */

