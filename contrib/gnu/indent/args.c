/* Copyright (c) 1999, 2000 Carlo Wood.  All rights reserved.
   Copyright (c) 1994 Joseph Arceneaux.  All rights reserved.
   Copyright (c) 1992 Free Software Foundation, Inc.  All rights reserved.

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
#include "args.h"
#include "globs.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

RCSTAG_CC ("$Id$");

int else_endif_col;

/* profile types */
enum profile
{
  PRO_BOOL,			/* boolean */
  PRO_INT,			/* integer */
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
static int exp_T;
static int exp_bacc;
static int exp_badp;
static int exp_bad;
static int exp_bap;
static int exp_bbb;
static int exp_bbo;
static int exp_bc;
static int exp_bl;
static int exp_bli;
static int exp_bls;
static int exp_bs;
static int exp_c;
static int exp_cbi;
static int exp_cdb;
static int exp_cd;
static int exp_cdw;
static int exp_ce;
static int exp_ci;
static int exp_cli;
static int exp_cp;
static int exp_cpp;
static int exp_cs;
static int exp_d;
static int exp_bfda;
static int exp_di;
static int exp_dj;
static int exp_eei;
static int exp_fc1;
static int exp_fca;
static int exp_gnu;
static int exp_hnl;
static int exp_i;
static int exp_ip;
static int exp_kr;
static int exp_l;
static int exp_lc;
static int exp_lp;
static int exp_lps;
static int exp_nip;
static int exp_orig;
static int exp_pcs;
static int exp_pi;
static int exp_pmt;
static int exp_pro;
static int exp_prs;
static int exp_psl;
static int exp_sai;
static int exp_saf;
static int exp_saw;
static int exp_sbi;
static int exp_sc;
static int exp_sob;
static int exp_ss;
static int exp_st;
static int exp_ts;
static int exp_ut;
static int exp_v;
static int exp_version;

/* The following variables are controlled by command line parameters and
   their meaning is explained in indent.h.  */
int leave_comma;
int decl_com_ind;
int case_indent;
int case_brace_indent;
int struct_brace_indent;
int com_ind;
int decl_indent;
int ljust_decl;
int unindent_displace;
int indent_parameters;
int ind_size;
int tabsize;
int blanklines_after_procs;
int use_stdout;
int blanklines_after_declarations;
int blanklines_before_blockcomments;
int blanklines_around_conditional_compilation;
int swallow_optional_blanklines;
int n_real_blanklines;
int prefix_blankline_requested;
enum codes prefix_blankline_requested_code;
int postfix_blankline_requested;
enum codes postfix_blankline_requested_code;
int brace_indent;
int btype_2;
int braces_on_struct_decl_line;
int break_before_boolean_operator;
int honour_newlines;
int preserve_mtime;

int space_sp_semicolon;
int max_col;
int verbose;
int cuddle_else;
int cuddle_do_while;
int star_comment_cont;
int comment_delimiter_on_blankline;
int procnames_start_line;
int proc_calls_space;
int parentheses_space;
int cast_space;
int format_col1_comments;
int format_comments;
int continuation_indent;
int paren_indent;
int lineup_to_parens;
int leave_preproc_space;
int blank_after_sizeof;
int blanklines_after_declarations_at_proctop;
int comment_max_col;
int extra_expression_indent;
int space_after_pointer_type;
int space_after_if;
int space_after_for;
int space_after_while;
int break_function_decl_args;
int expect_output_file;
int c_plus_plus;
int use_tabs;

/* N.B.: because of the way the table here is scanned, options whose names
   are substrings of other options must occur later; that is, with -lp vs -l,
   -lp must be first.  Also, while (most) booleans occur more than once, the
   last default value is the one actually assigned. */
struct pro
{
  char *p_name;			/* option name, e.g. "bl", "cli" */
  enum profile p_type;		/* profile type */
  int p_default;		/* the default value (if PRO_BOOL or PRO_INT) */

  /* If p_type == PRO_BOOL, ON or OFF to tell how this switch affects the
     variable. Not used for other p_type's.  */
  enum on_or_off p_special;

