/* Copyright (c) 1994, Joseph Arceneaux.  All rights reserved.

   Copyright (c) 1992, Free Software Foundation, Inc.  All rights reserved.

   Copyright (c) 1985 Sun Microsystems, Inc. Copyright (c) 1980 The Regents
   of the University of California. Copyright (c) 1976 Board of Trustees of
   the University of Illinois. All rights reserved.

   Redistribution and use in source and binary forms are permitted
   provided that
   the above copyright notice and this paragraph are duplicated in all such
   forms and that any documentation, advertising materials, and other
   materials related to such distribution and use acknowledge that the
   software was developed by the University of California, Berkeley, the
   University of Illinois, Urbana, and Sun Microsystems, Inc.  The name of
   either University or Sun Microsystems may not be used to endorse or
   promote products derived from this software without specific prior written
   permission. THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
   OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */


/* Argument scanning and profile reading code.  Default parameters are set
   here as well. */

#include "sys.h"
#include "indent.h"
#include <ctype.h>
#include "version.h"

int else_endif_col;

extern char *in_name;

char *getenv ();
void usage ();

/* profile types */
enum profile
{
  PRO_BOOL,			/* boolean */
  PRO_INT,			/* integer */
  PRO_FONT,			/* troff font */
  PRO_IGN,			/* ignore it */
  PRO_KEY,			/* -T switch */
  PRO_SETTINGS,			/* bundled set of settings */
  PRO_PRSTRING,			/* Print string and exit */
  PRO_FUNCTION			/* Call the associated function. */
};

/* profile specials for booleans */
enum on_or_off
{
  ONOFF_NA,			/* Not applicable.  Used in table for
				   non-booleans.  */
  OFF,				/* This option turns on the boolean variable
				   in question.  */
  ON				/* it turns it off */
};

/* Explicit flags for each option.  */
static int exp_T = 0;
static int exp_bacc = 0;
static int exp_badp = 0;
static int exp_bad = 0;
static int exp_bap = 0;
static int exp_bbb = 0;
static int exp_bc = 0;
static int exp_bli = 0;
static int exp_bl = 0;
static int exp_bs = 0;
static int exp_cdb = 0;
static int exp_cd = 0;
static int exp_ce = 0;
static int exp_ci = 0;
static int exp_cli = 0;
static int exp_cp = 0;
static int exp_cs = 0;
static int exp_c = 0;
static int exp_di = 0;
static int exp_dj = 0;
static int exp_d = 0;
static int exp_eei = 0;
static int exp_ei = 0;
static int exp_fbc = 0;
static int exp_fbx = 0;
static int exp_fb = 0;
static int exp_fc1 = 0;
static int exp_fca = 0;
static int exp_fc = 0;
static int exp_fk = 0;
static int exp_fs = 0;
static int exp_gnu = 0;
static int exp_orig = 0;
static int exp_ip = 0;
static int exp_nip = 0;
static int exp_i = 0;
static int exp_lc = 0;
static int exp_lp = 0;
static int exp_l = 0;
static int exp_lps = 0;
static int exp_pcs = 0;
static int exp_psl = 0;
static int exp_pro = 0;
static int exp_ps = 0;
static int exp_kr = 0;
static int exp_sc = 0;
static int exp_sob = 0;
static int exp_ss = 0;
static int exp_st = 0;
static int exp_troff = 0;
static int exp_ts = 0;
static int exp_v = 0;
static int exp_version = 0;

/* The following variables are controlled by command line parameters and
   their meaning is explained in indent.h.  */
int leave_comma;
int decl_com_ind;
int case_indent;
int com_ind;
int decl_indent;
int ljust_decl;
int unindent_displace;
int else_if;
int indent_parameters;
int ind_size;
int tabsize;
int blanklines_after_procs;
int use_stdout;
int pointer_as_binop;
int blanklines_after_declarations;
int blanklines_around_conditional_compilation;
int swallow_optional_blanklines;
int n_real_blanklines;
int prefix_blankline_requested;
int postfix_blankline_requested;
int brace_indent;
int btype_2;

