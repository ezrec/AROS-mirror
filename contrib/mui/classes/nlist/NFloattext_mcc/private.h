/*
  NFloattext.mcc (c) Copyright 1996 by Gilles Masson
  Registered MUI class, Serial Number: 1d51                            0x9d5100a1 to 0x9d5100aF
  *** use only YOUR OWN Serial Number for your public custom class ***
  NFloattext_priv_mcc.h
*/

#ifndef MUI_NFloattext_priv_MCC_H
#define MUI_NFloattext_priv_MCC_H

#ifndef MUI_MUI_H
#include "mui.h"
#endif

#include <MUI/NFloattext_mcc.h>

#include <mcc_common.h>
#include <mcc_debug.h>


#define ALIGN_MASK      (0x0700)

struct NFTData
{
  char *NFloattext_Text;
  LONG NFloattext_Align;
  LONG NFloattext_entry_len;
  char *NFloattext_entry;
  BOOL NFloattext_Justify;
  BOOL NFloattext_Copied;
};

#endif /* MUI_NFloattext_priv_MCC_H */

