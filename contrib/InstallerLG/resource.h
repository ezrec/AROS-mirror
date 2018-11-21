//----------------------------------------------------------------------------
// resource.h:
//
// Encapsulation of resources that we might want to localize later on.
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#ifndef RESOURCE_H_
#define RESOURCE_H_

//----------------------------------------------------------------------------
// String mapping
//----------------------------------------------------------------------------
typedef enum
{
    // Out of range.
    S_NONE = 0,

    S_CFLS, /* "Copying file(s)" */
    S_ODIR, /* "Overwrite existing directory '%s'?" */
    S_ACPY, /* "Abort copy '%s' to '%s'.\n" */
    S_NCPY, /* "Could not copy '%s' to '%s'.\n" */
    S_DWRT, /* "Delete write protected file? '%s'" */
    S_DWRD, /* "Delete write protected directory? '%s'" */
    S_OWRT, /* "Overwrite write protected file '%s'?" */
    S_DNRM, /* "Delete file '%s'?" */
    S_DLTD, /* "Deleted '%s'.\n" */
    S_NSFL, /* "No such file '%s'.\n" */
    S_FRND, /* "Renamed '%s' to '%s'.\n" */
    S_CPYD, /* "Copied '%s' to '%s'.\n" */
    S_EDIR, /* "Directory '%s' exists already.\n" */
    S_CRTD, /* "Created '%s'.\n" */
    S_GMSK, /* "Get mask '%s' %d\n" */
    S_PTCT, /* "Protect '%s' %d\n" */
    S_INST, /* "Installer" */
    S_INMD, /* "Set Installation Mode" */
    S_DSTD, /* "Destination drawer" */
    S_CPYF, /* "Copying file" */
    S_F2IN, /* "Files to install" */
    S_IMAL, /* "Installation mode and logging" */
    S_PGRS, /* "Progress" */
    S_PRCD, /* "Proceed" */
    S_RTRY, /* "Retry" */
    S_ABRT, /* "Abort" */
    S_AYES, /* "Yes" */
    S_NONO, /* "No" */
    S_OKEY, /* "Ok" */
    S_NEXT, /* "Next" */
    S_SKIP, /* "Skip" */
    S_UNER, /* "Unknown error" */
    S_STFL, /* "Setup failed" */
    S_FMCC, /* "Failed creating MUI custom class" */
    S_MAPP, /* "Failed creating MUI application" */
    S_FINT, /* "Initialization failed" */
    S_ERRS, /* "Error" */
    S_SUCC, /* "Success" */
    S_LERR, /* "Line %d: %s '%s'" */
    S_ULNV, /* "Novice User - All Actions Automatic" */
    S_ULIN, /* "Intermediate User - Limited Manual Control" */
    S_ULEX, /* "Expert User - Must Confirm all actions" */
    S_INRL, /* "Install for real" */
    S_INDR, /* "Dry run only" */
    S_NOLG, /* "No logging" */
    S_SILG, /* "Log to file" */
    S_PNLG, /* "Log to printer" */
    S_RQMS, /* "installer: required argument missing\n" */
    S_CBFI, /* "%s\n\n%s can be found in %s" */
    S_ICPL, /* "Installation complete" */
    S_XCTD, /* "Executed '%s'.\n" */
    S_CNOF, /* "Error: Could not open file '%s'\n" */
    S_SYNT, /* "Line %d: %s '%s'\n" */
    S_WRKN, /* "Working on installation...\n\n" */
    S_SDIR, /* "Select directory" */
    S_SFLE, /* "Select file" */
    S_ACRT, /* "Assign '%s' to '%s'" */
    S_ADEL, /* "Remove assign '%s'" */
    S_ACRE, /* "Could not assign '%s' to '%s'" */
    S_ADLE, /* "Could not remove assign '%s'" */
    S_APND, /* "Append '%s' to '%s'\n" */
    S_INCL, /* "Include '%s' in '%s'\n" */
    S_VINS, /* "Version to install" */
    S_VCUR, /* "Version currently installed" */
    S_VUNK, /* "Unknown version" */
    S_DDRW, /* "Destination drawer" */
    S_NINS, /* "There is no currently installed version" */

    // Out of range.
    S_GONE,
} res_t;

const char *tr(res_t r);
void locale_init(void);
void locale_exit(void);

#endif
