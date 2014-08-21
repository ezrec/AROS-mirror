/* Automatically generated header! Do not edit! */

#ifndef _VBCCINLINE_CAIRO_H
#define _VBCCINLINE_CAIRO_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EMUL_EMULREGS_H
#include <emul/emulregs.h>
#endif

cairo_status_t  __cairo_surface_write_to_png(cairo_surface_t *, const char *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1072(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_write_to_png(__p0, __p1) __cairo_surface_write_to_png((__p0), (__p1))

void  __cairo_rel_move_to(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-292(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_rel_move_to(__p0, __p1, __p2) __cairo_rel_move_to((__p0), (__p1), (__p2))

void  __cairo_font_options_set_subpixel_order(cairo_font_options_t *, cairo_subpixel_order_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-526(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_set_subpixel_order(__p0, __p1) __cairo_font_options_set_subpixel_order((__p0), (__p1))

cairo_scaled_font_t * __cairo_get_scaled_font(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-616(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_scaled_font(__p0) __cairo_get_scaled_font((__p0))

cairo_user_scaled_font_init_func_t  __cairo_user_font_face_get_init_func(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-868(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_get_init_func(__p0) __cairo_user_font_face_get_init_func((__p0))

int  __cairo_image_surface_get_stride(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1204(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_get_stride(__p0) __cairo_image_surface_get_stride((__p0))

void  __cairo_select_font_face(cairo_t *, const char *, cairo_font_slant_t , cairo_font_weight_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-562(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_select_font_face(__p0, __p1, __p2, __p3) __cairo_select_font_face((__p0), (__p1), (__p2), (__p3))

cairo_font_weight_t  __cairo_toy_font_face_get_weight(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-832(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_toy_font_face_get_weight(__p0) __cairo_toy_font_face_get_weight((__p0))

void  __cairo_get_dash(cairo_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-964(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_dash(__p0, __p1, __p2) __cairo_get_dash((__p0), (__p1), (__p2))

cairo_surface_t * __cairo_surface_create_observer(cairo_surface_t *, cairo_surface_observer_mode_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1618(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_create_observer(__p0, __p1) __cairo_surface_create_observer((__p0), (__p1))

void  __cairo_move_to(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-256(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_move_to(__p0, __p1, __p2) __cairo_move_to((__p0), (__p1), (__p2))

void  __cairo_scaled_font_destroy(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-724(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_destroy(__p0) __cairo_scaled_font_destroy((__p0))

void  __cairo_rectangle_list_destroy(cairo_rectangle_list_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-442(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_rectangle_list_destroy(__p0) __cairo_rectangle_list_destroy((__p0))

void  __cairo_text_path(cairo_t *, const char *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-640(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_text_path(__p0, __p1) __cairo_text_path((__p0), (__p1))

void  __cairo_pattern_set_matrix(cairo_pattern_t *, const cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1306(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_set_matrix(__p0, __p1) __cairo_pattern_set_matrix((__p0), (__p1))

void  __cairo_device_to_user(cairo_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-238(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_to_user(__p0, __p1, __p2) __cairo_device_to_user((__p0), (__p1), (__p2))

cairo_scaled_font_t * __cairo_scaled_font_create(cairo_font_face_t *, const cairo_matrix_t *, const cairo_matrix_t *, const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-712(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_create(__p0, __p1, __p2, __p3) __cairo_scaled_font_create((__p0), (__p1), (__p2), (__p3))

cairo_bool_t  __cairo_ps_surface_get_eps(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2116(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_get_eps(__p0) __cairo_ps_surface_get_eps((__p0))

void  __cairo_ft_font_options_substitute(const cairo_font_options_t *, FcPattern *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1480(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_font_options_substitute(__p0, __p1) __cairo_ft_font_options_substitute((__p0), (__p1))

cairo_surface_t * __cairo_get_target(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-976(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_target(__p0) __cairo_get_target((__p0))

cairo_hint_style_t  __cairo_font_options_get_hint_style(const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-544(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_get_hint_style(__p0) __cairo_font_options_get_hint_style((__p0))

void * __cairo_scaled_font_get_user_data(cairo_scaled_font_t *, const cairo_user_data_key_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-748(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_user_data(__p0, __p1) __cairo_scaled_font_get_user_data((__p0), (__p1))

cairo_font_face_t * __cairo_toy_font_face_create(const char *, cairo_font_slant_t , cairo_font_weight_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-814(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_toy_font_face_create(__p0, __p1, __p2) __cairo_toy_font_face_create((__p0), (__p1), (__p2))

cairo_region_t * __cairo_region_create_rectangles(const cairo_rectangle_int_t *, int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1918(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_create_rectangles(__p0, __p1) __cairo_region_create_rectangles((__p0), (__p1))

void  __cairo_path_destroy(cairo_path_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1006(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_path_destroy(__p0) __cairo_path_destroy((__p0))

void  __cairo_set_matrix(cairo_t *, const cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-214(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_matrix(__p0, __p1) __cairo_set_matrix((__p0), (__p1))

cairo_status_t  __cairo_surface_observer_add_fill_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1636(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_fill_callback(__p0, __p1, __p2) __cairo_surface_observer_add_fill_callback((__p0), (__p1), (__p2))

void  __cairo_mesh_pattern_curve_to(cairo_pattern_t *, double , double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1846(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_curve_to(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_mesh_pattern_curve_to((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

void  __cairo_pattern_set_extend(cairo_pattern_t *, cairo_extend_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1318(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_set_extend(__p0, __p1) __cairo_pattern_set_extend((__p0), (__p1))

void  __cairo_pop_group_to_source(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-106(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pop_group_to_source(__p0) __cairo_pop_group_to_source((__p0))

cairo_status_t  __cairo_region_union(cairo_region_t *, const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2020(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_union(__p0, __p1) __cairo_region_union((__p0), (__p1))

cairo_glyph_t * __cairo_glyph_allocate(int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-448(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_glyph_allocate(__p0) __cairo_glyph_allocate((__p0))

void  __cairo_matrix_rotate(cairo_matrix_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1420(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_rotate(__p0, __p1) __cairo_matrix_rotate((__p0), (__p1))

cairo_status_t  __cairo_surface_observer_add_paint_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1624(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_paint_callback(__p0, __p1, __p2) __cairo_surface_observer_add_paint_callback((__p0), (__p1), (__p2))

cairo_status_t  __cairo_surface_observer_add_stroke_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1642(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_stroke_callback(__p0, __p1, __p2) __cairo_surface_observer_add_stroke_callback((__p0), (__p1), (__p2))

unsigned int  __cairo_surface_get_reference_count(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1048(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_reference_count(__p0) __cairo_surface_get_reference_count((__p0))

void  __cairo_svg_surface_restrict_to_version(cairo_surface_t *, cairo_svg_version_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2158(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_svg_surface_restrict_to_version(__p0, __p1) __cairo_svg_surface_restrict_to_version((__p0), (__p1))

void  __cairo_clip_preserve(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-424(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_clip_preserve(__p0) __cairo_clip_preserve((__p0))

cairo_surface_t * __cairo_image_surface_create_for_data(unsigned char *, cairo_format_t , int , int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1174(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_create_for_data(__p0, __p1, __p2, __p3, __p4) __cairo_image_surface_create_for_data((__p0), (__p1), (__p2), (__p3), (__p4))

void  __cairo_show_glyphs(cairo_t *, const cairo_glyph_t *, int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-628(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_show_glyphs(__p0, __p1, __p2) __cairo_show_glyphs((__p0), (__p1), (__p2))

cairo_surface_t * __cairo_recording_surface_create(cairo_content_t , const cairo_rectangle_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1744(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_recording_surface_create(__p0, __p1) __cairo_recording_surface_create((__p0), (__p1))

unsigned char * __cairo_image_surface_get_data(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1180(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_get_data(__p0) __cairo_image_surface_get_data((__p0))

void  __cairo_glyph_extents(cairo_t *, const cairo_glyph_t *, int , cairo_text_extents_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-658(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_glyph_extents(__p0, __p1, __p2, __p3) __cairo_glyph_extents((__p0), (__p1), (__p2), (__p3))

cairo_status_t  __cairo_font_face_status(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-688(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_status(__p0) __cairo_font_face_status((__p0))

void  __cairo_text_cluster_free(cairo_text_cluster_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-466(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_text_cluster_free(__p0) __cairo_text_cluster_free((__p0))

cairo_font_face_t * __cairo_ft_font_face_create_for_pattern(FcPattern *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1474(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_font_face_create_for_pattern(__p0) __cairo_ft_font_face_create_for_pattern((__p0))

void  __cairo_show_text_glyphs(cairo_t *, const char *, int , const cairo_glyph_t *, int , const cairo_text_cluster_t *, int , cairo_text_cluster_flags_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-634(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_show_text_glyphs(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7) __cairo_show_text_glyphs((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6), (__p7))

cairo_pattern_t * __cairo_pattern_create_linear(double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1240(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_linear(__p0, __p1, __p2, __p3) __cairo_pattern_create_linear((__p0), (__p1), (__p2), (__p3))

cairo_raster_source_finish_func_t  __cairo_raster_source_pattern_get_finish(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1822(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_get_finish(__p0) __cairo_raster_source_pattern_get_finish((__p0))

cairo_font_options_t * __cairo_font_options_create() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-472(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_create() __cairo_font_options_create()

unsigned int  __cairo_scaled_font_get_reference_count(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-730(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_reference_count(__p0) __cairo_scaled_font_get_reference_count((__p0))

void  __cairo_surface_set_fallback_resolution(cairo_surface_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1132(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_set_fallback_resolution(__p0, __p1, __p2) __cairo_surface_set_fallback_resolution((__p0), (__p1), (__p2))

cairo_bool_t  __cairo_has_current_point(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-916(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_has_current_point(__p0) __cairo_has_current_point((__p0))

cairo_font_type_t  __cairo_scaled_font_get_type(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-742(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_type(__p0) __cairo_scaled_font_get_type((__p0))

void  __cairo_set_source_surface(cairo_t *, cairo_surface_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-136(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_source_surface(__p0, __p1, __p2, __p3) __cairo_set_source_surface((__p0), (__p1), (__p2), (__p3))

double  __cairo_get_line_width(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-934(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_line_width(__p0) __cairo_get_line_width((__p0))

void  __cairo_mesh_pattern_begin_patch(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1834(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_begin_patch(__p0) __cairo_mesh_pattern_begin_patch((__p0))

cairo_surface_t * __cairo_surface_create_similar_image(cairo_surface_t *, cairo_format_t , int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1594(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_create_similar_image(__p0, __p1, __p2, __p3) __cairo_surface_create_similar_image((__p0), (__p1), (__p2), (__p3))

void  __cairo_mesh_pattern_set_corner_color_rgba(cairo_pattern_t *, unsigned int , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1876(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_set_corner_color_rgba(__p0, __p1, __p2, __p3, __p4, __p5) __cairo_mesh_pattern_set_corner_color_rgba((__p0), (__p1), (__p2), (__p3), (__p4), (__p5))

void  __cairo_raster_source_pattern_set_acquire(cairo_pattern_t *, cairo_raster_source_acquire_func_t , cairo_raster_source_release_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1780(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_set_acquire(__p0, __p1, __p2) __cairo_raster_source_pattern_set_acquire((__p0), (__p1), (__p2))

void  __cairo_font_extents(cairo_t *, cairo_font_extents_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-664(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_extents(__p0, __p1) __cairo_font_extents((__p0), (__p1))

double  __cairo_get_miter_limit(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-952(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_miter_limit(__p0) __cairo_get_miter_limit((__p0))

cairo_bool_t  __cairo_region_contains_point(const cairo_region_t *, int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1984(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_contains_point(__p0, __p1, __p2) __cairo_region_contains_point((__p0), (__p1), (__p2))

void  __cairo_matrix_init_scale(cairo_matrix_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1396(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_init_scale(__p0, __p1, __p2) __cairo_matrix_init_scale((__p0), (__p1), (__p2))

void  __cairo_glyph_free(cairo_glyph_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-454(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_glyph_free(__p0) __cairo_glyph_free((__p0))

void  __cairo_matrix_scale(cairo_matrix_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1414(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_scale(__p0, __p1, __p2) __cairo_matrix_scale((__p0), (__p1), (__p2))

cairo_status_t  __cairo_surface_write_to_png_stream(cairo_surface_t *, cairo_write_func_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1078(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_write_to_png_stream(__p0, __p1, __p2) __cairo_surface_write_to_png_stream((__p0), (__p1), (__p2))

void * __cairo_surface_get_user_data(cairo_surface_t *, const cairo_user_data_key_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1084(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_user_data(__p0, __p1) __cairo_surface_get_user_data((__p0), (__p1))

void  __cairo_ft_font_face_unset_synthesize(cairo_font_face_t *, unsigned int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2050(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_font_face_unset_synthesize(__p0, __p1) __cairo_ft_font_face_unset_synthesize((__p0), (__p1))

cairo_hint_metrics_t  __cairo_font_options_get_hint_metrics(const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-556(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_get_hint_metrics(__p0) __cairo_font_options_get_hint_metrics((__p0))

void  __cairo_pattern_add_color_stop_rgb(cairo_pattern_t *, double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1294(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_add_color_stop_rgb(__p0, __p1, __p2, __p3, __p4) __cairo_pattern_add_color_stop_rgb((__p0), (__p1), (__p2), (__p3), (__p4))

void  __cairo_new_path(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-250(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_new_path(__p0) __cairo_new_path((__p0))

void  __cairo_set_font_options(cairo_t *, const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-586(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_font_options(__p0, __p1) __cairo_set_font_options((__p0), (__p1))

double  __cairo_device_observer_mask_elapsed(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1696(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_mask_elapsed(__p0) __cairo_device_observer_mask_elapsed((__p0))

void  __cairo_region_translate(cairo_region_t *, int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1990(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_translate(__p0, __p1, __p2) __cairo_region_translate((__p0), (__p1), (__p2))

cairo_status_t  __cairo_surface_observer_add_mask_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1630(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_mask_callback(__p0, __p1, __p2) __cairo_surface_observer_add_mask_callback((__p0), (__p1), (__p2))

void  __cairo_set_font_matrix(cairo_t *, const cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-574(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_font_matrix(__p0, __p1) __cairo_set_font_matrix((__p0), (__p1))

void  __cairo_glyph_path(cairo_t *, const cairo_glyph_t *, int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-646(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_glyph_path(__p0, __p1, __p2) __cairo_glyph_path((__p0), (__p1), (__p2))

void  __cairo_set_scaled_font(cairo_t *, const cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-610(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_scaled_font(__p0, __p1) __cairo_set_scaled_font((__p0), (__p1))

cairo_font_face_t * __cairo_user_font_face_create() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-838(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_create() __cairo_user_font_face_create()

void  __cairo_stroke(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-352(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_stroke(__p0) __cairo_stroke((__p0))

cairo_status_t  __cairo_font_face_set_user_data(cairo_font_face_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-706(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_set_user_data(__p0, __p1, __p2, __p3) __cairo_font_face_set_user_data((__p0), (__p1), (__p2), (__p3))

void  __cairo_fill_preserve(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-370(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_fill_preserve(__p0) __cairo_fill_preserve((__p0))

void  __cairo_set_antialias(cairo_t *, cairo_antialias_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-148(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_antialias(__p0, __p1) __cairo_set_antialias((__p0), (__p1))

void  __cairo_mesh_pattern_line_to(cairo_pattern_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1852(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_line_to(__p0, __p1, __p2) __cairo_mesh_pattern_line_to((__p0), (__p1), (__p2))

void  __cairo_path_extents(cairo_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-322(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_path_extents(__p0, __p1, __p2, __p3, __p4) __cairo_path_extents((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_antialias_t  __cairo_get_antialias(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-910(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_antialias(__p0) __cairo_get_antialias((__p0))

void  __cairo_raster_source_pattern_set_finish(cairo_pattern_t *, cairo_raster_source_finish_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1816(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_set_finish(__p0, __p1) __cairo_raster_source_pattern_set_finish((__p0), (__p1))

cairo_font_face_t * __cairo_get_font_face(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-604(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_font_face(__p0) __cairo_get_font_face((__p0))

cairo_surface_t * __cairo_pdf_surface_create_for_stream(cairo_write_func_t , void *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1462(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pdf_surface_create_for_stream(__p0, __p1, __p2, __p3) __cairo_pdf_surface_create_for_stream((__p0), (__p1), (__p2), (__p3))

void  __cairo_scaled_font_glyph_extents(cairo_scaled_font_t *, const cairo_glyph_t *, int , cairo_text_extents_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-772(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_glyph_extents(__p0, __p1, __p2, __p3) __cairo_scaled_font_glyph_extents((__p0), (__p1), (__p2), (__p3))

void  __cairo_device_destroy(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1570(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_destroy(__p0) __cairo_device_destroy((__p0))

void  __cairo_region_get_extents(const cairo_region_t *, cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1954(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_get_extents(__p0, __p1) __cairo_region_get_extents((__p0), (__p1))

void  __cairo_set_font_size(cairo_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-568(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_font_size(__p0, __p1) __cairo_set_font_size((__p0), (__p1))

void  __cairo_fill(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-364(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_fill(__p0) __cairo_fill((__p0))

cairo_antialias_t  __cairo_font_options_get_antialias(const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-520(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_get_antialias(__p0) __cairo_font_options_get_antialias((__p0))

cairo_device_t * __cairo_surface_get_device(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1720(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_device(__p0) __cairo_surface_get_device((__p0))

cairo_path_t * __cairo_copy_path_flat(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-994(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_copy_path_flat(__p0) __cairo_copy_path_flat((__p0))

cairo_font_slant_t  __cairo_toy_font_face_get_slant(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-826(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_toy_font_face_get_slant(__p0) __cairo_toy_font_face_get_slant((__p0))

cairo_status_t  __cairo_device_set_user_data(cairo_device_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1588(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_set_user_data(__p0, __p1, __p2, __p3) __cairo_device_set_user_data((__p0), (__p1), (__p2), (__p3))

void  __cairo_user_to_device_distance(cairo_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-232(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_to_device_distance(__p0, __p1, __p2) __cairo_user_to_device_distance((__p0), (__p1), (__p2))

struct BitMap * __cairo_morphos_surface_get_bitmap(void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1510(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_morphos_surface_get_bitmap(__p0) __cairo_morphos_surface_get_bitmap((__p0))

cairo_status_t  __cairo_device_status(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1540(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_status(__p0) __cairo_device_status((__p0))

void  __cairo_surface_finish(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1036(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_finish(__p0) __cairo_surface_finish((__p0))

cairo_bool_t  __cairo_region_is_empty(const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1972(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_is_empty(__p0) __cairo_region_is_empty((__p0))

cairo_status_t  __cairo_pattern_get_color_stop_count(cairo_pattern_t *, int *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1360(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_color_stop_count(__p0, __p1) __cairo_pattern_get_color_stop_count((__p0), (__p1))

void  __cairo_surface_show_page(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1150(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_show_page(__p0) __cairo_surface_show_page((__p0))

void  __cairo_set_source_rgba(cairo_t *, double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-130(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_source_rgba(__p0, __p1, __p2, __p3, __p4) __cairo_set_source_rgba((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_region_overlap_t  __cairo_region_contains_rectangle(const cairo_region_t *, const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1978(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_contains_rectangle(__p0, __p1) __cairo_region_contains_rectangle((__p0), (__p1))

void  __cairo_surface_mark_dirty(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1108(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_mark_dirty(__p0) __cairo_surface_mark_dirty((__p0))

cairo_surface_t * __cairo_ps_surface_create(const char *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2080(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_create(__p0, __p1, __p2) __cairo_ps_surface_create((__p0), (__p1), (__p2))

cairo_status_t  __cairo_matrix_invert(cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1426(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_invert(__p0) __cairo_matrix_invert((__p0))

void  __cairo_get_matrix(cairo_t *, cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-970(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_matrix(__p0, __p1) __cairo_get_matrix((__p0), (__p1))

cairo_pattern_t * __cairo_pattern_create_mesh() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1828(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_mesh() __cairo_pattern_create_mesh()

cairo_status_t  __cairo_pattern_set_user_data(cairo_pattern_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1282(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_set_user_data(__p0, __p1, __p2, __p3) __cairo_pattern_set_user_data((__p0), (__p1), (__p2), (__p3))

void  __cairo_matrix_translate(cairo_matrix_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1408(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_translate(__p0, __p1, __p2) __cairo_matrix_translate((__p0), (__p1), (__p2))

void * __cairo_device_get_user_data(cairo_device_t *, const cairo_user_data_key_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1582(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_get_user_data(__p0, __p1) __cairo_device_get_user_data((__p0), (__p1))

unsigned long  __cairo_font_options_hash(const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-508(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_hash(__p0) __cairo_font_options_hash((__p0))

cairo_pattern_t * __cairo_pattern_create_rgb(double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1222(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_rgb(__p0, __p1, __p2) __cairo_pattern_create_rgb((__p0), (__p1), (__p2))

const char * __cairo_status_to_string(cairo_status_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1018(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_status_to_string(__p0) __cairo_status_to_string((__p0))

cairo_status_t  __cairo_region_intersect(cairo_region_t *, const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2008(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_intersect(__p0, __p1) __cairo_region_intersect((__p0), (__p1))

void  __cairo_debug_reset_static_data() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1450(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_debug_reset_static_data() __cairo_debug_reset_static_data()

void  __cairo_identity_matrix(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-220(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_identity_matrix(__p0) __cairo_identity_matrix((__p0))

void  __cairo_device_flush(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1558(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_flush(__p0) __cairo_device_flush((__p0))

cairo_status_t  __cairo_mesh_pattern_get_patch_count(cairo_pattern_t *, unsigned int *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1882(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_get_patch_count(__p0, __p1) __cairo_mesh_pattern_get_patch_count((__p0), (__p1))

cairo_surface_t * __cairo_svg_surface_create(const char *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2146(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_svg_surface_create(__p0, __p1, __p2) __cairo_svg_surface_create((__p0), (__p1), (__p2))

void  __cairo_set_source_rgb(cairo_t *, double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-124(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_source_rgb(__p0, __p1, __p2, __p3) __cairo_set_source_rgb((__p0), (__p1), (__p2), (__p3))

const char * __cairo_svg_version_to_string(cairo_svg_version_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2170(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_svg_version_to_string(__p0) __cairo_svg_version_to_string((__p0))

void  __cairo_mesh_pattern_set_control_point(cairo_pattern_t *, unsigned int , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1864(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_set_control_point(__p0, __p1, __p2, __p3) __cairo_mesh_pattern_set_control_point((__p0), (__p1), (__p2), (__p3))

void  __cairo_rotate(cairo_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-202(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_rotate(__p0, __p1) __cairo_rotate((__p0), (__p1))

cairo_status_t  __cairo_surface_observer_print(cairo_surface_t *, cairo_write_func_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1666(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_print(__p0, __p1, __p2) __cairo_surface_observer_print((__p0), (__p1), (__p2))

cairo_rectangle_list_t * __cairo_copy_clip_rectangle_list(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-436(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_copy_clip_rectangle_list(__p0) __cairo_copy_clip_rectangle_list((__p0))

void  __cairo_clip(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-418(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_clip(__p0) __cairo_clip((__p0))

void  __cairo_surface_flush(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1102(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_flush(__p0) __cairo_surface_flush((__p0))

cairo_status_t  __cairo_surface_observer_add_flush_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1654(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_flush_callback(__p0, __p1, __p2) __cairo_surface_observer_add_flush_callback((__p0), (__p1), (__p2))

cairo_status_t  __cairo_region_union_rectangle(cairo_region_t *, const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2026(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_union_rectangle(__p0, __p1) __cairo_region_union_rectangle((__p0), (__p1))

cairo_font_options_t * __cairo_font_options_copy(const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-478(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_copy(__p0) __cairo_font_options_copy((__p0))

void  __cairo_close_path(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-316(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_close_path(__p0) __cairo_close_path((__p0))

cairo_user_scaled_font_render_glyph_func_t  __cairo_user_font_face_get_render_glyph_func(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-874(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_get_render_glyph_func(__p0) __cairo_user_font_face_get_render_glyph_func((__p0))

cairo_extend_t  __cairo_pattern_get_extend(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1324(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_extend(__p0) __cairo_pattern_get_extend((__p0))

cairo_raster_source_copy_func_t  __cairo_raster_source_pattern_get_copy(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1810(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_get_copy(__p0) __cairo_raster_source_pattern_get_copy((__p0))

const char * __cairo_pdf_version_to_string(cairo_pdf_version_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2074(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pdf_version_to_string(__p0) __cairo_pdf_version_to_string((__p0))

void  __cairo_push_group_with_content(cairo_t *, cairo_content_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-94(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_push_group_with_content(__p0, __p1) __cairo_push_group_with_content((__p0), (__p1))

double  __cairo_device_observer_paint_elapsed(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1690(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_paint_elapsed(__p0) __cairo_device_observer_paint_elapsed((__p0))

void  __cairo_restore(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-82(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_restore(__p0) __cairo_restore((__p0))

unsigned int  __cairo_get_reference_count(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-58(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_reference_count(__p0) __cairo_get_reference_count((__p0))

void  __cairo_translate(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-190(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_translate(__p0, __p1, __p2) __cairo_translate((__p0), (__p1), (__p2))

void  __cairo_ps_surface_set_size(cairo_surface_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2122(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_set_size(__p0, __p1, __p2) __cairo_ps_surface_set_size((__p0), (__p1), (__p2))

unsigned int  __cairo_font_face_get_reference_count(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-682(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_get_reference_count(__p0) __cairo_font_face_get_reference_count((__p0))

void  __cairo_surface_copy_page(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1144(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_copy_page(__p0) __cairo_surface_copy_page((__p0))

void  __cairo_set_font_face(cairo_t *, cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-598(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_font_face(__p0, __p1) __cairo_set_font_face((__p0), (__p1))

cairo_status_t  __cairo_mesh_pattern_get_control_point(cairo_pattern_t *, unsigned int , unsigned int , double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1900(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_get_control_point(__p0, __p1, __p2, __p3, __p4) __cairo_mesh_pattern_get_control_point((__p0), (__p1), (__p2), (__p3), (__p4))

const char * __cairo_ps_level_to_string(cairo_ps_level_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2104(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_level_to_string(__p0) __cairo_ps_level_to_string((__p0))

cairo_pattern_t * __cairo_pattern_create_for_surface(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1234(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_for_surface(__p0) __cairo_pattern_create_for_surface((__p0))

void  __cairo_matrix_transform_distance(const cairo_matrix_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1438(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_transform_distance(__p0, __p1, __p2) __cairo_matrix_transform_distance((__p0), (__p1), (__p2))

cairo_font_face_t * __cairo_font_face_reference(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-670(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_reference(__p0) __cairo_font_face_reference((__p0))

cairo_pattern_t * __cairo_pop_group(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-100(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pop_group(__p0) __cairo_pop_group((__p0))

cairo_surface_t * __cairo_ps_surface_create_for_stream(cairo_write_func_t , void *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2086(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_create_for_stream(__p0, __p1, __p2, __p3) __cairo_ps_surface_create_for_stream((__p0), (__p1), (__p2), (__p3))

void  __cairo_text_extents(cairo_t *, const char *, cairo_text_extents_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-652(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_text_extents(__p0, __p1, __p2) __cairo_text_extents((__p0), (__p1), (__p2))

cairo_surface_t * __cairo_morphos_surface_create_from_bitmap(cairo_content_t , int , int , struct BitMap *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1504(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_morphos_surface_create_from_bitmap(__p0, __p1, __p2, __p3) __cairo_morphos_surface_create_from_bitmap((__p0), (__p1), (__p2), (__p3))

cairo_status_t  __cairo_surface_observer_add_glyphs_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1648(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_glyphs_callback(__p0, __p1, __p2) __cairo_surface_observer_add_glyphs_callback((__p0), (__p1), (__p2))

cairo_region_t * __cairo_region_reference(cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1930(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_reference(__p0) __cairo_region_reference((__p0))

void  __cairo_font_face_destroy(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-676(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_destroy(__p0) __cairo_font_face_destroy((__p0))

cairo_pattern_t * __cairo_pattern_create_raster_source(void *, cairo_content_t , int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1762(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_raster_source(__p0, __p1, __p2, __p3) __cairo_pattern_create_raster_source((__p0), (__p1), (__p2), (__p3))

void  __cairo_mesh_pattern_end_patch(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1840(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_end_patch(__p0) __cairo_mesh_pattern_end_patch((__p0))

void  __cairo_copy_page(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-376(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_copy_page(__p0) __cairo_copy_page((__p0))

cairo_status_t  __cairo_region_status(const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1948(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_status(__p0) __cairo_region_status((__p0))

void  __cairo_raster_source_pattern_get_acquire(cairo_pattern_t *, cairo_raster_source_acquire_func_t *, cairo_raster_source_release_func_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1786(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_get_acquire(__p0, __p1, __p2) __cairo_raster_source_pattern_get_acquire((__p0), (__p1), (__p2))

void  __cairo_pattern_get_matrix(cairo_pattern_t *, cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1312(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_matrix(__p0, __p1) __cairo_pattern_get_matrix((__p0), (__p1))

void  __cairo_region_destroy(cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1936(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_destroy(__p0) __cairo_region_destroy((__p0))

void  __cairo_set_line_width(cairo_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-160(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_line_width(__p0, __p1) __cairo_set_line_width((__p0), (__p1))

void  __cairo_get_current_point(cairo_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-922(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_current_point(__p0, __p1, __p2) __cairo_get_current_point((__p0), (__p1), (__p2))

cairo_status_t  __cairo_pattern_get_surface(cairo_pattern_t *, cairo_surface_t **) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1348(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_surface(__p0, __p1) __cairo_pattern_get_surface((__p0), (__p1))

void  __cairo_surface_get_mime_data(cairo_surface_t *, const char *, const unsigned char **, unsigned long *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1726(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_mime_data(__p0, __p1, __p2, __p3) __cairo_surface_get_mime_data((__p0), (__p1), (__p2), (__p3))

unsigned int  __cairo_ft_font_face_get_synthesize(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2056(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_font_face_get_synthesize(__p0) __cairo_ft_font_face_get_synthesize((__p0))

cairo_surface_t * __cairo_image_surface_create_from_png(const char *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1210(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_create_from_png(__p0) __cairo_image_surface_create_from_png((__p0))

void  __cairo_scaled_font_extents(cairo_scaled_font_t *, cairo_font_extents_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-760(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_extents(__p0, __p1) __cairo_scaled_font_extents((__p0), (__p1))

cairo_font_face_t * __cairo_scaled_font_get_font_face(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-784(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_font_face(__p0) __cairo_scaled_font_get_font_face((__p0))

void  __cairo_scaled_font_get_font_matrix(cairo_scaled_font_t *, cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-790(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_font_matrix(__p0, __p1) __cairo_scaled_font_get_font_matrix((__p0), (__p1))

void  __cairo_reset_clip(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-412(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_reset_clip(__p0) __cairo_reset_clip((__p0))

cairo_path_t * __cairo_mesh_pattern_get_path(cairo_pattern_t *, unsigned int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1888(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_get_path(__p0, __p1) __cairo_mesh_pattern_get_path((__p0), (__p1))

void  __cairo_mesh_pattern_set_corner_color_rgb(cairo_pattern_t *, unsigned int , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1870(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_set_corner_color_rgb(__p0, __p1, __p2, __p3, __p4) __cairo_mesh_pattern_set_corner_color_rgb((__p0), (__p1), (__p2), (__p3), (__p4))

void  __cairo_pattern_add_color_stop_rgba(cairo_pattern_t *, double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1300(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_add_color_stop_rgba(__p0, __p1, __p2, __p3, __p4, __p5) __cairo_pattern_add_color_stop_rgba((__p0), (__p1), (__p2), (__p3), (__p4), (__p5))

cairo_status_t  __cairo_pattern_status(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1270(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_status(__p0) __cairo_pattern_status((__p0))

cairo_surface_t * __cairo_svg_surface_create_for_stream(cairo_write_func_t , void *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2152(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_svg_surface_create_for_stream(__p0, __p1, __p2, __p3) __cairo_svg_surface_create_for_stream((__p0), (__p1), (__p2), (__p3))

cairo_region_t * __cairo_region_create_rectangle(const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1912(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_create_rectangle(__p0) __cairo_region_create_rectangle((__p0))

cairo_region_t * __cairo_region_create() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1906(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_create() __cairo_region_create()

void  __cairo_fill_extents(cairo_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-406(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_fill_extents(__p0, __p1, __p2, __p3, __p4) __cairo_fill_extents((__p0), (__p1), (__p2), (__p3), (__p4))

FT_Face  __cairo_ft_scaled_font_lock_face(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1492(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_scaled_font_lock_face(__p0) __cairo_ft_scaled_font_lock_face((__p0))

cairo_font_face_t * __cairo_ft_font_face_create_for_ft_face(FT_Face , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1486(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_font_face_create_for_ft_face(__p0, __p1) __cairo_ft_font_face_create_for_ft_face((__p0), (__p1))

void  __cairo_ps_surface_restrict_to_level(cairo_surface_t *, cairo_ps_level_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2092(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_restrict_to_level(__p0, __p1) __cairo_ps_surface_restrict_to_level((__p0), (__p1))

cairo_status_t  __cairo_region_xor(cairo_region_t *, const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2032(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_xor(__p0, __p1) __cairo_region_xor((__p0), (__p1))

cairo_surface_t * __cairo_pdf_surface_create(const char *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1456(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pdf_surface_create(__p0, __p1, __p2) __cairo_pdf_surface_create((__p0), (__p1), (__p2))

cairo_surface_t * __cairo_surface_create_similar(cairo_surface_t *, cairo_content_t , int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1024(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_create_similar(__p0, __p1, __p2, __p3) __cairo_surface_create_similar((__p0), (__p1), (__p2), (__p3))

cairo_font_type_t  __cairo_font_face_get_type(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-694(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_get_type(__p0) __cairo_font_face_get_type((__p0))

int  __cairo_image_surface_get_width(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1192(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_get_width(__p0) __cairo_image_surface_get_width((__p0))

cairo_bool_t  __cairo_in_stroke(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-388(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_in_stroke(__p0, __p1, __p2) __cairo_in_stroke((__p0), (__p1), (__p2))

void  __cairo_matrix_init_rotate(cairo_matrix_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1402(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_init_rotate(__p0, __p1) __cairo_matrix_init_rotate((__p0), (__p1))

cairo_status_t  __cairo_region_intersect_rectangle(cairo_region_t *, const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2014(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_intersect_rectangle(__p0, __p1) __cairo_region_intersect_rectangle((__p0), (__p1))

void  __cairo_font_options_merge(cairo_font_options_t *, const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-496(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_merge(__p0, __p1) __cairo_font_options_merge((__p0), (__p1))

cairo_status_t  __cairo_device_acquire(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1546(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_acquire(__p0) __cairo_device_acquire((__p0))

cairo_bool_t  __cairo_in_fill(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-394(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_in_fill(__p0, __p1, __p2) __cairo_in_fill((__p0), (__p1), (__p2))

cairo_status_t  __cairo_device_observer_print(cairo_device_t *, cairo_write_func_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1678(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_print(__p0, __p1, __p2) __cairo_device_observer_print((__p0), (__p1), (__p2))

void  __cairo_matrix_init_translate(cairo_matrix_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1390(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_init_translate(__p0, __p1, __p2) __cairo_matrix_init_translate((__p0), (__p1), (__p2))

cairo_content_t  __cairo_surface_get_content(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1066(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_content(__p0) __cairo_surface_get_content((__p0))

void  __cairo_surface_set_device_offset(cairo_surface_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1120(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_set_device_offset(__p0, __p1, __p2) __cairo_surface_set_device_offset((__p0), (__p1), (__p2))

void  __cairo_ft_font_face_set_synthesize(cairo_font_face_t *, unsigned int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2044(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_font_face_set_synthesize(__p0, __p1) __cairo_ft_font_face_set_synthesize((__p0), (__p1))

void  __cairo_scale(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-196(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scale(__p0, __p1, __p2) __cairo_scale((__p0), (__p1), (__p2))

cairo_status_t  __cairo_pattern_get_rgba(cairo_pattern_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1342(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_rgba(__p0, __p1, __p2, __p3, __p4) __cairo_pattern_get_rgba((__p0), (__p1), (__p2), (__p3), (__p4))

void  __cairo_mask(cairo_t *, cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-340(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mask(__p0, __p1) __cairo_mask((__p0), (__p1))

void  __cairo_matrix_init_identity(cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1384(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_init_identity(__p0) __cairo_matrix_init_identity((__p0))

cairo_pattern_t * __cairo_pattern_reference(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1252(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_reference(__p0) __cairo_pattern_reference((__p0))

cairo_raster_source_snapshot_func_t  __cairo_raster_source_pattern_get_snapshot(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1798(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_get_snapshot(__p0) __cairo_raster_source_pattern_get_snapshot((__p0))

void  __cairo_set_operator(cairo_t *, cairo_operator_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-112(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_operator(__p0, __p1) __cairo_set_operator((__p0), (__p1))

cairo_surface_t * __cairo_surface_map_to_image(cairo_surface_t *, const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1600(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_map_to_image(__p0, __p1) __cairo_surface_map_to_image((__p0), (__p1))

cairo_status_t  __cairo_pattern_get_color_stop_rgba(cairo_pattern_t *, int , double *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1354(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_color_stop_rgba(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_pattern_get_color_stop_rgba((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

void  __cairo_ps_surface_set_eps(cairo_surface_t *, cairo_bool_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2110(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_set_eps(__p0, __p1) __cairo_ps_surface_set_eps((__p0), (__p1))

cairo_surface_t * __cairo_morphos_surface_create_for_rastport(cairo_content_t , struct RastPort *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1516(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_morphos_surface_create_for_rastport(__p0, __p1) __cairo_morphos_surface_create_for_rastport((__p0), (__p1))

void  __cairo_user_font_face_set_text_to_glyphs_func(cairo_font_face_t *, cairo_user_scaled_font_text_to_glyphs_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-856(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_set_text_to_glyphs_func(__p0, __p1) __cairo_user_font_face_set_text_to_glyphs_func((__p0), (__p1))

cairo_status_t  __cairo_region_subtract_rectangle(cairo_region_t *, const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2002(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_subtract_rectangle(__p0, __p1) __cairo_region_subtract_rectangle((__p0), (__p1))

void  __cairo_scaled_font_get_scale_matrix(cairo_scaled_font_t *, cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-802(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_scale_matrix(__p0, __p1) __cairo_scaled_font_get_scale_matrix((__p0), (__p1))

void  __cairo_scaled_font_get_font_options(cairo_scaled_font_t *, cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-808(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_font_options(__p0, __p1) __cairo_scaled_font_get_font_options((__p0), (__p1))

cairo_pattern_t * __cairo_get_source(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-898(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_source(__p0) __cairo_get_source((__p0))

cairo_pattern_t * __cairo_pattern_create_rgba(double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1228(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_rgba(__p0, __p1, __p2, __p3) __cairo_pattern_create_rgba((__p0), (__p1), (__p2), (__p3))

cairo_text_cluster_t * __cairo_text_cluster_allocate(int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-460(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_text_cluster_allocate(__p0) __cairo_text_cluster_allocate((__p0))

void  __cairo_surface_get_fallback_resolution(cairo_surface_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1138(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_fallback_resolution(__p0, __p1, __p2) __cairo_surface_get_fallback_resolution((__p0), (__p1), (__p2))

void * __cairo_get_user_data(cairo_t *, const cairo_user_data_key_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-64(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_user_data(__p0, __p1) __cairo_get_user_data((__p0), (__p1))

void  __cairo_surface_unmap_image(cairo_surface_t *, cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1606(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_unmap_image(__p0, __p1) __cairo_surface_unmap_image((__p0), (__p1))

cairo_user_scaled_font_unicode_to_glyph_func_t  __cairo_user_font_face_get_unicode_to_glyph_func(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-886(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_get_unicode_to_glyph_func(__p0) __cairo_user_font_face_get_unicode_to_glyph_func((__p0))

void  __cairo_region_get_rectangle(const cairo_region_t *, int , cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1966(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_get_rectangle(__p0, __p1, __p2) __cairo_region_get_rectangle((__p0), (__p1), (__p2))

cairo_surface_t * __cairo_image_surface_create(cairo_format_t , int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1162(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_create(__p0, __p1, __p2) __cairo_image_surface_create((__p0), (__p1), (__p2))

void  __cairo_device_to_user_distance(cairo_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-244(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_to_user_distance(__p0, __p1, __p2) __cairo_device_to_user_distance((__p0), (__p1), (__p2))

void  __cairo_font_options_set_hint_style(cairo_font_options_t *, cairo_hint_style_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-538(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_set_hint_style(__p0, __p1) __cairo_font_options_set_hint_style((__p0), (__p1))

void  __cairo_pattern_set_filter(cairo_pattern_t *, cairo_filter_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1330(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_set_filter(__p0, __p1) __cairo_pattern_set_filter((__p0), (__p1))

void  __cairo_pdf_surface_set_size(cairo_surface_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1468(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pdf_surface_set_size(__p0, __p1, __p2) __cairo_pdf_surface_set_size((__p0), (__p1), (__p2))

void  __cairo_paint(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-328(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_paint(__p0) __cairo_paint((__p0))

cairo_status_t  __cairo_set_user_data(cairo_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-70(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_user_data(__p0, __p1, __p2, __p3) __cairo_set_user_data((__p0), (__p1), (__p2), (__p3))

void * __cairo_raster_source_pattern_get_callback_data(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1774(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_get_callback_data(__p0) __cairo_raster_source_pattern_get_callback_data((__p0))

void  __cairo_rectangle(cairo_t *, double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-310(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_rectangle(__p0, __p1, __p2, __p3, __p4) __cairo_rectangle((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_line_join_t  __cairo_get_line_join(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-946(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_line_join(__p0) __cairo_get_line_join((__p0))

void  __cairo_append_path(cairo_t *, const cairo_path_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1000(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_append_path(__p0, __p1) __cairo_append_path((__p0), (__p1))

void  __cairo_matrix_init(cairo_matrix_t *, double , double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1378(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_init(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_matrix_init((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

void  __cairo_user_font_face_set_unicode_to_glyph_func(cairo_font_face_t *, cairo_user_scaled_font_unicode_to_glyph_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-862(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_set_unicode_to_glyph_func(__p0, __p1) __cairo_user_font_face_set_unicode_to_glyph_func((__p0), (__p1))

cairo_user_scaled_font_text_to_glyphs_func_t  __cairo_user_font_face_get_text_to_glyphs_func(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-880(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_get_text_to_glyphs_func(__p0) __cairo_user_font_face_get_text_to_glyphs_func((__p0))

void  __cairo_ps_surface_dsc_begin_setup(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2134(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_dsc_begin_setup(__p0) __cairo_ps_surface_dsc_begin_setup((__p0))

void  __cairo_font_options_destroy(cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-484(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_destroy(__p0) __cairo_font_options_destroy((__p0))

void  __cairo_surface_mark_dirty_rectangle(cairo_surface_t *, int , int , int , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1114(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_mark_dirty_rectangle(__p0, __p1, __p2, __p3, __p4) __cairo_surface_mark_dirty_rectangle((__p0), (__p1), (__p2), (__p3), (__p4))

void  __cairo_scaled_font_get_ctm(cairo_scaled_font_t *, cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-796(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_get_ctm(__p0, __p1) __cairo_scaled_font_get_ctm((__p0), (__p1))

cairo_status_t  __cairo_region_subtract(cairo_region_t *, const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1996(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_subtract(__p0, __p1) __cairo_region_subtract((__p0), (__p1))

double  __cairo_device_observer_stroke_elapsed(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1708(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_stroke_elapsed(__p0) __cairo_device_observer_stroke_elapsed((__p0))

cairo_subpixel_order_t  __cairo_font_options_get_subpixel_order(const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-532(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_get_subpixel_order(__p0) __cairo_font_options_get_subpixel_order((__p0))

void  __cairo_scaled_font_text_extents(cairo_scaled_font_t *, const char *, cairo_text_extents_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-766(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_text_extents(__p0, __p1, __p2) __cairo_scaled_font_text_extents((__p0), (__p1), (__p2))

void  __cairo_curve_to(cairo_t *, double , double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-274(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_curve_to(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_curve_to((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

void  __cairo_set_line_cap(cairo_t *, cairo_line_cap_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-166(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_line_cap(__p0, __p1) __cairo_set_line_cap((__p0), (__p1))

void  __cairo_font_options_set_antialias(cairo_font_options_t *, cairo_antialias_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-514(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_set_antialias(__p0, __p1) __cairo_font_options_set_antialias((__p0), (__p1))

cairo_status_t  __cairo_pattern_get_radial_circles(cairo_pattern_t *, double *, double *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1372(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_radial_circles(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_pattern_get_radial_circles((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

double  __cairo_get_tolerance(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-904(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_tolerance(__p0) __cairo_get_tolerance((__p0))

void  __cairo_set_miter_limit(cairo_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-184(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_miter_limit(__p0, __p1) __cairo_set_miter_limit((__p0), (__p1))

void  __cairo_rel_line_to(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-298(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_rel_line_to(__p0, __p1, __p2) __cairo_rel_line_to((__p0), (__p1), (__p2))

cairo_operator_t  __cairo_get_operator(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-892(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_operator(__p0) __cairo_get_operator((__p0))

cairo_surface_t * __cairo_image_surface_create_from_png_stream(cairo_read_func_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1216(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_create_from_png_stream(__p0, __p1) __cairo_image_surface_create_from_png_stream((__p0), (__p1))

int  __cairo_format_stride_for_width(cairo_format_t , int ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1168(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_format_stride_for_width(__p0, __p1) __cairo_format_stride_for_width((__p0), (__p1))

cairo_line_cap_t  __cairo_get_line_cap(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-940(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_line_cap(__p0) __cairo_get_line_cap((__p0))

cairo_format_t  __cairo_image_surface_get_format(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1186(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_get_format(__p0) __cairo_image_surface_get_format((__p0))

void  __cairo_transform(cairo_t *, const cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-208(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_transform(__p0, __p1) __cairo_transform((__p0), (__p1))

cairo_surface_t * __cairo_surface_reference(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1030(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_reference(__p0) __cairo_surface_reference((__p0))

void  __cairo_set_line_join(cairo_t *, cairo_line_join_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-172(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_line_join(__p0, __p1) __cairo_set_line_join((__p0), (__p1))

void  __cairo_stroke_extents(cairo_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-400(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_stroke_extents(__p0, __p1, __p2, __p3, __p4) __cairo_stroke_extents((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_status_t  __cairo_mesh_pattern_get_corner_color_rgba(cairo_pattern_t *, unsigned int , unsigned int , double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1894(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_get_corner_color_rgba(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_mesh_pattern_get_corner_color_rgba((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

void  __cairo_line_to(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-268(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_line_to(__p0, __p1, __p2) __cairo_line_to((__p0), (__p1), (__p2))

cairo_status_t  __cairo_status(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1012(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_status(__p0) __cairo_status((__p0))

cairo_status_t  __cairo_surface_set_user_data(cairo_surface_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1090(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_set_user_data(__p0, __p1, __p2, __p3) __cairo_surface_set_user_data((__p0), (__p1), (__p2), (__p3))

cairo_bool_t  __cairo_surface_supports_mime_type(cairo_surface_t *, const char *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1738(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_supports_mime_type(__p0, __p1) __cairo_surface_supports_mime_type((__p0), (__p1))

void  __cairo_new_sub_path(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-262(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_new_sub_path(__p0) __cairo_new_sub_path((__p0))

void  __cairo_show_page(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-382(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_show_page(__p0) __cairo_show_page((__p0))

void  __cairo_font_options_set_hint_metrics(cairo_font_options_t *, cairo_hint_metrics_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-550(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_set_hint_metrics(__p0, __p1) __cairo_font_options_set_hint_metrics((__p0), (__p1))

void  __cairo_ps_surface_dsc_begin_page_setup(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2140(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_dsc_begin_page_setup(__p0) __cairo_ps_surface_dsc_begin_page_setup((__p0))

unsigned int  __cairo_pattern_get_reference_count(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1264(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_reference_count(__p0) __cairo_pattern_get_reference_count((__p0))

cairo_status_t  __cairo_surface_observer_add_finish_callback(cairo_surface_t *, cairo_surface_observer_callback_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1660(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_add_finish_callback(__p0, __p1, __p2) __cairo_surface_observer_add_finish_callback((__p0), (__p1), (__p2))

void  __cairo_get_font_options(cairo_t *, cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-592(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_font_options(__p0, __p1) __cairo_get_font_options((__p0), (__p1))

cairo_filter_t  __cairo_pattern_get_filter(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1336(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_filter(__p0) __cairo_pattern_get_filter((__p0))

cairo_device_type_t  __cairo_device_get_type(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1534(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_get_type(__p0) __cairo_device_get_type((__p0))

cairo_device_t * __cairo_device_reference(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1528(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_reference(__p0) __cairo_device_reference((__p0))

double  __cairo_device_observer_glyphs_elapsed(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1714(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_glyphs_elapsed(__p0) __cairo_device_observer_glyphs_elapsed((__p0))

cairo_surface_t * __cairo_get_group_target(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-982(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_group_target(__p0) __cairo_get_group_target((__p0))

double  __cairo_surface_observer_elapsed(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1672(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_observer_elapsed(__p0) __cairo_surface_observer_elapsed((__p0))

cairo_surface_t * __cairo_surface_create_for_rectangle(cairo_surface_t *, double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1612(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_create_for_rectangle(__p0, __p1, __p2, __p3, __p4) __cairo_surface_create_for_rectangle((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_status_t  __cairo_font_options_status(cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-490(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_status(__p0) __cairo_font_options_status((__p0))

cairo_status_t  __cairo_surface_set_mime_data(cairo_surface_t *, const char *, const unsigned char *, unsigned long , cairo_destroy_func_t , void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1732(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_set_mime_data(__p0, __p1, __p2, __p3, __p4, __p5) __cairo_surface_set_mime_data((__p0), (__p1), (__p2), (__p3), (__p4), (__p5))

cairo_t * __cairo_reference(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-46(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_reference(__p0) __cairo_reference((__p0))

cairo_t * __cairo_create(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-40(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_create(__p0) __cairo_create((__p0))

int  __cairo_version() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-28(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_version() __cairo_version()

void  __cairo_pdf_get_versions(cairo_pdf_version_t const **, int *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2068(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pdf_get_versions(__p0, __p1) __cairo_pdf_get_versions((__p0), (__p1))

cairo_status_t  __cairo_scaled_font_status(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-736(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_status(__p0) __cairo_scaled_font_status((__p0))

const char * __cairo_toy_font_face_get_family(cairo_font_face_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-820(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_toy_font_face_get_family(__p0) __cairo_toy_font_face_get_family((__p0))

void  __cairo_user_font_face_set_init_func(cairo_font_face_t *, cairo_user_scaled_font_init_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-844(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_set_init_func(__p0, __p1) __cairo_user_font_face_set_init_func((__p0), (__p1))

void  __cairo_recording_surface_ink_extents(cairo_surface_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1750(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_recording_surface_ink_extents(__p0, __p1, __p2, __p3, __p4) __cairo_recording_surface_ink_extents((__p0), (__p1), (__p2), (__p3), (__p4))

void  __cairo_ps_surface_dsc_comment(cairo_surface_t *, const char *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2128(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_surface_dsc_comment(__p0, __p1) __cairo_ps_surface_dsc_comment((__p0), (__p1))

cairo_pattern_t * __cairo_pattern_create_radial(double , double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1246(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_create_radial(__p0, __p1, __p2, __p3, __p4, __p5) __cairo_pattern_create_radial((__p0), (__p1), (__p2), (__p3), (__p4), (__p5))

cairo_surface_type_t  __cairo_surface_get_type(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1060(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_type(__p0) __cairo_surface_get_type((__p0))

void  __cairo_stroke_preserve(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-358(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_stroke_preserve(__p0) __cairo_stroke_preserve((__p0))

cairo_bool_t  __cairo_recording_surface_get_extents(cairo_surface_t *, cairo_rectangle_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1756(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_recording_surface_get_extents(__p0, __p1) __cairo_recording_surface_get_extents((__p0), (__p1))

void  __cairo_device_release(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1552(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_release(__p0) __cairo_device_release((__p0))

void  __cairo_surface_get_font_options(cairo_surface_t *, cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1096(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_font_options(__p0, __p1) __cairo_surface_get_font_options((__p0), (__p1))

void  __cairo_ft_scaled_font_unlock_face(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1498(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ft_scaled_font_unlock_face(__p0) __cairo_ft_scaled_font_unlock_face((__p0))

void  __cairo_surface_destroy(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1042(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_destroy(__p0) __cairo_surface_destroy((__p0))

void  __cairo_set_fill_rule(cairo_t *, cairo_fill_rule_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-154(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_fill_rule(__p0, __p1) __cairo_set_fill_rule((__p0), (__p1))

void  __cairo_raster_source_pattern_set_callback_data(cairo_pattern_t *, void *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1768(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_set_callback_data(__p0, __p1) __cairo_raster_source_pattern_set_callback_data((__p0), (__p1))

cairo_status_t  __cairo_scaled_font_text_to_glyphs(cairo_scaled_font_t *, double , double , const char *, int , cairo_glyph_t **, int *, cairo_text_cluster_t **, int *, cairo_text_cluster_flags_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-778(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_text_to_glyphs(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9) __cairo_scaled_font_text_to_glyphs((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6), (__p7), (__p8), (__p9))

cairo_region_t * __cairo_region_copy(const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1924(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_copy(__p0) __cairo_region_copy((__p0))

void  __cairo_mesh_pattern_move_to(cairo_pattern_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1858(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mesh_pattern_move_to(__p0, __p1, __p2) __cairo_mesh_pattern_move_to((__p0), (__p1), (__p2))

void  __cairo_pdf_surface_restrict_to_version(cairo_surface_t *, cairo_pdf_version_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2062(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pdf_surface_restrict_to_version(__p0, __p1) __cairo_pdf_surface_restrict_to_version((__p0), (__p1))

void  __cairo_clip_extents(cairo_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-430(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_clip_extents(__p0, __p1, __p2, __p3, __p4) __cairo_clip_extents((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_status_t  __cairo_surface_status(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1054(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_status(__p0) __cairo_surface_status((__p0))

void  __cairo_paint_with_alpha(cairo_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-334(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_paint_with_alpha(__p0, __p1) __cairo_paint_with_alpha((__p0), (__p1))

void  __cairo_device_finish(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1564(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_finish(__p0) __cairo_device_finish((__p0))

void  __cairo_get_font_matrix(cairo_t *, cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-580(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_font_matrix(__p0, __p1) __cairo_get_font_matrix((__p0), (__p1))

void  __cairo_svg_get_versions(cairo_svg_version_t const **, int *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2164(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_svg_get_versions(__p0, __p1) __cairo_svg_get_versions((__p0), (__p1))

void  __cairo_arc(cairo_t *, double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-280(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_arc(__p0, __p1, __p2, __p3, __p4, __p5) __cairo_arc((__p0), (__p1), (__p2), (__p3), (__p4), (__p5))

cairo_status_t  __cairo_scaled_font_set_user_data(cairo_scaled_font_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-754(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_set_user_data(__p0, __p1, __p2, __p3) __cairo_scaled_font_set_user_data((__p0), (__p1), (__p2), (__p3))

void  __cairo_destroy(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-52(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_destroy(__p0) __cairo_destroy((__p0))

void  __cairo_matrix_transform_point(const cairo_matrix_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1444(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_transform_point(__p0, __p1, __p2) __cairo_matrix_transform_point((__p0), (__p1), (__p2))

void  __cairo_set_dash(cairo_t *, const double *, int , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-178(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_dash(__p0, __p1, __p2, __p3) __cairo_set_dash((__p0), (__p1), (__p2), (__p3))

cairo_status_t  __cairo_region_xor_rectangle(cairo_region_t *, const cairo_rectangle_int_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2038(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_xor_rectangle(__p0, __p1) __cairo_region_xor_rectangle((__p0), (__p1))

double  __cairo_device_observer_elapsed(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1684(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_elapsed(__p0) __cairo_device_observer_elapsed((__p0))

void  __cairo_set_tolerance(cairo_t *, double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-142(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_tolerance(__p0, __p1) __cairo_set_tolerance((__p0), (__p1))

void  __cairo_user_font_face_set_render_glyph_func(cairo_font_face_t *, cairo_user_scaled_font_render_glyph_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-850(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_font_face_set_render_glyph_func(__p0, __p1) __cairo_user_font_face_set_render_glyph_func((__p0), (__p1))

int  __cairo_region_num_rectangles(const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1960(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_num_rectangles(__p0) __cairo_region_num_rectangles((__p0))

const char * __cairo_version_string() =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-34(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_version_string() __cairo_version_string()

double  __cairo_device_observer_fill_elapsed(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1702(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_observer_fill_elapsed(__p0) __cairo_device_observer_fill_elapsed((__p0))

void * __cairo_pattern_get_user_data(cairo_pattern_t *, const cairo_user_data_key_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1276(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_user_data(__p0, __p1) __cairo_pattern_get_user_data((__p0), (__p1))

void  __cairo_rel_curve_to(cairo_t *, double , double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-304(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_rel_curve_to(__p0, __p1, __p2, __p3, __p4, __p5, __p6) __cairo_rel_curve_to((__p0), (__p1), (__p2), (__p3), (__p4), (__p5), (__p6))

cairo_bool_t  __cairo_in_clip(cairo_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1522(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_in_clip(__p0, __p1, __p2) __cairo_in_clip((__p0), (__p1), (__p2))

void  __cairo_arc_negative(cairo_t *, double , double , double , double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-286(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_arc_negative(__p0, __p1, __p2, __p3, __p4, __p5) __cairo_arc_negative((__p0), (__p1), (__p2), (__p3), (__p4), (__p5))

void * __cairo_font_face_get_user_data(cairo_font_face_t *, const cairo_user_data_key_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-700(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_face_get_user_data(__p0, __p1) __cairo_font_face_get_user_data((__p0), (__p1))

cairo_bool_t  __cairo_font_options_equal(const cairo_font_options_t *, const cairo_font_options_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-502(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_font_options_equal(__p0, __p1) __cairo_font_options_equal((__p0), (__p1))

cairo_bool_t  __cairo_surface_has_show_text_glyphs(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1156(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_has_show_text_glyphs(__p0) __cairo_surface_has_show_text_glyphs((__p0))

cairo_path_t * __cairo_copy_path(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-988(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_copy_path(__p0) __cairo_copy_path((__p0))

cairo_pattern_type_t  __cairo_pattern_get_type(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1288(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_type(__p0) __cairo_pattern_get_type((__p0))

void  __cairo_save(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-76(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_save(__p0) __cairo_save((__p0))

void  __cairo_push_group(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-88(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_push_group(__p0) __cairo_push_group((__p0))

cairo_status_t  __cairo_pattern_get_linear_points(cairo_pattern_t *, double *, double *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1366(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_get_linear_points(__p0, __p1, __p2, __p3, __p4) __cairo_pattern_get_linear_points((__p0), (__p1), (__p2), (__p3), (__p4))

cairo_scaled_font_t * __cairo_scaled_font_reference(cairo_scaled_font_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-718(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_scaled_font_reference(__p0) __cairo_scaled_font_reference((__p0))

void  __cairo_pattern_destroy(cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1258(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_pattern_destroy(__p0) __cairo_pattern_destroy((__p0))

int  __cairo_get_dash_count(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-958(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_dash_count(__p0) __cairo_get_dash_count((__p0))

cairo_bool_t  __cairo_region_equal(const cairo_region_t *, const cairo_region_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1942(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_region_equal(__p0, __p1) __cairo_region_equal((__p0), (__p1))

void  __cairo_matrix_multiply(cairo_matrix_t *, const cairo_matrix_t *, const cairo_matrix_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1432(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_matrix_multiply(__p0, __p1, __p2) __cairo_matrix_multiply((__p0), (__p1), (__p2))

void  __cairo_raster_source_pattern_set_copy(cairo_pattern_t *, cairo_raster_source_copy_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1804(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_set_copy(__p0, __p1) __cairo_raster_source_pattern_set_copy((__p0), (__p1))

void  __cairo_user_to_device(cairo_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-226(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_user_to_device(__p0, __p1, __p2) __cairo_user_to_device((__p0), (__p1), (__p2))

void  __cairo_set_source(cairo_t *, cairo_pattern_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-118(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_set_source(__p0, __p1) __cairo_set_source((__p0), (__p1))

unsigned int  __cairo_device_get_reference_count(cairo_device_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1576(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_device_get_reference_count(__p0) __cairo_device_get_reference_count((__p0))

void  __cairo_ps_get_levels(cairo_ps_level_t const **, int *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-2098(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_ps_get_levels(__p0, __p1) __cairo_ps_get_levels((__p0), (__p1))

void  __cairo_show_text(cairo_t *, const char *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-622(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_show_text(__p0, __p1) __cairo_show_text((__p0), (__p1))

void  __cairo_raster_source_pattern_set_snapshot(cairo_pattern_t *, cairo_raster_source_snapshot_func_t ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1792(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_raster_source_pattern_set_snapshot(__p0, __p1) __cairo_raster_source_pattern_set_snapshot((__p0), (__p1))

void  __cairo_surface_get_device_offset(cairo_surface_t *, double *, double *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1126(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_surface_get_device_offset(__p0, __p1, __p2) __cairo_surface_get_device_offset((__p0), (__p1), (__p2))

cairo_fill_rule_t  __cairo_get_fill_rule(cairo_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-928(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_get_fill_rule(__p0) __cairo_get_fill_rule((__p0))

int  __cairo_image_surface_get_height(cairo_surface_t *) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-1198(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_image_surface_get_height(__p0) __cairo_image_surface_get_height((__p0))

void  __cairo_mask_surface(cairo_t *, cairo_surface_t *, double , double ) =
	"\tlis\t11,CairoBase@ha\n"
	"\tlwz\t12,CairoBase@l(11)\n"
	"\tlwz\t0,-346(12)\n"
	"\tmtlr\t0\n"
	"\tblrl";
#define cairo_mask_surface(__p0, __p1, __p2, __p3) __cairo_mask_surface((__p0), (__p1), (__p2), (__p3))

#endif /* !_VBCCINLINE_CAIRO_H */