int space_sp_semicolon;
int case_ind;
int max_col;
int verbose;
int cuddle_else;
int star_comment_cont;
int comment_delimiter_on_blankline;
int troff;
int procnames_start_line;
int proc_calls_space;
int cast_space;
int format_col1_comments;
int format_comments;
int continuation_indent;
int lineup_to_parens;
int leave_preproc_space;
int blank_after_sizeof;
int blanklines_after_declarations_at_proctop;
int comment_max_col;
int extra_expression_indent;

int expect_output_file;

/* N.B.: because of the way the table here is scanned, options whose names
   are substrings of other options must occur later; that is, with -lp vs -l,
   -lp must be first.  Also, while (most) booleans occur more than once, the
   last default value is the one actually assigned. */
struct pro
{
  char *p_name;			/* name, eg -bl, -cli */
  enum profile p_type;
  int p_default;		/* the default value (if int) */

  /* If p_type == PRO_BOOL, ON or OFF to tell how this switch affects the
     variable. Not used for other p_type's.  */
  enum on_or_off p_special;

  /* if p_type == PRO_SETTINGS, a (char *) pointing to a list of the switches
     to set, separated by NULs, terminated by 2 NULs. if p_type == PRO_BOOL,
     PRO_INT, or PRO_FONT, address of the variable that gets set by the
     option. if p_type == PRO_PRSTRING, a (char *) pointing to the string.
     if p_type == PRO_FUNCTION, a pointer to a function to be called. */
  int *p_obj;

  /* Points to a nonzero value (allocated statically for all options) if the
     option has been specified explicitly.  This is necessary because for
     boolean options, the options to set and reset the variable must share
     the explicit flag.  */
  int *p_explicit;
};