  /* if p_type == PRO_SETTINGS, a (char *) pointing to a list of the switches
     to set, separated by NULs, terminated by 2 NULs. if p_type == PRO_BOOL,
     PRO_INT, address of the variable that gets set by the option.
     if p_type == PRO_PRSTRING, a (char *) pointing to the string.
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
struct pro pro[] = {
  {"version", PRO_PRSTRING, 0, ONOFF_NA, (int *) VERSION, &exp_version},
  {"v", PRO_BOOL, false, ON, &verbose, &exp_v},
  {"ut", PRO_BOOL, true, ON, &use_tabs, &exp_ut},
  {"ts", PRO_INT, 8, ONOFF_NA, &tabsize, &exp_ts},
  {"st", PRO_BOOL, false, ON, &use_stdout, &exp_st},
  {"ss", PRO_BOOL, false, ON, &space_sp_semicolon, &exp_ss},
  {"sob", PRO_BOOL, false, ON, &swallow_optional_blanklines, &exp_sob},
  {"sc", PRO_BOOL, true, ON, &star_comment_cont, &exp_sc},
  {"sbi", PRO_INT, 0, ONOFF_NA, &struct_brace_indent, &exp_sbi},
  {"saw", PRO_BOOL, true, ON, &space_after_while, &exp_saw},
  {"sai", PRO_BOOL, true, ON, &space_after_if, &exp_sai},
  {"saf", PRO_BOOL, true, ON, &space_after_for, &exp_saf},
  {"psl", PRO_BOOL, true, ON, &procnames_start_line, &exp_psl},
  {"prs", PRO_BOOL, false, ON, &parentheses_space, &exp_prs},
#ifdef PRESERVE_MTIME
  {"pmt", PRO_BOOL, false, ON, &preserve_mtime, &exp_pmt},
#endif
  {"pi", PRO_INT, -1, ONOFF_NA, &paren_indent, &exp_pi},
  {"pcs", PRO_BOOL, false, ON, &proc_calls_space, &exp_pcs},
  {"o", PRO_BOOL, false, ON, &expect_output_file, &expect_output_file},
  {"nv", PRO_BOOL, false, OFF, &verbose, &exp_v},
  {"nut", PRO_BOOL, true, OFF, &use_tabs, &exp_ut},
  {"nss", PRO_BOOL, false, OFF, &space_sp_semicolon, &exp_ss},
  {"nsob", PRO_BOOL, false, OFF, &swallow_optional_blanklines, &exp_sob},
  {"nsc", PRO_BOOL, true, OFF, &star_comment_cont, &exp_sc},
  {"nsaw", PRO_BOOL, true, OFF, &space_after_while, &exp_saw},
  {"nsai", PRO_BOOL, true, OFF, &space_after_if, &exp_sai},
  {"nsaf", PRO_BOOL, true, OFF, &space_after_for, &exp_saf},
  {"npsl", PRO_BOOL, true, OFF, &procnames_start_line, &exp_psl},
  {"nprs", PRO_BOOL, false, OFF, &parentheses_space, &exp_prs},
  {"npro", PRO_IGN, 0, ONOFF_NA, 0, &exp_pro},
#ifdef PRESERVE_MTIME
  {"npmt", PRO_BOOL, false, OFF, &preserve_mtime, &exp_pmt},
#endif
  {"npcs", PRO_BOOL, false, OFF, &proc_calls_space, &exp_pcs},
  {"nlps", PRO_BOOL, false, OFF, &leave_preproc_space, &exp_lps},
  {"nlp", PRO_BOOL, true, OFF, &lineup_to_parens, &exp_lp},
  {"nip", PRO_SETTINGS, 0, ONOFF_NA, (int *) "-ip0", &exp_nip},
  {"nhnl", PRO_BOOL, true, OFF, &honour_newlines, &exp_hnl},
  {"nfca", PRO_BOOL, true, OFF, &format_comments, &exp_fca},
  {"nfc1", PRO_BOOL, true, OFF, &format_col1_comments, &exp_fc1},
  {"neei", PRO_BOOL, false, OFF, &extra_expression_indent, &exp_eei},
  {"ndj", PRO_BOOL, false, OFF, &ljust_decl, &exp_dj},
  {"ncs", PRO_BOOL, true, OFF, &cast_space, &exp_cs},
  {"nce", PRO_BOOL, true, OFF, &cuddle_else, &exp_ce},
  {"ncdw", PRO_BOOL, false, OFF, &cuddle_do_while, &exp_cdw},
  {"ncdb", PRO_BOOL, true, OFF, &comment_delimiter_on_blankline, &exp_cdb},
  {"nbs", PRO_BOOL, false, OFF, &blank_after_sizeof, &exp_bs},
  {"nbfda", PRO_BOOL, false, OFF, &break_function_decl_args, &exp_bfda},
  {"nbc", PRO_BOOL, true, ON, &leave_comma, &exp_bc},
  {"nbbo", PRO_BOOL, true, OFF, &break_before_boolean_operator, &exp_bbo},
  {"nbbb", PRO_BOOL, false, OFF, &blanklines_before_blockcomments, &exp_bbb},
  {"nbap", PRO_BOOL, false, OFF, &blanklines_after_procs, &exp_bap},
  {"nbadp", PRO_BOOL, false, OFF,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"nbad", PRO_BOOL, false, OFF, &blanklines_after_declarations, &exp_bad},
  {"nbacc", PRO_BOOL, false, OFF,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"lps", PRO_BOOL, false, ON, &leave_preproc_space, &exp_lps},
  {"lp", PRO_BOOL, true, ON, &lineup_to_parens, &exp_lp},
  {"lc", PRO_INT, DEFAULT_RIGHT_COMMENT_MARGIN,
   ONOFF_NA, &comment_max_col, &exp_lc},
  {"l", PRO_INT, DEFAULT_RIGHT_MARGIN, ONOFF_NA, &max_col, &exp_l},
  {"kr", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-nbc\0-bbo\0-hnl\0-br\0-brs\0-c33\0-cd33\0-ncdb\0\
-ce\0-ci4\0-cli0\0-d0\0-di1\0-nfc1\0-i4\0-ip0\0-l75\0-lp\0-npcs\0-nprs\0-npsl\0\
-cs\0-sai\0-saw\0-saf\0-nsc\0-nsc\0-nsob\0-nfca\0-cp33\0-nss\0", &exp_kr},
  {"ip", PRO_INT, 4, ONOFF_NA, &indent_parameters, &exp_ip},
  {"i", PRO_INT, 4, ONOFF_NA, &ind_size, &exp_i},
  {"hnl", PRO_BOOL, true, ON, &honour_newlines, &exp_hnl},
  {"h", PRO_FUNCTION, 0, ONOFF_NA, (int *) usage, &exp_version},
  {"gnu", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-bbo\0-hnl\0-nbc\0-bl\0-bls\0-ncdb\0-cs\0-nce\0\
-di2\0-ndj\0-nfc1\0-i2\0-ip5\0-lp\0-pcs\0-nprs\0-psl\0-nsc\0-sai\0-saf\0-saw\0-nsob\0\
-bli2\0-cp1\0-nfca\0", &exp_gnu},
  {"fca", PRO_BOOL, true, ON, &format_comments, &exp_fca},
  {"fc1", PRO_BOOL, true, ON, &format_col1_comments, &exp_fc1},
  {"eei", PRO_BOOL, false, ON, &extra_expression_indent, &exp_eei},
  {"dj", PRO_BOOL, false, ON, &ljust_decl, &exp_dj},
  {"di", PRO_INT, 16, ONOFF_NA, &decl_indent, &exp_di},
  {"d", PRO_INT, 0, ONOFF_NA, &unindent_displace, &exp_d},
  {"cs", PRO_BOOL, true, ON, &cast_space, &exp_cs},
  {"cp", PRO_INT, 33, ONOFF_NA, &else_endif_col, &exp_cp},
  {"cli", PRO_INT, 0, ONOFF_NA, &case_indent, &exp_cli},
  {"ci", PRO_INT, 4, ONOFF_NA, &continuation_indent, &exp_ci},
  {"ce", PRO_BOOL, true, ON, &cuddle_else, &exp_ce},
  {"cdw", PRO_BOOL, false, ON, &cuddle_do_while, &exp_cdw},
  {"cdb", PRO_BOOL, true, ON, &comment_delimiter_on_blankline, &exp_cdb},
  {"cd", PRO_INT, 33, ONOFF_NA, &decl_com_ind, &exp_cd},
  {"cbi", PRO_INT, -1, ONOFF_NA, &case_brace_indent, &exp_cbi},
  {"c++", PRO_BOOL, false, ON, &c_plus_plus, &exp_cpp},
  {"c", PRO_INT, 33, ONOFF_NA, &com_ind, &exp_c},
  {"bbo", PRO_BOOL, true, ON, &break_before_boolean_operator, &exp_bbo},
  {"bs", PRO_BOOL, false, ON, &blank_after_sizeof, &exp_bs},
  {"brs", PRO_BOOL, true, ON, &braces_on_struct_decl_line, &exp_bls},
  {"br", PRO_BOOL, true, ON, &btype_2, &exp_bl},
  {"bls", PRO_BOOL, true, OFF, &braces_on_struct_decl_line, &exp_bls},
  {"bli", PRO_INT, 0, ONOFF_NA, &brace_indent, &exp_bli},
  {"bl", PRO_BOOL, true, OFF, &btype_2, &exp_bl},
  {"bfda", PRO_BOOL, false, ON, &break_function_decl_args, &exp_bfda},
  {"bc", PRO_BOOL, true, OFF, &leave_comma, &exp_bc},
  {"bbb", PRO_BOOL, false, ON, &blanklines_before_blockcomments, &exp_bbb},
  {"bap", PRO_BOOL, false, ON, &blanklines_after_procs, &exp_bap},
  {"badp", PRO_BOOL, false, ON,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"bad", PRO_BOOL, false, ON, &blanklines_after_declarations, &exp_bad},
  {"bacc", PRO_BOOL, false, ON,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"T", PRO_KEY, 0, ONOFF_NA, 0, &exp_T},
/* Signify end of structure.  */
  {0, PRO_IGN, 0, ONOFF_NA, 0, 0}
};

#else /* Default to GNU style */

/* Changed to make GNU style the default. */
struct pro pro[] = {
  {"version", PRO_PRSTRING, 0, ONOFF_NA, (int *) VERSION, &exp_version},
  {"v", PRO_BOOL, false, ON, &verbose, &exp_v},
  {"ut", PRO_BOOL, true, ON, &use_tabs, &exp_ut},
  {"ts", PRO_INT, 8, ONOFF_NA, &tabsize, &exp_ts},
  {"st", PRO_BOOL, false, ON, &use_stdout, &exp_st},
  {"ss", PRO_BOOL, false, ON, &space_sp_semicolon, &exp_ss},
  {"sob", PRO_BOOL, false, ON, &swallow_optional_blanklines, &exp_sob},
  {"sc", PRO_BOOL, false, ON, &star_comment_cont, &exp_sc},
  {"sbi", PRO_INT, 0, ONOFF_NA, &struct_brace_indent, &exp_sbi},
  {"saw", PRO_BOOL, true, ON, &space_after_while, &exp_saw},
  {"sai", PRO_BOOL, true, ON, &space_after_if, &exp_sai},
  {"saf", PRO_BOOL, true, ON, &space_after_for, &exp_saf},
  {"psl", PRO_BOOL, true, ON, &procnames_start_line, &exp_psl},
  {"prs", PRO_BOOL, false, ON, &parentheses_space, &exp_prs},
#ifdef PRESERVE_MTIME
  {"pmt", PRO_BOOL, false, ON, &preserve_mtime, &exp_pmt},
#endif
  {"pi", PRO_INT, -1, ONOFF_NA, &paren_indent, &exp_pi},
  {"pcs", PRO_BOOL, true, ON, &proc_calls_space, &exp_pcs},
  {"orig", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbap\0-nbad\0-bbo\0-hnl\0-bc\0-br\0-brs\0-c33\0-cd33\0-cdb\0\
-ce\0-ci4\0-cli0\0-cp33\0-di16\0-fc1\0-fca\0-i4\0-l75\0-lp\0-npcs\0-nprs\0\
-psl\0-sc\0-sai\0-saf\0-saw\0-nsob\0-nss\0-ts8\0", &exp_orig},
  {"o", PRO_BOOL, false, ON, &expect_output_file, &expect_output_file},
  {"nv", PRO_BOOL, false, OFF, &verbose, &exp_v},
  {"nut", PRO_BOOL, true, OFF, &use_tabs, &exp_ut},
  {"nss", PRO_BOOL, false, OFF, &space_sp_semicolon, &exp_ss},
  {"nsob", PRO_BOOL, false, OFF, &swallow_optional_blanklines, &exp_sob},
  {"nsc", PRO_BOOL, false, OFF, &star_comment_cont, &exp_sc},
  {"nsaw", PRO_BOOL, true, OFF, &space_after_while, &exp_saw},
  {"nsai", PRO_BOOL, true, OFF, &space_after_if, &exp_sai},
  {"nsaf", PRO_BOOL, true, OFF, &space_after_for, &exp_saf},
  {"npsl", PRO_BOOL, true, OFF, &procnames_start_line, &exp_psl},
  {"nprs", PRO_BOOL, false, OFF, &parentheses_space, &exp_prs},
  {"npro", PRO_IGN, 0, ONOFF_NA, 0, &exp_pro},
#ifdef PRESERVE_MTIME
  {"npmt", PRO_BOOL, false, OFF, &preserve_mtime, &exp_pmt},
#endif
  {"npcs", PRO_BOOL, true, OFF, &proc_calls_space, &exp_pcs},
  {"nlps", PRO_BOOL, false, OFF, &leave_preproc_space, &exp_lps},
  {"nlp", PRO_BOOL, true, OFF, &lineup_to_parens, &exp_lp},
  {"nip", PRO_SETTINGS, 0, ONOFF_NA, (int *) "-ip0\0", &exp_nip},
  {"nhnl", PRO_BOOL, true, OFF, &honour_newlines, &exp_hnl},
  {"nfca", PRO_BOOL, false, OFF, &format_comments, &exp_fca},
  {"nfc1", PRO_BOOL, false, OFF, &format_col1_comments, &exp_fc1},
  {"neei", PRO_BOOL, false, OFF, &extra_expression_indent, &exp_eei},
  {"ndj", PRO_BOOL, false, OFF, &ljust_decl, &exp_dj},
  {"ncs", PRO_BOOL, true, OFF, &cast_space, &exp_cs},
  {"nce", PRO_BOOL, false, OFF, &cuddle_else, &exp_ce},
  {"ncdw", PRO_BOOL, false, OFF, &cuddle_do_while, &exp_cdw},
  {"ncdb", PRO_BOOL, false, OFF, &comment_delimiter_on_blankline, &exp_cdb},
  {"nbs", PRO_BOOL, false, OFF, &blank_after_sizeof, &exp_bs},
  {"nbfda", PRO_BOOL, false, OFF, &break_function_decl_args, &exp_bfda},
  {"nbc", PRO_BOOL, true, ON, &leave_comma, &exp_bc},
  {"nbbo", PRO_BOOL, true, OFF, &break_before_boolean_operator, &exp_bbo},
  {"nbbb", PRO_BOOL, false, OFF, &blanklines_before_blockcomments, &exp_bbb},
  {"nbap", PRO_BOOL, true, OFF, &blanklines_after_procs, &exp_bap},
  {"nbadp", PRO_BOOL, false, OFF,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"nbad", PRO_BOOL, false, OFF, &blanklines_after_declarations, &exp_bad},
  {"nbacc", PRO_BOOL, false, OFF,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"lps", PRO_BOOL, false, ON, &leave_preproc_space, &exp_lps},
  {"lp", PRO_BOOL, true, ON, &lineup_to_parens, &exp_lp},
  {"lc", PRO_INT, DEFAULT_RIGHT_COMMENT_MARGIN,
   ONOFF_NA, &comment_max_col, &exp_lc},
  {"l", PRO_INT, DEFAULT_RIGHT_MARGIN, ONOFF_NA, &max_col, &exp_l},
  {"kr", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-nbc\0-bbo\0-hnl\0-br\0-brs\0-c33\0-cd33\0\
-ncdb\0-ce\0-ci4\0-cli0\0-d0\0-di1\0-nfc1\0-i4\0-ip0\0-l75\0-lp\0\
-npcs\0-nprs\0-npsl\0-sai\0-saf\0-saw\0-cs\0-nsc\0-nsob\0-nfca\0-cp33\0-nss\0", &exp_kr},
  {"ip", PRO_INT, 5, ONOFF_NA, &indent_parameters, &exp_ip},
  {"i", PRO_INT, 2, ONOFF_NA, &ind_size, &exp_i},
  {"hnl", PRO_BOOL, true, ON, &honour_newlines, &exp_hnl},
  {"h", PRO_FUNCTION, 0, ONOFF_NA, (int *) usage, &exp_version},
/* This is now the default. */
  {"gnu", PRO_SETTINGS, 0, ONOFF_NA,
   (int *) "-nbad\0-bap\0-bbo\0-hnl\0-nbc\0-bl\0-bls\0-ncdb\0-cs\0\
-nce\0-di2\0-ndj\0-nfc1\0-i2\0-ip5\0-lp\0-pcs\0-nprs\0-psl\0\
-nsc\0-sai\0-saf\0-saw\0-nsob\0-bli2\0-cp1\0-nfca\0", &exp_gnu},
  {"fca", PRO_BOOL, false, ON, &format_comments, &exp_fca},
  {"fc1", PRO_BOOL, false, ON, &format_col1_comments, &exp_fc1},
  {"eei", PRO_BOOL, false, ON, &extra_expression_indent, &exp_eei},
  {"dj", PRO_BOOL, false, ON, &ljust_decl, &exp_dj},
  {"di", PRO_INT, 2, ONOFF_NA, &decl_indent, &exp_di},
  {"d", PRO_INT, 0, ONOFF_NA, &unindent_displace, &exp_d},
  {"cs", PRO_BOOL, true, ON, &cast_space, &exp_cs},
  {"cp", PRO_INT, 1, ONOFF_NA, &else_endif_col, &exp_cp},
  {"cli", PRO_INT, 0, ONOFF_NA, &case_indent, &exp_cli},
  {"ci", PRO_INT, 0, ONOFF_NA, &continuation_indent, &exp_ci},
  {"ce", PRO_BOOL, false, ON, &cuddle_else, &exp_ce},
  {"cdw", PRO_BOOL, false, ON, &cuddle_do_while, &exp_cdw},
  {"cdb", PRO_BOOL, false, ON, &comment_delimiter_on_blankline, &exp_cdb},
  {"cd", PRO_INT, 33, ONOFF_NA, &decl_com_ind, &exp_cd},
  {"cbi", PRO_INT, -1, ONOFF_NA, &case_brace_indent, &exp_cbi},
  {"c++", PRO_BOOL, false, ON, &c_plus_plus, &exp_cpp},
  {"c", PRO_INT, 33, ONOFF_NA, &com_ind, &exp_c},
  {"bbo", PRO_BOOL, true, ON, &break_before_boolean_operator, &exp_bbo},
  {"bs", PRO_BOOL, false, ON, &blank_after_sizeof, &exp_bs},
  {"brs", PRO_BOOL, false, ON, &braces_on_struct_decl_line, &exp_bls},
  {"br", PRO_BOOL, false, ON, &btype_2, &exp_bl},
  {"bls", PRO_BOOL, false, OFF, &braces_on_struct_decl_line, &exp_bls},
  {"bli", PRO_INT, 2, ONOFF_NA, &brace_indent, &exp_bli},
  {"bl", PRO_BOOL, false, OFF, &btype_2, &exp_bl},
  {"bfda", PRO_BOOL, false, ON, &break_function_decl_args, &exp_bfda},
  {"bc", PRO_BOOL, true, OFF, &leave_comma, &exp_bc},
  {"bbb", PRO_BOOL, false, ON, &blanklines_before_blockcomments, &exp_bbb},
  {"bap", PRO_BOOL, true, ON, &blanklines_after_procs, &exp_bap},
  {"badp", PRO_BOOL, false, ON,
   &blanklines_after_declarations_at_proctop, &exp_badp},
  {"bad", PRO_BOOL, false, ON, &blanklines_after_declarations, &exp_bad},
  {"bacc", PRO_BOOL, false, ON,
   &blanklines_around_conditional_compilation, &exp_bacc},
  {"T", PRO_KEY, 0, ONOFF_NA, 0, &exp_T},
/* Signify end of structure.  */
  {0, PRO_IGN, 0, ONOFF_NA, 0, 0}
};

#endif /* GNU defaults */

struct long_option_conversion
{
  char *long_name;
  char *short_name;
};

struct long_option_conversion option_conversions[] = {
  {"version", "version"},
  {"verbose", "v"},
  {"usage", "h"},
  {"use-tabs", "ut"},
  {"tab-size", "ts"},
  {"swallow-optional-blank-lines", "sob"},
  {"struct-brace-indentation", "sbi"},
  {"start-left-side-of-comments", "sc"},
  {"standard-output", "st"},
  {"space-special-semicolon", "ss"},
  {"space-after-while", "saw"},
  {"space-after-procedure-calls", "pcs"},
  {"space-after-parentheses", "prs"},
  {"space-after-if", "sai"},
  {"space-after-for", "saf"},
  {"space-after-cast", "cs"},
  {"remove-preprocessor-space", "nlps"},
  {"procnames-start-lines", "psl"},
#ifdef PRESERVE_MTIME
  {"preserve-mtime", "pmt"},
#endif
  {"paren-indentation", "pi"},
  {"parameter-indentation", "ip"},
  {"output-file", "o"},
  {"output", "o"},
  {"original-style", "orig"},
  {"original", "orig"},
  {"no-verbosity", "nv"},
  {"no-tabs", "nt"},
  {"no-space-after-while", "nsaw"},
  {"no-space-after-parentheses", "nprs"},
  {"no-space-after-if", "nsai"},
  {"no-space-after-function-call-names", "npcs"},
  {"no-space-after-for", "nsaf"},
  {"no-space-after-casts", "ncs"},
  {"no-parameter-indentation", "nip"},
  {"no-extra-expression-indentation", "neei"},
  {"no-comment-delimiters-on-blank-lines", "ncdb"},
  {"no-blank-lines-before-block-comments", "nbbb"},
  {"no-blank-lines-after-procedures", "nbap"},
  {"no-blank-lines-after-procedure-declarations", "nbadp"},
  {"no-blank-lines-after-ifdefs", "nbacc"},
  {"no-blank-lines-after-declarations", "nbad"},
  {"no-blank-lines-after-commas", "nbc"},
  {"no-blank-before-sizeof", "nbs"},
  {"no-Bill-Shannon", "nbs"},
  {"line-length", "l"},
  {"line-comments-indentation", "d"},
  {"left-justify-declarations", "dj"},
  {"leave-preprocessor-space", "lps"},
  {"leave-optional-blank-lines", "nsob"},
  {"kernighan-and-ritchie-style", "kr"},
  {"kernighan-and-ritchie", "kr"},
  {"k-and-r-style", "kr"},
  {"indentation-level", "i"},
  {"indent-level", "i"},
  {"ignore-profile", "npro"},
  {"ignore-newlines", "nhnl"},
  {"honour-newlines", "hnl"},
  {"help", "h"},
  {"gnu-style", "gnu"},
  {"format-first-column-comments", "fc1"},
  {"format-all-comments", "fca"},
  {"extra-expression-indentation", "eei"},
  {"else-endif-column", "cp"},
  {"dont-star-comments", "nsc"},
  {"dont-space-special-semicolon", "nss"},
  {"dont-line-up-parentheses", "nlp"},
  {"dont-left-justify-declarations", "ndj"},
  {"dont-indent-parameters", "nip"},
  {"dont-format-first-column-comments", "nfc1"},
  {"dont-format-comments", "nfca"},
  {"dont-cuddle-else", "nce"},
  {"dont-cuddle-do-while", "ncdw"},
  {"dont-break-procedure-type", "npsl"},
  {"dont-break-function-decl-args", "nbfda"},
  {"declaration-indentation", "di"},
  {"declaration-comment-column", "cd"},
  {"cuddle-else", "ce"},
  {"cuddle-do-while", "cdw"},
  {"continue-at-parentheses", "lp"},
  {"continuation-indentation", "ci"},
  {"comment-line-length", "lc"},
  {"comment-indentation", "c"},
  {"comment-delimiters-on-blank-lines", "cdb"},
  {"case-indentation", "cli"},
  {"case-brace-indentation", "cbi"},
  {"c-plus-plus", "c++"},
  {"break-function-decl-args", "bfda"},
  {"break-before-boolean-operator", "bbo"},
  {"break-after-boolean-operator", "nbbo"},
  {"braces-on-struct-decl-line", "brs"},
  {"braces-on-if-line", "br"},
  {"braces-after-struct-decl-line", "bls"},
  {"braces-after-if-line", "bl"},
  {"brace-indent", "bli"},
  {"blank-lines-before-block-comments", "bbb"},
  {"blank-lines-after-procedures", "bap"},
  {"blank-lines-after-procedure-declarations", "badp"},
  {"blank-lines-after-ifdefs", "bacc"},
  {"blank-lines-after-declarations", "bad"},
  {"blank-lines-after-commas", "bc"},
  {"blank-before-sizeof", "bs"},
  {"berkeley-style", "orig"},
  {"berkeley", "orig"},
  {"Bill-Shannon", "bs"},
  {0, 0},
};

/* S1 should be a string.  S2 should be a string, perhaps followed by an
   argument.  Compare the two, returning true if they are equal, and if they
   are equal set *START_PARAM to point to the argument in S2.  */

static int
eqin (s1, s2, start_param)
     char *s1;
     char *s2;
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
  struct pro *p;

  for (p = pro; p->p_name; p++)
    if ((p->p_type == PRO_BOOL && p->p_special == ON) || p->p_type == PRO_INT)
      *p->p_obj = p->p_default;
}

/* Set the defaults after options set */
void
set_defaults_after ()
{
  if (!exp_lc)			/* if no -lc option was given */
    comment_max_col = max_col;
}

/* Stings which can prefix an option, longest first. */
static char *option_prefixes[] = {
  "--",
  "-",
  "+",
  0
};

static int
option_prefix (arg)
     char *arg;
{
  char **prefixes = option_prefixes;
  char *this_prefix, *argp;

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
  int option_length, val;

  val = 0;
  option_length = option_prefix (option);
  if (option_length > 0)
    {
      if (option_length == 1 && *option == '-')
	/* Short option prefix */
	{
	  option++;
	  for (p = pro; p->p_name; p++)
	    if (*p->p_name == *option && eqin (p->p_name, option, &param_start))
	      goto found;
	}
      else
	/* Long prefix */
	{
	  struct long_option_conversion *o = option_conversions;

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
  exit (invocation_error);

arg_missing:
  fprintf (stderr, "indent: missing argument to parameter %s\n", option);
  exit (invocation_error);

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
	  /* This is not really an error, but zero exit values are
	     returned only when code has been successfully formatted. */
	  printf ("GNU indent %s\n", (char *) p->p_obj);
	  exit (invocation_error);

	case PRO_FUNCTION:
	  ((void (*)()) p->p_obj) ();
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
	    char *str;

	    if (*param_start == 0)
	      {
		if (!(param_start = param))
		  goto arg_missing;
		else
		  val = 1;
	      }

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
	    {
	      if (!(param_start = param))
		goto arg_missing;
	      else
		val = 1;
	    }

	  if (!isdigit (*param_start) && *param_start != '-')
	    {
	      fprintf (stderr, "indent: option ``%s'' requires a numeric parameter\n", option - 1);
	      exit (invocation_error);
	    }
	  *p->p_obj = atoi (param_start);
	  break;

	default:
	  fprintf (stderr, "indent: set_option: internal error: p_type %d\n", (int) p->p_type);
	  exit (invocation_error);
	}
    }

  return val;
}


/* Scan the options in the file F. */

static void
scan_profile (f)
     FILE *f;
{
  int i;
  char *p, *this, *next, *temp;
  char b0[BUFSIZ];
  char b1[BUFSIZ];

  next = b0;
  this = 0;
  while (1)
    {
      for (p = next; ((i = getc (f)) != EOF && (*p = i) > ' ' && i != '/' && p < next + BUFSIZ); ++p);

      if (i == '/')
	{
	  i = getc (f);
	  switch (i)
	    {
	    case '/':
	      do
		i = getc (f);
	      while (i != EOF && i != EOL);
	      if (i == EOF)
		return;
	      continue;

	    case '*':
	      do
		{
		  do
		    i = getc (f);
		  while (i != EOF && i != '*');
		  if (i == '*')
		    i = getc (f);
		  if (i == EOF)
		    {
		      WARNING ("Profile contains unpalatable characters", 0, 0);
		      return;
		    }
		}
	      while (i != '/');
	      continue;

	    default:
	      WARNING ("Profile contains unpalatable characters", 0, 0);
	      if (i == EOF)
		return;
	      continue;
	    }
	}

      /* If we've scanned something... */
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
  FILE *f;
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
      (void) memcpy (&fname[2], INDENT_PROFILE, len - 3);
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
