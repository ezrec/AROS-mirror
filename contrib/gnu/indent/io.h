/* Copyright (c) 1999 Carlo Wood.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef INDENT_IO_H
#define INDENT_IO_H

RCSTAG_H (io, "$Id$");

struct file_buffer
{
  char *name;
  unsigned long size;
  char *data;
};

enum bb_code
{
  bb_none,
  bb_comma,
  bb_embedded_comment_start,
  bb_embedded_comment_end,
  bb_proc_call,
  bb_dec_ind,
  bb_unary_op,
  bb_binary_op,
  bb_before_boolean_binary_op,
  bb_after_boolean_binary_op,
  bb_after_equal_sign,
  bb_comparisation,
  bb_question,
  bb_colon,
  bb_label,
  bb_semicolon,
  bb_lbrace,
  bb_rbrace,
  bb_overloaded,
  bb_const_qualifier,
  bb_ident,
  bb_attribute,
  bb_struct_delim,
  bb_operator2,			/* member selection (bb_struct_delim `.' or `->') */
  bb_operator4,			/* member selection (bb_struct_delim `.*' or `->*') */
  bb_operator5,			/* multiply, divide or modulo */
  bb_operator6,			/* add or subtract */
  bb_doublecolon,
  bb_cast
};

extern struct buf_break_st *buf_break;

struct stat;
extern struct file_buffer *read_file PARAMS ((char *filename, struct stat *));
extern struct file_buffer *read_stdin PARAMS ((void));
extern int count_columns PARAMS ((int column, char *bp, int stop_char));
extern int current_column PARAMS ((void));
extern void fill_buffer PARAMS ((void));
extern void dump_line PARAMS ((int force_nl));
extern int compute_code_target PARAMS ((void));
extern int compute_label_target PARAMS ((void));
extern void clear_buf_break_list PARAMS ((void));
extern void set_buf_break PARAMS ((enum bb_code code));
extern int buf_break_used;

#endif /* INDENT_IO_H */