#ifdef BERKELEY_DEFAULTS
/* Settings for original defaults */
struct pro pro[] =
{
  {"T", PRO_KEY, 0, ONOFF_NA, 0, &exp_T},
  {"bacc", PRO_BOOL, false, ON,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"badp", PRO_BOOL, false, ON,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"bad", PRO_BOOL, false, ON, &blanklines_after_declarations, &exp_bad},
  {"bap", PRO_BOOL, false, ON, &blanklines_after_procs, &exp_bap},
  {"bc", PRO_BOOL, true, OFF, &leave_comma, &exp_bc},
  {"bli", PRO_INT, 0, ONOFF_NA, &brace_indent, &exp_bli},
  {"bl", PRO_BOOL, true, OFF, &btype_2, &exp_bl},
  {"br", PRO_BOOL, true, ON, &btype_2, &exp_bl},
  {"bs", PRO_BOOL, false, ON, &blank_after_sizeof, &exp_bs},
  {"cdb", PRO_BOOL, true, ON, &comment_delimiter_on_blankline, &exp_cdb},
  {"cd", PRO_INT, 33, ONOFF_NA, &decl_com_ind, &exp_cd},
  {"ce", PRO_BOOL, true, ON, &cuddle_else, &exp_ce},
  {"ci", PRO_INT, 4, ONOFF_NA, &continuation_indent, &exp_ci},
  {"cli", PRO_INT, 0, ONOFF_NA, &case_indent, &exp_cli},
  {"cp", PRO_INT, 33, ONOFF_NA, &else_endif_col, &exp_cp},
  {"cs", PRO_BOOL, true, ON, &cast_space, &exp_cs},
  {"c", PRO_INT, 33, ONOFF_NA, &com_ind, &exp_c},
  {"di", PRO_INT, 16, ONOFF_NA, &decl_indent, &exp_di},
  {"dj", PRO_BOOL, false, ON, &ljust_decl, &exp_dj},
  {"d", PRO_INT, 0, ONOFF_NA, &unindent_displace, &exp_d},
  {"eei", PRO_BOOL, false, ON, &extra_expression_indent, &exp_eei},
  {"ei", PRO_BOOL, true, ON, &else_if, &exp_ei},
  {"fbc", PRO_FONT, 0, ONOFF_NA, (int *) &blkcomf, &exp_fbc},
  {"fbx", PRO_FONT, 0, ONOFF_NA, (int *) &boxcomf, &exp_fbx},
  {"fb", PRO_FONT, 0, ONOFF_NA, (int *) &bodyf, &exp_fb},
  {"fc1", PRO_BOOL, true, ON, &format_col1_comments, &exp_fc1},
  {"fca", PRO_BOOL, true, ON, &format_comments, &exp_fca},
  {"fc", PRO_FONT, 0, ONOFF_NA, (int *) &scomf, &exp_fc},
  {"fk", PRO_FONT, 0, ONOFF_NA, (int *) &keywordf, &exp_fk},
  {"fs", PRO_FONT, 0, ONOFF_NA, (int *) &stringf, &exp_fs},
  {"gnu", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-nbc\0-bl\0-ncdb\0-cs\0-nce\0-di2\0-ndj\0\
-ei\0-nfc1\0-i2\0-ip5\0-lp\0-pcs\0-nps\0-psl\0-nsc\0-nsob\0-bli2\0\
-cp1\0-nfca\0", &exp_gnu},
  {"h", PRO_FUNCTION, 0, 0, (int *) usage, &exp_version},
  {"ip", PRO_INT, 4, ON, &indent_parameters, &exp_ip},
  {"i", PRO_INT, 4, ONOFF_NA, &ind_size, &exp_i},
  {"kr", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-nbc\0-br\0-c33\0-cd33\0-ncdb\0-ce\0\
-ci4\0-cli0\0-d0\0-di1\0-nfc1\0-i4\0-ip0\0-l75\0-lp\0-npcs\0-npsl\0-cs\0\
-nsc\0-nsc\0-nsob\0-nfca\0-cp33\0-nss\0", &exp_kr},
  {"lc", PRO_INT, 0, ONOFF_NA, &comment_max_col, &exp_lc},
  {"lps", PRO_BOOL, false, ON, &leave_preproc_space, &exp_lps},
  {"lp", PRO_BOOL, true, ON, &lineup_to_parens, &exp_lp},
  {"l", PRO_INT, 78, ONOFF_NA, &max_col, &exp_l},
  {"nbacc", PRO_BOOL, false, OFF,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"nbadp", PRO_BOOL, false, OFF,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"nbad", PRO_BOOL, false, OFF, &blanklines_after_declarations, &exp_bad},
  {"nbap", PRO_BOOL, false, OFF, &blanklines_after_procs, &exp_bap},
  {"nbc", PRO_BOOL, true, ON, &leave_comma, &exp_bc},
  {"nbs", PRO_BOOL, false, OFF, &blank_after_sizeof, &exp_bs},
  {"ncdb", PRO_BOOL, true, OFF, &comment_delimiter_on_blankline, &exp_cdb},
  {"nce", PRO_BOOL, true, OFF, &cuddle_else, &exp_ce},
  {"ncs", PRO_BOOL, true, OFF, &cast_space, &exp_cs},
  {"ndj", PRO_BOOL, false, OFF, &ljust_decl, &exp_dj},
  {"neei", PRO_BOOL, false, OFF, &extra_expression_indent, &exp_eei},
  {"nei", PRO_BOOL, true, OFF, &else_if, &exp_ei},
  {"nfc1", PRO_BOOL, true, OFF, &format_col1_comments, &exp_fc1},
  {"nfca", PRO_BOOL, true, OFF, &format_comments, &exp_fca},
  {"nip", PRO_SETTINGS, 0, ONOFF_NA, (int *) "-ip0", &exp_nip},
  {"nlp", PRO_BOOL, true, OFF, &lineup_to_parens, &exp_lp},
  {"nlps", PRO_BOOL, false, OFF, &leave_preproc_space, &exp_lps},
  {"npcs", PRO_BOOL, false, OFF, &proc_calls_space, &exp_pcs},
  {"npro", PRO_IGN, 0, ONOFF_NA, 0, &exp_pro},
  {"npsl", PRO_BOOL, true, OFF, &procnames_start_line, &exp_psl},
  {"nps", PRO_BOOL, false, OFF, &pointer_as_binop, &exp_ps},
  {"nsc", PRO_BOOL, true, OFF, &star_comment_cont, &exp_sc},
  {"nsob", PRO_BOOL, false, OFF, &swallow_optional_blanklines, &exp_sob},
  {"nss", PRO_BOOL, false, OFF, &space_sp_semicolon, &exp_ss},
  {"nv", PRO_BOOL, false, OFF, &verbose, &exp_v},

  {"o", PRO_BOOL, false, ON, &expect_output_file, &expect_output_file},

  {"pcs", PRO_BOOL, false, ON, &proc_calls_space, &exp_pcs},
  {"psl", PRO_BOOL, true, ON, &procnames_start_line, &exp_psl},
  {"ps", PRO_BOOL, false, ON, &pointer_as_binop, &exp_ps},
  {"sc", PRO_BOOL, true, ON, &star_comment_cont, &exp_sc},
  {"sob", PRO_BOOL, false, ON, &swallow_optional_blanklines, &exp_sob},
  {"ss", PRO_BOOL, false, ON, &space_sp_semicolon, &exp_ss},
  {"st", PRO_BOOL, false, ON, &use_stdout, &exp_st},
  {"troff", PRO_BOOL, false, ON, &troff, &exp_troff},
  {"ts", PRO_INT, 8, ONOFF_NA, &tabsize, &exp_ts},
  {"version", PRO_PRSTRING, 0, ONOFF_NA,
   (int *) VERSION_STRING, &exp_version},
  {"v", PRO_BOOL, false, ON, &verbose, &exp_v},

/* Signify end of structure.  */
  {0, PRO_IGN, 0, ONOFF_NA, 0, 0}
};
#else /* Default to GNU style */

/* Changed to make GNU style the default. */
struct pro pro[] =
{
  {"T", PRO_KEY, 0, ONOFF_NA, 0, &exp_T},
  {"bacc", PRO_BOOL, false, ON,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"badp", PRO_BOOL, false, ON,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"bad", PRO_BOOL, false, ON, &blanklines_after_declarations, &exp_bad},
  {"bap", PRO_BOOL, true, ON, &blanklines_after_procs, &exp_bap},
  {"bc", PRO_BOOL, true, OFF, &leave_comma, &exp_bc},
  {"bli", PRO_INT, 2, ONOFF_NA, &brace_indent, &exp_bli},
  {"bl", PRO_BOOL, true, OFF, &btype_2, &exp_bl},
  {"br", PRO_BOOL, false, ON, &btype_2, &exp_bl},
  {"bs", PRO_BOOL, false, ON, &blank_after_sizeof, &exp_bs},
  {"cdb", PRO_BOOL, false, ON, &comment_delimiter_on_blankline, &exp_cdb},
  {"cd", PRO_INT, 33, ONOFF_NA, &decl_com_ind, &exp_cd},
  {"ce", PRO_BOOL, false, ON, &cuddle_else, &exp_ce},
  {"ci", PRO_INT, 0, ONOFF_NA, &continuation_indent, &exp_ci},
  {"cli", PRO_INT, 0, ONOFF_NA, &case_indent, &exp_cli},
  {"cp", PRO_INT, 1, ONOFF_NA, &else_endif_col, &exp_cp},
  {"cs", PRO_BOOL, true, ON, &cast_space, &exp_cs},
  {"c", PRO_INT, 33, ONOFF_NA, &com_ind, &exp_c},
  {"di", PRO_INT, 2, ONOFF_NA, &decl_indent, &exp_di},
  {"dj", PRO_BOOL, false, ON, &ljust_decl, &exp_dj},
  {"d", PRO_INT, 0, ONOFF_NA, &unindent_displace, &exp_d},
  {"eei", PRO_BOOL, false, ON, &extra_expression_indent, &exp_eei},
  {"ei", PRO_BOOL, true, ON, &else_if, &exp_ei},
  {"fbc", PRO_FONT, 0, ONOFF_NA, (int *) &blkcomf, &exp_fbc},
  {"fbx", PRO_FONT, 0, ONOFF_NA, (int *) &boxcomf, &exp_fbx},
  {"fb", PRO_FONT, 0, ONOFF_NA, (int *) &bodyf, &exp_fb},
  {"fc1", PRO_BOOL, false, ON, &format_col1_comments, &exp_fc1},
  {"fca", PRO_BOOL, false, ON, &format_comments, &exp_fca},
  {"fc", PRO_FONT, 0, ONOFF_NA, (int *) &scomf, &exp_fc},
  {"fk", PRO_FONT, 0, ONOFF_NA, (int *) &keywordf, &exp_fk},
  {"fs", PRO_FONT, 0, ONOFF_NA, (int *) &stringf, &exp_fs},

/* This is now the default. */
  {"gnu", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-nbc\0-bl\0-ncdb\0-cs\0-nce\0-di2\0-ndj\0\
-ei\0-nfc1\0-i2\0-ip5\0-lp\0-pcs\0-nps\0-psl\0-nsc\0-nsob\0-bli2\0\
-cp1\0-nfca\0", &exp_gnu},

  {"h", PRO_FUNCTION, 0, 0, (int *) usage, &exp_version},
  {"ip", PRO_INT, 5, ON, &indent_parameters, &exp_ip},
  {"i", PRO_INT, 2, ONOFF_NA, &ind_size, &exp_i},
  {"kr", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-nbc\0-br\0-c33\0-cd33\0-ncdb\0-ce\0\
-ci4\0-cli0\0-d0\0-di1\0-nfc1\0-i4\0-ip0\0-l75\0-lp\0-npcs\0-npsl\0-cs\0\
-nsc\0-nsc\0-nsob\0-nfca\0-cp33\0-nss\0", &exp_kr},
  {"lc", PRO_INT, 0, ONOFF_NA, &comment_max_col, &exp_lc},
  {"lps", PRO_BOOL, false, ON, &leave_preproc_space, &exp_lps},
  {"lp", PRO_BOOL, true, ON, &lineup_to_parens, &exp_lp},
  {"l", PRO_INT, 78, ONOFF_NA, &max_col, &exp_l},
  {"nbacc", PRO_BOOL, false, OFF,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"nbadp", PRO_BOOL, false, OFF,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"nbad", PRO_BOOL, false, OFF, &blanklines_after_declarations, &exp_bad},
  {"nbap", PRO_BOOL, true, OFF, &blanklines_after_procs, &exp_bap},
  {"nbc", PRO_BOOL, true, ON, &leave_comma, &exp_bc},
  {"nbs", PRO_BOOL, false, OFF, &blank_after_sizeof, &exp_bs},
  {"ncdb", PRO_BOOL, false, OFF, &comment_delimiter_on_blankline, &exp_cdb},
  {"nce", PRO_BOOL, false, OFF, &cuddle_else, &exp_ce},
  {"ncs", PRO_BOOL, true, OFF, &cast_space, &exp_cs},
  {"ndj", PRO_BOOL, false, OFF, &ljust_decl, &exp_dj},
  {"neei", PRO_BOOL, false, OFF, &extra_expression_indent, &exp_eei},
  {"nei", PRO_BOOL, true, OFF, &else_if, &exp_ei},
  {"nfc1", PRO_BOOL, false, OFF, &format_col1_comments, &exp_fc1},
  {"nfca", PRO_BOOL, false, OFF, &format_comments, &exp_fca},
  {"nip", PRO_SETTINGS, 0, ONOFF_NA, (int *) "-ip0\0", &exp_nip},
  {"nlp", PRO_BOOL, true, OFF, &lineup_to_parens, &exp_lp},
  {"nlps", PRO_BOOL, false, OFF, &leave_preproc_space, &exp_lps},
  {"npcs", PRO_BOOL, true, OFF, &proc_calls_space, &exp_pcs},
  {"npro", PRO_IGN, 0, ONOFF_NA, 0, &exp_pro},
  {"npsl", PRO_BOOL, true, OFF, &procnames_start_line, &exp_psl},
  {"nps", PRO_BOOL, false, OFF, &pointer_as_binop, &exp_ps},
  {"nsc", PRO_BOOL, false, OFF, &star_comment_cont, &exp_sc},
  {"nsob", PRO_BOOL, false, OFF, &swallow_optional_blanklines, &exp_sob},
  {"nss", PRO_BOOL, false, OFF, &space_sp_semicolon, &exp_ss},
  {"nv", PRO_BOOL, false, OFF, &verbose, &exp_v},
  {"orig", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbap\0-nbad\0-bc\0-br\0-c33\0-cd33\0-cdb\0-ce\0-ci4\0\
-cli0\0-cp33\0-di16\0-fc1\0-fca\0-i4\0-ip4\0-l75\0-lp\0\
-npcs\0-psl\0-sc\0-nsob\0-nss\0-ts8\0", &exp_orig},

  {"o", PRO_BOOL, false, ON, &expect_output_file, &expect_output_file},

  {"pcs", PRO_BOOL, true, ON, &proc_calls_space, &exp_pcs},
  {"psl", PRO_BOOL, true, ON, &procnames_start_line, &exp_psl},
  {"ps", PRO_BOOL, false, ON, &pointer_as_binop, &exp_ps},
  {"sc", PRO_BOOL, false, ON, &star_comment_cont, &exp_sc},
  {"sob", PRO_BOOL, false, ON, &swallow_optional_blanklines, &exp_sob},
  {"ss", PRO_BOOL, false, ON, &space_sp_semicolon, &exp_ss},
  {"st", PRO_BOOL, false, ON, &use_stdout, &exp_st},
  {"troff", PRO_BOOL, false, ON, &troff, &exp_troff},
  {"ts", PRO_INT, 8, ONOFF_NA, &tabsize, &exp_ts},
  {"version", PRO_PRSTRING, 0, ONOFF_NA,
   (int *) VERSION_STRING, &exp_version},
  {"v", PRO_BOOL, false, ON, &verbose, &exp_v},

/* Signify end of structure.  */
  {0, PRO_IGN, 0, ONOFF_NA, 0, 0}
};
#endif /* GNU defaults */

struct long_option_conversion
{
  char *long_name;
  char *short_name;
};

struct long_option_conversion option_conversions[] =
{
  {"blank-lines-after-ifdefs", "bacc"},
  {"blank-lines-after-procedure-declarations", "badp"},
  {"blank-lines-after-declarations", "bad"},
  {"blank-lines-after-procedures", "bap"},
  {"blank-lines-after-block-comments", "bbb"},
  {"blank-lines-after-commas", "bc"},
  {"brace-indent", "bli"},
  {"braces-after-if-line", "bl"},
  {"braces-on-if-line", "br"},
  {"Bill-Shannon", "bs"},
  {"blank-before-sizeof", "bs"},
  {"comment-delimiters-on-blank-lines", "cdb"},
  {"declaration-comment-column", "cd"},
  {"cuddle-else", "ce"},
  {"continuation-indentation", "ci"},
  {"case-indentation", "cli"},
  {"else-endif-column", "cp"},
  {"space-after-cast", "cs"},
  {"comment-indentation", "c"},
  {"declaration-indentation", "di"},
  {"left-justify-declarations", "dj"},
  {"line-comments-indentation", "d"},
  {"extra-expression-indentation", "eei"},
  {"else-if", "ei"},
  {"*", "fbc"},
  {"*", "fbx"},
  {"*", "fb"},
  {"format-first-column-comments", "fc1"},
  {"format-all-comments", "fca"},
  {"*", "fc"},
  {"*", "fk"},
  {"*", "fs"},
  {"gnu-style", "gnu"},
  {"help", "h"},
  {"usage", "h"},
  {"parameter-indentation", "ip"},
  {"indentation-level", "i"},
  {"indent-level", "i"},
  {"k-and-r-style", "kr"},
  {"kernighan-and-ritchie-style", "kr"},
  {"kernighan-and-ritchie", "kr"},
  {"comment-line-length", "lc"},
  {"continue-at-parentheses", "lp"},
  {"leave-preprocessor-space", "lps"},
  {"remove-preprocessor-space", "nlps"},
  {"line-length", "l"},
  {"no-blank-lines-after-ifdefs", "nbacc"},
  {"no-blank-lines-after-procedure-declarations", "nbadp"},
  {"no-blank-lines-after-declarations", "nbad"},
  {"no-blank-lines-after-procedures", "nbap"},
  {"no-blank-lines-after-commas", "nbc"},
  {"no-Bill-Shannon", "nbs"},
  {"no-blank-before-sizeof", "nbs"},
  {"no-comment-delimiters-on-blank-lines", "ncdb"},
  {"dont-cuddle-else", "nce"},
  {"no-space-after-casts", "ncs"},
  {"dont-left-justify-declarations", "ndj"},
  {"no-extra-expression-indentation", "neei"},
  {"no-else-if", "nei"},
  {"dont-format-first-column-comments", "nfc1"},
  {"dont-format-comments", "nfca"},
  {"no-parameter-indentation", "nip"},
  {"dont-indent-parameters", "nip"},
  {"dont-line-up-parentheses", "nlp"},
  {"no-space-after-function-call-names", "npcs"},
  {"ignore-profile", "npro"},
  {"dont-break-procedure-type", "npsl"},
  {"*", "nps"},
  {"dont-star-comments", "nsc"},
  {"leave-optional-blank-lines", "nsob"},
  {"dont-space-special-semicolon", "nss"},
  {"no-verbosity", "nv"},
  {"output", "o"},
  {"output-file", "o"},
  {"original", "orig"},
  {"original-style", "orig"},
  {"berkeley-style", "orig"},
  {"berkeley", "orig"},
  {"space-after-procedure-calls", "pcs"},
  {"procnames-start-lines", "psl"},
  {"pointer-as-binary-op", "ps"},
  {"start-left-side-of-comments", "sc"},
  {"swallow-optional-blank-lines", "sob"},
  {"space-special-semicolon", "ss"},
  {"standard-output", "st"},
  {"troff-formatting", "troff"},
  {"tab-size", "ts"},
  {"version", "version"},
  {"verbose", "v"},
  {0, 0},
};

/* S1 should be a string.  S2 should be a string, perhaps followed by an
   argument.  Compare the two, returning true if they are equal, and if they
   are equal set *START_PARAM to point to the argument in S2.  */

static int
eqin (s1, s2, start_param)
     register char *s1;
     register char *s2;
     char **start_param;
{
  while (*s1)
    {
      if (*s1++ != *s2++)
	return (false);
    }
  *start_param = s2;
  return (true);
}

/* Set the defaults. */
void
set_defaults ()
{
  register struct pro *p;

  for (p = pro; p->p_name; p++)
    if (p->p_type == PRO_BOOL || p->p_type == PRO_INT)
      *p->p_obj = p->p_default;
}

/* Stings which can prefix an option, longest first. */
static char *option_prefixes[] =
{
  "--",
  "-",
  "+",
  0
};

static int
option_prefix (arg)
     char *arg;
{
  register char **prefixes = option_prefixes;
  register char *this_prefix, *argp;

  do
    {
      this_prefix = *prefixes;
      argp = arg;
      while (*this_prefix == *argp)
	{
	  this_prefix++;
	  argp++;
	}
      if (*this_prefix == '\0')
	return this_prefix - *prefixes;
    }
  while (*++prefixes);

  return 0;
}

/* Process an option ARG (e.g. "-l60").  PARAM is a possible value
   for ARG, if PARAM is nonzero.  EXPLICT should be nonzero iff the
   argument is being explicitly specified (as opposed to being taken from a
   PRO_SETTINGS group of settings).

   Returns 1 if the option had a value, returns 0 otherwise. */

int
set_option (option, param, explicit)
     char *option, *param;
     int explicit;
{
  struct pro *p = pro;
  char *param_start;
  register int option_length, val;

  val = 0;
  option_length = option_prefix (option);
  if (option_length > 0)
    {
      if (option_length == 1 && *option == '-')
	/* Short option prefix */
	{
	  option++;
	  for (p = pro; p->p_name; p++)
	    if (*p->p_name == *option
		&& eqin (p->p_name, option, &param_start))
	      goto found;
	}
      else
	/* Long prefix */
	{
	  register struct long_option_conversion *o = option_conversions;
	  option += option_length;

	  while (o->short_name)
	    {
	      if (eqin (o->long_name, option, &param_start))
		break;
	      o++;
	    }

	  /* Searching twice means we don't have to keep the two tables in
	     sync. */
	  if (o->short_name)
	    for (p = pro; p->p_name; p++)
	      if (!strcmp (p->p_name, o->short_name))
		goto found;
	}
    }

  fprintf (stderr, "indent: unknown option \"%s\"\n", option - 1);
  exit (1);

arg_missing:
  fprintf (stderr, "indent: missing argument to parameter %s\n", option);
  exit (1);

found:
  /* If the parameter has been explicitly specified, we don't
     want a group of bundled settings to override the explicit
     setting.  */
  if (verbose)
    fprintf (stderr, "option: %s\n", p->p_name);
  if (explicit || !*(p->p_explicit))
    {
      if (explicit)
	*(p->p_explicit) = 1;

      switch (p->p_type)
	{

	case PRO_PRSTRING:
	  puts ((char *) p->p_obj);
	  exit (0);

	case PRO_FUNCTION:
	  ((void(*)()) p->p_obj) ();
	  break;

	case PRO_SETTINGS:
	  {
	    char *t;		/* current position */

	    t = (char *) p->p_obj;
	    do
	      {
		set_option (t, 0, 0);
		/* advance to character following next NUL */
		while (*t++);
	      }
	    while (*t);
	  }

	case PRO_IGN:
	  break;

	case PRO_KEY:
	  {
	    register char *str;
	    if (*param_start == 0)
	      if (!(param_start = param))
		goto arg_missing;
	      else
		val = 1;

	    str = (char *) xmalloc (strlen (param_start) + 1);
	    strcpy (str, param_start);
	    addkey (str, rw_decl);
	  }
	  break;

	case PRO_BOOL:
	  if (p->p_special == OFF)
	    *p->p_obj = false;
	  else
	    *p->p_obj = true;
	  break;

	case PRO_INT:
	  if (*param_start == 0)
	    if (!(param_start = param))
	      goto arg_missing;
	    else
	      val = 1;
	  if (!isdigit (*param_start))
	    {
	      fprintf (stderr,
		     "indent: option ``%s'' requires a numeric parameter\n",
		       option - 1);
	      exit (1);
	    }
	  *p->p_obj = atoi (param_start);
	  break;

	case PRO_FONT:
	  if (*param_start == 0)
	    if (!(param_start = param))
	      goto arg_missing;
	    else
	      val = 1;
	  parsefont ((struct fstate *) p->p_obj, param_start);
	  break;

	default:
	  fprintf (stderr, "indent: set_option: internal error: p_type %d\n",
		   (int) p->p_type);
	  exit (1);
	}
    }

  return val;
}


/* Scan the options in the file F. */

static void
scan_profile (f)
     register FILE *f;
{
  register int i;
  register char *p, *this, *next, *temp;
  char b0[BUFSIZ];
  char b1[BUFSIZ];

  next = b0;
  this = 0;
  while (1)
    {
      for (p = next; (i = getc (f)) != EOF && (*p = i) > ' '; ++p);

      if (p != next)
	{
	  *p++ = 0;
	  if (!this)
	    {
	      this = b0;
	      next = b1;
	      continue;
	    }

	  if (set_option (this, next, 1))
	    {
	      this = 0;
	      next = b0;
	      continue;
	    }

	  temp = this;
	  this = next;
	  next = temp;
	}
      else if (i == EOF)
	{
	  if (this)
	    set_option (this, 0, 1);
	  return;
	}
    }
}


/* Some operating systems don't allow more than one dot in a filename.  */
#if defined (ONE_DOT_PER_FILENAME)
#define INDENT_PROFILE "indent.pro"
#else
#define INDENT_PROFILE ".indent.pro"
#endif

#ifndef PROFILE_FORMAT
#define PROFILE_FORMAT "%s/%s"
#endif


/* set_profile looks for ./.indent.pro or $HOME/.indent.pro, in
   that order, and reads the options given in that file.  Return the
   path of the file read.

   Note that as of version 1.3, indent only reads one file. */

char *
set_profile ()
{
  register FILE *f;
  char *fname;
  static char prof[] = INDENT_PROFILE;
  char *homedir;

  if ((f = fopen (INDENT_PROFILE, "r")) != NULL)
    {
      int len = strlen (INDENT_PROFILE) + 3;

      scan_profile (f);
      (void) fclose (f);
      fname = xmalloc (len);
      fname[0] = '.';
      fname[1] = '/';
      memcpy (&fname[2], INDENT_PROFILE, len - 3);
      fname[len - 1] = '\0';
      return fname;
    }

  homedir = getenv ("HOME");
  if (homedir)
    {
      fname = xmalloc (strlen (homedir) + 10 + sizeof prof);
      sprintf (fname, PROFILE_FORMAT, homedir, prof);

      if ((f = fopen (fname, "r")) != NULL)
	{
	  scan_profile (f);
	  (void) fclose (f);
	  return fname;
	}

      free (fname);
    }

  return 0;
}
