/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_CAIRO_H
#define _PPCINLINE_CAIRO_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef CAIRO_BASE_NAME
#define CAIRO_BASE_NAME CairoBase
#endif /* !CAIRO_BASE_NAME */

#define cairo_surface_write_to_png(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, const char *))*(void**)(__base - 1072))(__t__p0, __t__p1));\
	})

#define cairo_rel_move_to(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double ))*(void**)(__base - 292))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_font_options_set_subpixel_order(__p0, __p1) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		cairo_subpixel_order_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_options_t *, cairo_subpixel_order_t ))*(void**)(__base - 526))(__t__p0, __t__p1));\
	})

#define cairo_get_scaled_font(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_scaled_font_t *(*)(cairo_t *))*(void**)(__base - 616))(__t__p0));\
	})

#define cairo_user_font_face_get_init_func(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_user_scaled_font_init_func_t (*)(cairo_font_face_t *))*(void**)(__base - 868))(__t__p0));\
	})

#define cairo_image_surface_get_stride(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(cairo_surface_t *))*(void**)(__base - 1204))(__t__p0));\
	})

#define cairo_select_font_face(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		cairo_font_slant_t  __t__p2 = __p2;\
		cairo_font_weight_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const char *, cairo_font_slant_t , cairo_font_weight_t ))*(void**)(__base - 562))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_toy_font_face_get_weight(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_weight_t (*)(cairo_font_face_t *))*(void**)(__base - 832))(__t__p0));\
	})

#define cairo_get_dash(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *))*(void**)(__base - 964))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_create_observer(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_mode_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_surface_t *, cairo_surface_observer_mode_t ))*(void**)(__base - 1618))(__t__p0, __t__p1));\
	})

#define cairo_move_to(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double ))*(void**)(__base - 256))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_scaled_font_destroy(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *))*(void**)(__base - 724))(__t__p0));\
	})

#define cairo_rectangle_list_destroy(__p0) \
	({ \
		cairo_rectangle_list_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_rectangle_list_t *))*(void**)(__base - 442))(__t__p0));\
	})

#define cairo_text_path(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const char *))*(void**)(__base - 640))(__t__p0, __t__p1));\
	})

#define cairo_pattern_set_matrix(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		const cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, const cairo_matrix_t *))*(void**)(__base - 1306))(__t__p0, __t__p1));\
	})

#define cairo_device_to_user(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *))*(void**)(__base - 238))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_scaled_font_create(__p0, __p1, __p2, __p3) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		const cairo_matrix_t * __t__p1 = __p1;\
		const cairo_matrix_t * __t__p2 = __p2;\
		const cairo_font_options_t * __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_scaled_font_t *(*)(cairo_font_face_t *, const cairo_matrix_t *, const cairo_matrix_t *, const cairo_font_options_t *))*(void**)(__base - 712))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_ps_surface_get_eps(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_surface_t *))*(void**)(__base - 2116))(__t__p0));\
	})

#define cairo_ft_font_options_substitute(__p0, __p1) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		FcPattern * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(const cairo_font_options_t *, FcPattern *))*(void**)(__base - 1480))(__t__p0, __t__p1));\
	})

#define cairo_get_target(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_t *))*(void**)(__base - 976))(__t__p0));\
	})

#define cairo_font_options_get_hint_style(__p0) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_hint_style_t (*)(const cairo_font_options_t *))*(void**)(__base - 544))(__t__p0));\
	})

#define cairo_scaled_font_get_user_data(__p0, __p1) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_scaled_font_t *, const cairo_user_data_key_t *))*(void**)(__base - 748))(__t__p0, __t__p1));\
	})

#define cairo_toy_font_face_create(__p0, __p1, __p2) \
	({ \
		const char * __t__p0 = __p0;\
		cairo_font_slant_t  __t__p1 = __p1;\
		cairo_font_weight_t  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(const char *, cairo_font_slant_t , cairo_font_weight_t ))*(void**)(__base - 814))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_region_create_rectangles(__p0, __p1) \
	({ \
		const cairo_rectangle_int_t * __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_region_t *(*)(const cairo_rectangle_int_t *, int ))*(void**)(__base - 1918))(__t__p0, __t__p1));\
	})

#define cairo_path_destroy(__p0) \
	({ \
		cairo_path_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_path_t *))*(void**)(__base - 1006))(__t__p0));\
	})

#define cairo_set_matrix(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_matrix_t *))*(void**)(__base - 214))(__t__p0, __t__p1));\
	})

#define cairo_surface_observer_add_fill_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1636))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_mesh_pattern_curve_to(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		double  __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, double , double , double , double , double , double ))*(void**)(__base - 1846))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_pattern_set_extend(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_extend_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_extend_t ))*(void**)(__base - 1318))(__t__p0, __t__p1));\
	})

#define cairo_pop_group_to_source(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 106))(__t__p0));\
	})

#define cairo_region_union(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_region_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_region_t *))*(void**)(__base - 2020))(__t__p0, __t__p1));\
	})

#define cairo_glyph_allocate(__p0) \
	({ \
		int  __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_glyph_t *(*)(int ))*(void**)(__base - 448))(__t__p0));\
	})

#define cairo_matrix_rotate(__p0, __p1) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double ))*(void**)(__base - 1420))(__t__p0, __t__p1));\
	})

#define cairo_surface_observer_add_paint_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1624))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_observer_add_stroke_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1642))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_get_reference_count(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_surface_t *))*(void**)(__base - 1048))(__t__p0));\
	})

#define cairo_svg_surface_restrict_to_version(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_svg_version_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, cairo_svg_version_t ))*(void**)(__base - 2158))(__t__p0, __t__p1));\
	})

#define cairo_clip_preserve(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 424))(__t__p0));\
	})

#define cairo_image_surface_create_for_data(__p0, __p1, __p2, __p3, __p4) \
	({ \
		unsigned char * __t__p0 = __p0;\
		cairo_format_t  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		int  __t__p3 = __p3;\
		int  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(unsigned char *, cairo_format_t , int , int , int ))*(void**)(__base - 1174))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_show_glyphs(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_glyph_t * __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_glyph_t *, int ))*(void**)(__base - 628))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_recording_surface_create(__p0, __p1) \
	({ \
		cairo_content_t  __t__p0 = __p0;\
		const cairo_rectangle_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_content_t , const cairo_rectangle_t *))*(void**)(__base - 1744))(__t__p0, __t__p1));\
	})

#define cairo_image_surface_get_data(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned char *(*)(cairo_surface_t *))*(void**)(__base - 1180))(__t__p0));\
	})

#define cairo_glyph_extents(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_glyph_t * __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		cairo_text_extents_t * __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_glyph_t *, int , cairo_text_extents_t *))*(void**)(__base - 658))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_font_face_status(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_font_face_t *))*(void**)(__base - 688))(__t__p0));\
	})

#define cairo_text_cluster_free(__p0) \
	({ \
		cairo_text_cluster_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_text_cluster_t *))*(void**)(__base - 466))(__t__p0));\
	})

#define cairo_ft_font_face_create_for_pattern(__p0) \
	({ \
		FcPattern * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(FcPattern *))*(void**)(__base - 1474))(__t__p0));\
	})

#define cairo_show_text_glyphs(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		const cairo_glyph_t * __t__p3 = __p3;\
		int  __t__p4 = __p4;\
		const cairo_text_cluster_t * __t__p5 = __p5;\
		int  __t__p6 = __p6;\
		cairo_text_cluster_flags_t  __t__p7 = __p7;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const char *, int , const cairo_glyph_t *, int , const cairo_text_cluster_t *, int , cairo_text_cluster_flags_t ))*(void**)(__base - 634))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6, __t__p7));\
	})

#define cairo_pattern_create_linear(__p0, __p1, __p2, __p3) \
	({ \
		double  __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(double , double , double , double ))*(void**)(__base - 1240))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_raster_source_pattern_get_finish(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_raster_source_finish_func_t (*)(cairo_pattern_t *))*(void**)(__base - 1822))(__t__p0));\
	})

#define cairo_font_options_create() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_options_t *(*)(void))*(void**)(__base - 472))());\
	})

#define cairo_scaled_font_get_reference_count(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_scaled_font_t *))*(void**)(__base - 730))(__t__p0));\
	})

#define cairo_surface_set_fallback_resolution(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double , double ))*(void**)(__base - 1132))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_has_current_point(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_t *))*(void**)(__base - 916))(__t__p0));\
	})

#define cairo_scaled_font_get_type(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_type_t (*)(cairo_scaled_font_t *))*(void**)(__base - 742))(__t__p0));\
	})

#define cairo_set_source_surface(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_surface_t * __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_surface_t *, double , double ))*(void**)(__base - 136))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_get_line_width(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_t *))*(void**)(__base - 934))(__t__p0));\
	})

#define cairo_mesh_pattern_begin_patch(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *))*(void**)(__base - 1834))(__t__p0));\
	})

#define cairo_surface_create_similar_image(__p0, __p1, __p2, __p3) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_format_t  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		int  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_surface_t *, cairo_format_t , int , int ))*(void**)(__base - 1594))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_mesh_pattern_set_corner_color_rgba(__p0, __p1, __p2, __p3, __p4, __p5) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, unsigned int , double , double , double , double ))*(void**)(__base - 1876))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5));\
	})

#define cairo_raster_source_pattern_set_acquire(__p0, __p1, __p2) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_raster_source_acquire_func_t  __t__p1 = __p1;\
		cairo_raster_source_release_func_t  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_raster_source_acquire_func_t , cairo_raster_source_release_func_t ))*(void**)(__base - 1780))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_font_extents(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_font_extents_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_font_extents_t *))*(void**)(__base - 664))(__t__p0, __t__p1));\
	})

#define cairo_get_miter_limit(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_t *))*(void**)(__base - 952))(__t__p0));\
	})

#define cairo_region_contains_point(__p0, __p1, __p2) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(const cairo_region_t *, int , int ))*(void**)(__base - 1984))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_matrix_init_scale(__p0, __p1, __p2) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double , double ))*(void**)(__base - 1396))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_glyph_free(__p0) \
	({ \
		cairo_glyph_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_glyph_t *))*(void**)(__base - 454))(__t__p0));\
	})

#define cairo_matrix_scale(__p0, __p1, __p2) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double , double ))*(void**)(__base - 1414))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_write_to_png_stream(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_write_func_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_write_func_t , void *))*(void**)(__base - 1078))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_get_user_data(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_surface_t *, const cairo_user_data_key_t *))*(void**)(__base - 1084))(__t__p0, __t__p1));\
	})

#define cairo_ft_font_face_unset_synthesize(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *, unsigned int ))*(void**)(__base - 2050))(__t__p0, __t__p1));\
	})

#define cairo_font_options_get_hint_metrics(__p0) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_hint_metrics_t (*)(const cairo_font_options_t *))*(void**)(__base - 556))(__t__p0));\
	})

#define cairo_pattern_add_color_stop_rgb(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, double , double , double , double ))*(void**)(__base - 1294))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_new_path(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 250))(__t__p0));\
	})

#define cairo_set_font_options(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_font_options_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_font_options_t *))*(void**)(__base - 586))(__t__p0, __t__p1));\
	})

#define cairo_device_observer_mask_elapsed(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_device_t *))*(void**)(__base - 1696))(__t__p0));\
	})

#define cairo_region_translate(__p0, __p1, __p2) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_region_t *, int , int ))*(void**)(__base - 1990))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_observer_add_mask_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1630))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_set_font_matrix(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_matrix_t *))*(void**)(__base - 574))(__t__p0, __t__p1));\
	})

#define cairo_glyph_path(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_glyph_t * __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_glyph_t *, int ))*(void**)(__base - 646))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_set_scaled_font(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_scaled_font_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_scaled_font_t *))*(void**)(__base - 610))(__t__p0, __t__p1));\
	})

#define cairo_user_font_face_create() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(void))*(void**)(__base - 838))());\
	})

#define cairo_stroke(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 352))(__t__p0));\
	})

#define cairo_font_face_set_user_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		cairo_destroy_func_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_font_face_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ))*(void**)(__base - 706))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_fill_preserve(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 370))(__t__p0));\
	})

#define cairo_set_antialias(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_antialias_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_antialias_t ))*(void**)(__base - 148))(__t__p0, __t__p1));\
	})

#define cairo_mesh_pattern_line_to(__p0, __p1, __p2) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, double , double ))*(void**)(__base - 1852))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_path_extents(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *, double *, double *))*(void**)(__base - 322))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_get_antialias(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_antialias_t (*)(cairo_t *))*(void**)(__base - 910))(__t__p0));\
	})

#define cairo_raster_source_pattern_set_finish(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_raster_source_finish_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_raster_source_finish_func_t ))*(void**)(__base - 1816))(__t__p0, __t__p1));\
	})

#define cairo_get_font_face(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(cairo_t *))*(void**)(__base - 604))(__t__p0));\
	})

#define cairo_pdf_surface_create_for_stream(__p0, __p1, __p2, __p3) \
	({ \
		cairo_write_func_t  __t__p0 = __p0;\
		void * __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_write_func_t , void *, double , double ))*(void**)(__base - 1462))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_scaled_font_glyph_extents(__p0, __p1, __p2, __p3) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		const cairo_glyph_t * __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		cairo_text_extents_t * __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, const cairo_glyph_t *, int , cairo_text_extents_t *))*(void**)(__base - 772))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_device_destroy(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_device_t *))*(void**)(__base - 1570))(__t__p0));\
	})

#define cairo_region_get_extents(__p0, __p1) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(const cairo_region_t *, cairo_rectangle_int_t *))*(void**)(__base - 1954))(__t__p0, __t__p1));\
	})

#define cairo_set_font_size(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double ))*(void**)(__base - 568))(__t__p0, __t__p1));\
	})

#define cairo_fill(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 364))(__t__p0));\
	})

#define cairo_font_options_get_antialias(__p0) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_antialias_t (*)(const cairo_font_options_t *))*(void**)(__base - 520))(__t__p0));\
	})

#define cairo_surface_get_device(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_device_t *(*)(cairo_surface_t *))*(void**)(__base - 1720))(__t__p0));\
	})

#define cairo_copy_path_flat(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_path_t *(*)(cairo_t *))*(void**)(__base - 994))(__t__p0));\
	})

#define cairo_toy_font_face_get_slant(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_slant_t (*)(cairo_font_face_t *))*(void**)(__base - 826))(__t__p0));\
	})

#define cairo_device_set_user_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		cairo_destroy_func_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_device_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ))*(void**)(__base - 1588))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_user_to_device_distance(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *))*(void**)(__base - 232))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_morphos_surface_get_bitmap(__p0) \
	({ \
		void * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((struct BitMap *(*)(void *))*(void**)(__base - 1510))(__t__p0));\
	})

#define cairo_device_status(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_device_t *))*(void**)(__base - 1540))(__t__p0));\
	})

#define cairo_surface_finish(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 1036))(__t__p0));\
	})

#define cairo_region_is_empty(__p0) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(const cairo_region_t *))*(void**)(__base - 1972))(__t__p0));\
	})

#define cairo_pattern_get_color_stop_count(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		int * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, int *))*(void**)(__base - 1360))(__t__p0, __t__p1));\
	})

#define cairo_surface_show_page(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 1150))(__t__p0));\
	})

#define cairo_set_source_rgba(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double , double ))*(void**)(__base - 130))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_region_contains_rectangle(__p0, __p1) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		const cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_region_overlap_t (*)(const cairo_region_t *, const cairo_rectangle_int_t *))*(void**)(__base - 1978))(__t__p0, __t__p1));\
	})

#define cairo_surface_mark_dirty(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 1108))(__t__p0));\
	})

#define cairo_ps_surface_create(__p0, __p1, __p2) \
	({ \
		const char * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(const char *, double , double ))*(void**)(__base - 2080))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_matrix_invert(__p0) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_matrix_t *))*(void**)(__base - 1426))(__t__p0));\
	})

#define cairo_get_matrix(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_matrix_t *))*(void**)(__base - 970))(__t__p0, __t__p1));\
	})

#define cairo_pattern_create_mesh() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(void))*(void**)(__base - 1828))());\
	})

#define cairo_pattern_set_user_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		cairo_destroy_func_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ))*(void**)(__base - 1282))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_matrix_translate(__p0, __p1, __p2) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double , double ))*(void**)(__base - 1408))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_device_get_user_data(__p0, __p1) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_device_t *, const cairo_user_data_key_t *))*(void**)(__base - 1582))(__t__p0, __t__p1));\
	})

#define cairo_font_options_hash(__p0) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned long (*)(const cairo_font_options_t *))*(void**)(__base - 508))(__t__p0));\
	})

#define cairo_pattern_create_rgb(__p0, __p1, __p2) \
	({ \
		double  __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(double , double , double ))*(void**)(__base - 1222))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_status_to_string(__p0) \
	({ \
		cairo_status_t  __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((const char *(*)(cairo_status_t ))*(void**)(__base - 1018))(__t__p0));\
	})

#define cairo_region_intersect(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_region_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_region_t *))*(void**)(__base - 2008))(__t__p0, __t__p1));\
	})

#define cairo_debug_reset_static_data() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(void))*(void**)(__base - 1450))());\
	})

#define cairo_identity_matrix(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 220))(__t__p0));\
	})

#define cairo_device_flush(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_device_t *))*(void**)(__base - 1558))(__t__p0));\
	})

#define cairo_mesh_pattern_get_patch_count(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, unsigned int *))*(void**)(__base - 1882))(__t__p0, __t__p1));\
	})

#define cairo_svg_surface_create(__p0, __p1, __p2) \
	({ \
		const char * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(const char *, double , double ))*(void**)(__base - 2146))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_set_source_rgb(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double ))*(void**)(__base - 124))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_svg_version_to_string(__p0) \
	({ \
		cairo_svg_version_t  __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((const char *(*)(cairo_svg_version_t ))*(void**)(__base - 2170))(__t__p0));\
	})

#define cairo_mesh_pattern_set_control_point(__p0, __p1, __p2, __p3) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, unsigned int , double , double ))*(void**)(__base - 1864))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_rotate(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double ))*(void**)(__base - 202))(__t__p0, __t__p1));\
	})

#define cairo_surface_observer_print(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_write_func_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_write_func_t , void *))*(void**)(__base - 1666))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_copy_clip_rectangle_list(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_rectangle_list_t *(*)(cairo_t *))*(void**)(__base - 436))(__t__p0));\
	})

#define cairo_clip(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 418))(__t__p0));\
	})

#define cairo_surface_flush(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 1102))(__t__p0));\
	})

#define cairo_surface_observer_add_flush_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1654))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_region_union_rectangle(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_rectangle_int_t *))*(void**)(__base - 2026))(__t__p0, __t__p1));\
	})

#define cairo_font_options_copy(__p0) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_options_t *(*)(const cairo_font_options_t *))*(void**)(__base - 478))(__t__p0));\
	})

#define cairo_close_path(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 316))(__t__p0));\
	})

#define cairo_user_font_face_get_render_glyph_func(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_user_scaled_font_render_glyph_func_t (*)(cairo_font_face_t *))*(void**)(__base - 874))(__t__p0));\
	})

#define cairo_pattern_get_extend(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_extend_t (*)(cairo_pattern_t *))*(void**)(__base - 1324))(__t__p0));\
	})

#define cairo_raster_source_pattern_get_copy(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_raster_source_copy_func_t (*)(cairo_pattern_t *))*(void**)(__base - 1810))(__t__p0));\
	})

#define cairo_pdf_version_to_string(__p0) \
	({ \
		cairo_pdf_version_t  __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((const char *(*)(cairo_pdf_version_t ))*(void**)(__base - 2074))(__t__p0));\
	})

#define cairo_push_group_with_content(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_content_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_content_t ))*(void**)(__base - 94))(__t__p0, __t__p1));\
	})

#define cairo_device_observer_paint_elapsed(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_device_t *))*(void**)(__base - 1690))(__t__p0));\
	})

#define cairo_restore(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 82))(__t__p0));\
	})

#define cairo_get_reference_count(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_t *))*(void**)(__base - 58))(__t__p0));\
	})

#define cairo_translate(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double ))*(void**)(__base - 190))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_ps_surface_set_size(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double , double ))*(void**)(__base - 2122))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_font_face_get_reference_count(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_font_face_t *))*(void**)(__base - 682))(__t__p0));\
	})

#define cairo_surface_copy_page(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 1144))(__t__p0));\
	})

#define cairo_set_font_face(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_font_face_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_font_face_t *))*(void**)(__base - 598))(__t__p0, __t__p1));\
	})

#define cairo_mesh_pattern_get_control_point(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		unsigned int  __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, unsigned int , unsigned int , double *, double *))*(void**)(__base - 1900))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_ps_level_to_string(__p0) \
	({ \
		cairo_ps_level_t  __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((const char *(*)(cairo_ps_level_t ))*(void**)(__base - 2104))(__t__p0));\
	})

#define cairo_pattern_create_for_surface(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(cairo_surface_t *))*(void**)(__base - 1234))(__t__p0));\
	})

#define cairo_matrix_transform_distance(__p0, __p1, __p2) \
	({ \
		const cairo_matrix_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(const cairo_matrix_t *, double *, double *))*(void**)(__base - 1438))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_font_face_reference(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(cairo_font_face_t *))*(void**)(__base - 670))(__t__p0));\
	})

#define cairo_pop_group(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(cairo_t *))*(void**)(__base - 100))(__t__p0));\
	})

#define cairo_ps_surface_create_for_stream(__p0, __p1, __p2, __p3) \
	({ \
		cairo_write_func_t  __t__p0 = __p0;\
		void * __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_write_func_t , void *, double , double ))*(void**)(__base - 2086))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_text_extents(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		cairo_text_extents_t * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const char *, cairo_text_extents_t *))*(void**)(__base - 652))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_morphos_surface_create_from_bitmap(__p0, __p1, __p2, __p3) \
	({ \
		cairo_content_t  __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		struct BitMap * __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_content_t , int , int , struct BitMap *))*(void**)(__base - 1504))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_surface_observer_add_glyphs_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1648))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_region_reference(__p0) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_region_t *(*)(cairo_region_t *))*(void**)(__base - 1930))(__t__p0));\
	})

#define cairo_font_face_destroy(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *))*(void**)(__base - 676))(__t__p0));\
	})

#define cairo_pattern_create_raster_source(__p0, __p1, __p2, __p3) \
	({ \
		void * __t__p0 = __p0;\
		cairo_content_t  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		int  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(void *, cairo_content_t , int , int ))*(void**)(__base - 1762))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_mesh_pattern_end_patch(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *))*(void**)(__base - 1840))(__t__p0));\
	})

#define cairo_copy_page(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 376))(__t__p0));\
	})

#define cairo_region_status(__p0) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(const cairo_region_t *))*(void**)(__base - 1948))(__t__p0));\
	})

#define cairo_raster_source_pattern_get_acquire(__p0, __p1, __p2) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_raster_source_acquire_func_t * __t__p1 = __p1;\
		cairo_raster_source_release_func_t * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_raster_source_acquire_func_t *, cairo_raster_source_release_func_t *))*(void**)(__base - 1786))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_pattern_get_matrix(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_matrix_t *))*(void**)(__base - 1312))(__t__p0, __t__p1));\
	})

#define cairo_region_destroy(__p0) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_region_t *))*(void**)(__base - 1936))(__t__p0));\
	})

#define cairo_set_line_width(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double ))*(void**)(__base - 160))(__t__p0, __t__p1));\
	})

#define cairo_get_current_point(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *))*(void**)(__base - 922))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_pattern_get_surface(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_surface_t ** __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, cairo_surface_t **))*(void**)(__base - 1348))(__t__p0, __t__p1));\
	})

#define cairo_surface_get_mime_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		const unsigned char ** __t__p2 = __p2;\
		unsigned long * __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, const char *, const unsigned char **, unsigned long *))*(void**)(__base - 1726))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_ft_font_face_get_synthesize(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_font_face_t *))*(void**)(__base - 2056))(__t__p0));\
	})

#define cairo_image_surface_create_from_png(__p0) \
	({ \
		const char * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(const char *))*(void**)(__base - 1210))(__t__p0));\
	})

#define cairo_scaled_font_extents(__p0, __p1) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		cairo_font_extents_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, cairo_font_extents_t *))*(void**)(__base - 760))(__t__p0, __t__p1));\
	})

#define cairo_scaled_font_get_font_face(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(cairo_scaled_font_t *))*(void**)(__base - 784))(__t__p0));\
	})

#define cairo_scaled_font_get_font_matrix(__p0, __p1) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, cairo_matrix_t *))*(void**)(__base - 790))(__t__p0, __t__p1));\
	})

#define cairo_reset_clip(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 412))(__t__p0));\
	})

#define cairo_mesh_pattern_get_path(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_path_t *(*)(cairo_pattern_t *, unsigned int ))*(void**)(__base - 1888))(__t__p0, __t__p1));\
	})

#define cairo_mesh_pattern_set_corner_color_rgb(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, unsigned int , double , double , double ))*(void**)(__base - 1870))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_pattern_add_color_stop_rgba(__p0, __p1, __p2, __p3, __p4, __p5) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, double , double , double , double , double ))*(void**)(__base - 1300))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5));\
	})

#define cairo_pattern_status(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *))*(void**)(__base - 1270))(__t__p0));\
	})

#define cairo_svg_surface_create_for_stream(__p0, __p1, __p2, __p3) \
	({ \
		cairo_write_func_t  __t__p0 = __p0;\
		void * __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_write_func_t , void *, double , double ))*(void**)(__base - 2152))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_region_create_rectangle(__p0) \
	({ \
		const cairo_rectangle_int_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_region_t *(*)(const cairo_rectangle_int_t *))*(void**)(__base - 1912))(__t__p0));\
	})

#define cairo_region_create() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_region_t *(*)(void))*(void**)(__base - 1906))());\
	})

#define cairo_fill_extents(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *, double *, double *))*(void**)(__base - 406))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_ft_scaled_font_lock_face(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((FT_Face (*)(cairo_scaled_font_t *))*(void**)(__base - 1492))(__t__p0));\
	})

#define cairo_ft_font_face_create_for_ft_face(__p0, __p1) \
	({ \
		FT_Face  __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_face_t *(*)(FT_Face , int ))*(void**)(__base - 1486))(__t__p0, __t__p1));\
	})

#define cairo_ps_surface_restrict_to_level(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_ps_level_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, cairo_ps_level_t ))*(void**)(__base - 2092))(__t__p0, __t__p1));\
	})

#define cairo_region_xor(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_region_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_region_t *))*(void**)(__base - 2032))(__t__p0, __t__p1));\
	})

#define cairo_pdf_surface_create(__p0, __p1, __p2) \
	({ \
		const char * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(const char *, double , double ))*(void**)(__base - 1456))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_create_similar(__p0, __p1, __p2, __p3) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_content_t  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		int  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_surface_t *, cairo_content_t , int , int ))*(void**)(__base - 1024))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_font_face_get_type(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_font_type_t (*)(cairo_font_face_t *))*(void**)(__base - 694))(__t__p0));\
	})

#define cairo_image_surface_get_width(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(cairo_surface_t *))*(void**)(__base - 1192))(__t__p0));\
	})

#define cairo_in_stroke(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_t *, double , double ))*(void**)(__base - 388))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_matrix_init_rotate(__p0, __p1) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double ))*(void**)(__base - 1402))(__t__p0, __t__p1));\
	})

#define cairo_region_intersect_rectangle(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_rectangle_int_t *))*(void**)(__base - 2014))(__t__p0, __t__p1));\
	})

#define cairo_font_options_merge(__p0, __p1) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		const cairo_font_options_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_options_t *, const cairo_font_options_t *))*(void**)(__base - 496))(__t__p0, __t__p1));\
	})

#define cairo_device_acquire(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_device_t *))*(void**)(__base - 1546))(__t__p0));\
	})

#define cairo_in_fill(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_t *, double , double ))*(void**)(__base - 394))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_device_observer_print(__p0, __p1, __p2) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		cairo_write_func_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_device_t *, cairo_write_func_t , void *))*(void**)(__base - 1678))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_matrix_init_translate(__p0, __p1, __p2) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double , double ))*(void**)(__base - 1390))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_surface_get_content(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_content_t (*)(cairo_surface_t *))*(void**)(__base - 1066))(__t__p0));\
	})

#define cairo_surface_set_device_offset(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double , double ))*(void**)(__base - 1120))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_ft_font_face_set_synthesize(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *, unsigned int ))*(void**)(__base - 2044))(__t__p0, __t__p1));\
	})

#define cairo_scale(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double ))*(void**)(__base - 196))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_pattern_get_rgba(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, double *, double *, double *, double *))*(void**)(__base - 1342))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_mask(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_pattern_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_pattern_t *))*(void**)(__base - 340))(__t__p0, __t__p1));\
	})

#define cairo_matrix_init_identity(__p0) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *))*(void**)(__base - 1384))(__t__p0));\
	})

#define cairo_pattern_reference(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(cairo_pattern_t *))*(void**)(__base - 1252))(__t__p0));\
	})

#define cairo_raster_source_pattern_get_snapshot(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_raster_source_snapshot_func_t (*)(cairo_pattern_t *))*(void**)(__base - 1798))(__t__p0));\
	})

#define cairo_set_operator(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_operator_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_operator_t ))*(void**)(__base - 112))(__t__p0, __t__p1));\
	})

#define cairo_surface_map_to_image(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_surface_t *, const cairo_rectangle_int_t *))*(void**)(__base - 1600))(__t__p0, __t__p1));\
	})

#define cairo_pattern_get_color_stop_rgba(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		double * __t__p5 = __p5;\
		double * __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, int , double *, double *, double *, double *, double *))*(void**)(__base - 1354))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_ps_surface_set_eps(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_bool_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, cairo_bool_t ))*(void**)(__base - 2110))(__t__p0, __t__p1));\
	})

#define cairo_morphos_surface_create_for_rastport(__p0, __p1) \
	({ \
		cairo_content_t  __t__p0 = __p0;\
		struct RastPort * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_content_t , struct RastPort *))*(void**)(__base - 1516))(__t__p0, __t__p1));\
	})

#define cairo_user_font_face_set_text_to_glyphs_func(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		cairo_user_scaled_font_text_to_glyphs_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *, cairo_user_scaled_font_text_to_glyphs_func_t ))*(void**)(__base - 856))(__t__p0, __t__p1));\
	})

#define cairo_region_subtract_rectangle(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_rectangle_int_t *))*(void**)(__base - 2002))(__t__p0, __t__p1));\
	})

#define cairo_scaled_font_get_scale_matrix(__p0, __p1) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, cairo_matrix_t *))*(void**)(__base - 802))(__t__p0, __t__p1));\
	})

#define cairo_scaled_font_get_font_options(__p0, __p1) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		cairo_font_options_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, cairo_font_options_t *))*(void**)(__base - 808))(__t__p0, __t__p1));\
	})

#define cairo_get_source(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(cairo_t *))*(void**)(__base - 898))(__t__p0));\
	})

#define cairo_pattern_create_rgba(__p0, __p1, __p2, __p3) \
	({ \
		double  __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(double , double , double , double ))*(void**)(__base - 1228))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_text_cluster_allocate(__p0) \
	({ \
		int  __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_text_cluster_t *(*)(int ))*(void**)(__base - 460))(__t__p0));\
	})

#define cairo_surface_get_fallback_resolution(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double *, double *))*(void**)(__base - 1138))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_get_user_data(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_t *, const cairo_user_data_key_t *))*(void**)(__base - 64))(__t__p0, __t__p1));\
	})

#define cairo_surface_unmap_image(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, cairo_surface_t *))*(void**)(__base - 1606))(__t__p0, __t__p1));\
	})

#define cairo_user_font_face_get_unicode_to_glyph_func(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_user_scaled_font_unicode_to_glyph_func_t (*)(cairo_font_face_t *))*(void**)(__base - 886))(__t__p0));\
	})

#define cairo_region_get_rectangle(__p0, __p1, __p2) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		cairo_rectangle_int_t * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(const cairo_region_t *, int , cairo_rectangle_int_t *))*(void**)(__base - 1966))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_image_surface_create(__p0, __p1, __p2) \
	({ \
		cairo_format_t  __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_format_t , int , int ))*(void**)(__base - 1162))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_device_to_user_distance(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *))*(void**)(__base - 244))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_font_options_set_hint_style(__p0, __p1) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		cairo_hint_style_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_options_t *, cairo_hint_style_t ))*(void**)(__base - 538))(__t__p0, __t__p1));\
	})

#define cairo_pattern_set_filter(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_filter_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_filter_t ))*(void**)(__base - 1330))(__t__p0, __t__p1));\
	})

#define cairo_pdf_surface_set_size(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double , double ))*(void**)(__base - 1468))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_paint(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 328))(__t__p0));\
	})

#define cairo_set_user_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		cairo_destroy_func_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ))*(void**)(__base - 70))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_raster_source_pattern_get_callback_data(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_pattern_t *))*(void**)(__base - 1774))(__t__p0));\
	})

#define cairo_rectangle(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double , double ))*(void**)(__base - 310))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_get_line_join(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_line_join_t (*)(cairo_t *))*(void**)(__base - 946))(__t__p0));\
	})

#define cairo_append_path(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_path_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_path_t *))*(void**)(__base - 1000))(__t__p0, __t__p1));\
	})

#define cairo_matrix_init(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		double  __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, double , double , double , double , double , double ))*(void**)(__base - 1378))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_user_font_face_set_unicode_to_glyph_func(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		cairo_user_scaled_font_unicode_to_glyph_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *, cairo_user_scaled_font_unicode_to_glyph_func_t ))*(void**)(__base - 862))(__t__p0, __t__p1));\
	})

#define cairo_user_font_face_get_text_to_glyphs_func(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_user_scaled_font_text_to_glyphs_func_t (*)(cairo_font_face_t *))*(void**)(__base - 880))(__t__p0));\
	})

#define cairo_ps_surface_dsc_begin_setup(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 2134))(__t__p0));\
	})

#define cairo_font_options_destroy(__p0) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_options_t *))*(void**)(__base - 484))(__t__p0));\
	})

#define cairo_surface_mark_dirty_rectangle(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		int  __t__p3 = __p3;\
		int  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, int , int , int , int ))*(void**)(__base - 1114))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_scaled_font_get_ctm(__p0, __p1) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, cairo_matrix_t *))*(void**)(__base - 796))(__t__p0, __t__p1));\
	})

#define cairo_region_subtract(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_region_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_region_t *))*(void**)(__base - 1996))(__t__p0, __t__p1));\
	})

#define cairo_device_observer_stroke_elapsed(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_device_t *))*(void**)(__base - 1708))(__t__p0));\
	})

#define cairo_font_options_get_subpixel_order(__p0) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_subpixel_order_t (*)(const cairo_font_options_t *))*(void**)(__base - 532))(__t__p0));\
	})

#define cairo_scaled_font_text_extents(__p0, __p1, __p2) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		cairo_text_extents_t * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *, const char *, cairo_text_extents_t *))*(void**)(__base - 766))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_curve_to(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		double  __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double , double , double , double ))*(void**)(__base - 274))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_set_line_cap(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_line_cap_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_line_cap_t ))*(void**)(__base - 166))(__t__p0, __t__p1));\
	})

#define cairo_font_options_set_antialias(__p0, __p1) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		cairo_antialias_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_options_t *, cairo_antialias_t ))*(void**)(__base - 514))(__t__p0, __t__p1));\
	})

#define cairo_pattern_get_radial_circles(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		double * __t__p5 = __p5;\
		double * __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, double *, double *, double *, double *, double *, double *))*(void**)(__base - 1372))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_get_tolerance(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_t *))*(void**)(__base - 904))(__t__p0));\
	})

#define cairo_set_miter_limit(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double ))*(void**)(__base - 184))(__t__p0, __t__p1));\
	})

#define cairo_rel_line_to(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double ))*(void**)(__base - 298))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_get_operator(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_operator_t (*)(cairo_t *))*(void**)(__base - 892))(__t__p0));\
	})

#define cairo_image_surface_create_from_png_stream(__p0, __p1) \
	({ \
		cairo_read_func_t  __t__p0 = __p0;\
		void * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_read_func_t , void *))*(void**)(__base - 1216))(__t__p0, __t__p1));\
	})

#define cairo_format_stride_for_width(__p0, __p1) \
	({ \
		cairo_format_t  __t__p0 = __p0;\
		int  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(cairo_format_t , int ))*(void**)(__base - 1168))(__t__p0, __t__p1));\
	})

#define cairo_get_line_cap(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_line_cap_t (*)(cairo_t *))*(void**)(__base - 940))(__t__p0));\
	})

#define cairo_image_surface_get_format(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_format_t (*)(cairo_surface_t *))*(void**)(__base - 1186))(__t__p0));\
	})

#define cairo_transform(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const cairo_matrix_t *))*(void**)(__base - 208))(__t__p0, __t__p1));\
	})

#define cairo_surface_reference(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_surface_t *))*(void**)(__base - 1030))(__t__p0));\
	})

#define cairo_set_line_join(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_line_join_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_line_join_t ))*(void**)(__base - 172))(__t__p0, __t__p1));\
	})

#define cairo_stroke_extents(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *, double *, double *))*(void**)(__base - 400))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_mesh_pattern_get_corner_color_rgba(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		unsigned int  __t__p1 = __p1;\
		unsigned int  __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		double * __t__p5 = __p5;\
		double * __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, unsigned int , unsigned int , double *, double *, double *, double *))*(void**)(__base - 1894))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_line_to(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double ))*(void**)(__base - 268))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_status(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_t *))*(void**)(__base - 1012))(__t__p0));\
	})

#define cairo_surface_set_user_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		cairo_destroy_func_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ))*(void**)(__base - 1090))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_surface_supports_mime_type(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_surface_t *, const char *))*(void**)(__base - 1738))(__t__p0, __t__p1));\
	})

#define cairo_new_sub_path(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 262))(__t__p0));\
	})

#define cairo_show_page(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 382))(__t__p0));\
	})

#define cairo_font_options_set_hint_metrics(__p0, __p1) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		cairo_hint_metrics_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_options_t *, cairo_hint_metrics_t ))*(void**)(__base - 550))(__t__p0, __t__p1));\
	})

#define cairo_ps_surface_dsc_begin_page_setup(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 2140))(__t__p0));\
	})

#define cairo_pattern_get_reference_count(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_pattern_t *))*(void**)(__base - 1264))(__t__p0));\
	})

#define cairo_surface_observer_add_finish_callback(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_surface_observer_callback_t  __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, cairo_surface_observer_callback_t , void *))*(void**)(__base - 1660))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_get_font_options(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_font_options_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_font_options_t *))*(void**)(__base - 592))(__t__p0, __t__p1));\
	})

#define cairo_pattern_get_filter(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_filter_t (*)(cairo_pattern_t *))*(void**)(__base - 1336))(__t__p0));\
	})

#define cairo_device_get_type(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_device_type_t (*)(cairo_device_t *))*(void**)(__base - 1534))(__t__p0));\
	})

#define cairo_device_reference(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_device_t *(*)(cairo_device_t *))*(void**)(__base - 1528))(__t__p0));\
	})

#define cairo_device_observer_glyphs_elapsed(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_device_t *))*(void**)(__base - 1714))(__t__p0));\
	})

#define cairo_get_group_target(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_t *))*(void**)(__base - 982))(__t__p0));\
	})

#define cairo_surface_observer_elapsed(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_surface_t *))*(void**)(__base - 1672))(__t__p0));\
	})

#define cairo_surface_create_for_rectangle(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_t *(*)(cairo_surface_t *, double , double , double , double ))*(void**)(__base - 1612))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_font_options_status(__p0) \
	({ \
		cairo_font_options_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_font_options_t *))*(void**)(__base - 490))(__t__p0));\
	})

#define cairo_surface_set_mime_data(__p0, __p1, __p2, __p3, __p4, __p5) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		const unsigned char * __t__p2 = __p2;\
		unsigned long  __t__p3 = __p3;\
		cairo_destroy_func_t  __t__p4 = __p4;\
		void * __t__p5 = __p5;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *, const char *, const unsigned char *, unsigned long , cairo_destroy_func_t , void *))*(void**)(__base - 1732))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5));\
	})

#define cairo_reference(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_t *(*)(cairo_t *))*(void**)(__base - 46))(__t__p0));\
	})

#define cairo_create(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_t *(*)(cairo_surface_t *))*(void**)(__base - 40))(__t__p0));\
	})

#define cairo_version() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(void))*(void**)(__base - 28))());\
	})

#define cairo_pdf_get_versions(__p0, __p1) \
	({ \
		cairo_pdf_version_t const ** __t__p0 = __p0;\
		int * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pdf_version_t const **, int *))*(void**)(__base - 2068))(__t__p0, __t__p1));\
	})

#define cairo_scaled_font_status(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_scaled_font_t *))*(void**)(__base - 736))(__t__p0));\
	})

#define cairo_toy_font_face_get_family(__p0) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((const char *(*)(cairo_font_face_t *))*(void**)(__base - 820))(__t__p0));\
	})

#define cairo_user_font_face_set_init_func(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		cairo_user_scaled_font_init_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *, cairo_user_scaled_font_init_func_t ))*(void**)(__base - 844))(__t__p0, __t__p1));\
	})

#define cairo_recording_surface_ink_extents(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double *, double *, double *, double *))*(void**)(__base - 1750))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_ps_surface_dsc_comment(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, const char *))*(void**)(__base - 2128))(__t__p0, __t__p1));\
	})

#define cairo_pattern_create_radial(__p0, __p1, __p2, __p3, __p4, __p5) \
	({ \
		double  __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_t *(*)(double , double , double , double , double , double ))*(void**)(__base - 1246))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5));\
	})

#define cairo_surface_get_type(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_surface_type_t (*)(cairo_surface_t *))*(void**)(__base - 1060))(__t__p0));\
	})

#define cairo_stroke_preserve(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 358))(__t__p0));\
	})

#define cairo_recording_surface_get_extents(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_rectangle_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_surface_t *, cairo_rectangle_t *))*(void**)(__base - 1756))(__t__p0, __t__p1));\
	})

#define cairo_device_release(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_device_t *))*(void**)(__base - 1552))(__t__p0));\
	})

#define cairo_surface_get_font_options(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_font_options_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, cairo_font_options_t *))*(void**)(__base - 1096))(__t__p0, __t__p1));\
	})

#define cairo_ft_scaled_font_unlock_face(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_scaled_font_t *))*(void**)(__base - 1498))(__t__p0));\
	})

#define cairo_surface_destroy(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *))*(void**)(__base - 1042))(__t__p0));\
	})

#define cairo_set_fill_rule(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_fill_rule_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_fill_rule_t ))*(void**)(__base - 154))(__t__p0, __t__p1));\
	})

#define cairo_raster_source_pattern_set_callback_data(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		void * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, void *))*(void**)(__base - 1768))(__t__p0, __t__p1));\
	})

#define cairo_scaled_font_text_to_glyphs(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		const char * __t__p3 = __p3;\
		int  __t__p4 = __p4;\
		cairo_glyph_t ** __t__p5 = __p5;\
		int * __t__p6 = __p6;\
		cairo_text_cluster_t ** __t__p7 = __p7;\
		int * __t__p8 = __p8;\
		cairo_text_cluster_flags_t * __t__p9 = __p9;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_scaled_font_t *, double , double , const char *, int , cairo_glyph_t **, int *, cairo_text_cluster_t **, int *, cairo_text_cluster_flags_t *))*(void**)(__base - 778))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6, __t__p7, __t__p8, __t__p9));\
	})

#define cairo_region_copy(__p0) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_region_t *(*)(const cairo_region_t *))*(void**)(__base - 1924))(__t__p0));\
	})

#define cairo_mesh_pattern_move_to(__p0, __p1, __p2) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, double , double ))*(void**)(__base - 1858))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_pdf_surface_restrict_to_version(__p0, __p1) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		cairo_pdf_version_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, cairo_pdf_version_t ))*(void**)(__base - 2062))(__t__p0, __t__p1));\
	})

#define cairo_clip_extents(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *, double *, double *))*(void**)(__base - 430))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_surface_status(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_surface_t *))*(void**)(__base - 1054))(__t__p0));\
	})

#define cairo_paint_with_alpha(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double ))*(void**)(__base - 334))(__t__p0, __t__p1));\
	})

#define cairo_device_finish(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_device_t *))*(void**)(__base - 1564))(__t__p0));\
	})

#define cairo_get_font_matrix(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_matrix_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_matrix_t *))*(void**)(__base - 580))(__t__p0, __t__p1));\
	})

#define cairo_svg_get_versions(__p0, __p1) \
	({ \
		cairo_svg_version_t const ** __t__p0 = __p0;\
		int * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_svg_version_t const **, int *))*(void**)(__base - 2164))(__t__p0, __t__p1));\
	})

#define cairo_arc(__p0, __p1, __p2, __p3, __p4, __p5) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double , double , double ))*(void**)(__base - 280))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5));\
	})

#define cairo_scaled_font_set_user_data(__p0, __p1, __p2, __p3) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		void * __t__p2 = __p2;\
		cairo_destroy_func_t  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_scaled_font_t *, const cairo_user_data_key_t *, void *, cairo_destroy_func_t ))*(void**)(__base - 754))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_destroy(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 52))(__t__p0));\
	})

#define cairo_matrix_transform_point(__p0, __p1, __p2) \
	({ \
		const cairo_matrix_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(const cairo_matrix_t *, double *, double *))*(void**)(__base - 1444))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_set_dash(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const double * __t__p1 = __p1;\
		int  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const double *, int , double ))*(void**)(__base - 178))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#define cairo_region_xor_rectangle(__p0, __p1) \
	({ \
		cairo_region_t * __t__p0 = __p0;\
		const cairo_rectangle_int_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_region_t *, const cairo_rectangle_int_t *))*(void**)(__base - 2038))(__t__p0, __t__p1));\
	})

#define cairo_device_observer_elapsed(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_device_t *))*(void**)(__base - 1684))(__t__p0));\
	})

#define cairo_set_tolerance(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double ))*(void**)(__base - 142))(__t__p0, __t__p1));\
	})

#define cairo_user_font_face_set_render_glyph_func(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		cairo_user_scaled_font_render_glyph_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_font_face_t *, cairo_user_scaled_font_render_glyph_func_t ))*(void**)(__base - 850))(__t__p0, __t__p1));\
	})

#define cairo_region_num_rectangles(__p0) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(const cairo_region_t *))*(void**)(__base - 1960))(__t__p0));\
	})

#define cairo_version_string() \
	({ \
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((const char *(*)(void))*(void**)(__base - 34))());\
	})

#define cairo_device_observer_fill_elapsed(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((double (*)(cairo_device_t *))*(void**)(__base - 1702))(__t__p0));\
	})

#define cairo_pattern_get_user_data(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_pattern_t *, const cairo_user_data_key_t *))*(void**)(__base - 1276))(__t__p0, __t__p1));\
	})

#define cairo_rel_curve_to(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		double  __t__p6 = __p6;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double , double , double , double ))*(void**)(__base - 304))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5, __t__p6));\
	})

#define cairo_in_clip(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_t *, double , double ))*(void**)(__base - 1522))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_arc_negative(__p0, __p1, __p2, __p3, __p4, __p5) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double  __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		double  __t__p4 = __p4;\
		double  __t__p5 = __p5;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double , double , double , double , double ))*(void**)(__base - 286))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4, __t__p5));\
	})

#define cairo_font_face_get_user_data(__p0, __p1) \
	({ \
		cairo_font_face_t * __t__p0 = __p0;\
		const cairo_user_data_key_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void *(*)(cairo_font_face_t *, const cairo_user_data_key_t *))*(void**)(__base - 700))(__t__p0, __t__p1));\
	})

#define cairo_font_options_equal(__p0, __p1) \
	({ \
		const cairo_font_options_t * __t__p0 = __p0;\
		const cairo_font_options_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(const cairo_font_options_t *, const cairo_font_options_t *))*(void**)(__base - 502))(__t__p0, __t__p1));\
	})

#define cairo_surface_has_show_text_glyphs(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(cairo_surface_t *))*(void**)(__base - 1156))(__t__p0));\
	})

#define cairo_copy_path(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_path_t *(*)(cairo_t *))*(void**)(__base - 988))(__t__p0));\
	})

#define cairo_pattern_get_type(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_pattern_type_t (*)(cairo_pattern_t *))*(void**)(__base - 1288))(__t__p0));\
	})

#define cairo_save(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 76))(__t__p0));\
	})

#define cairo_push_group(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *))*(void**)(__base - 88))(__t__p0));\
	})

#define cairo_pattern_get_linear_points(__p0, __p1, __p2, __p3, __p4) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		double * __t__p3 = __p3;\
		double * __t__p4 = __p4;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_status_t (*)(cairo_pattern_t *, double *, double *, double *, double *))*(void**)(__base - 1366))(__t__p0, __t__p1, __t__p2, __t__p3, __t__p4));\
	})

#define cairo_scaled_font_reference(__p0) \
	({ \
		cairo_scaled_font_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_scaled_font_t *(*)(cairo_scaled_font_t *))*(void**)(__base - 718))(__t__p0));\
	})

#define cairo_pattern_destroy(__p0) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *))*(void**)(__base - 1258))(__t__p0));\
	})

#define cairo_get_dash_count(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(cairo_t *))*(void**)(__base - 958))(__t__p0));\
	})

#define cairo_region_equal(__p0, __p1) \
	({ \
		const cairo_region_t * __t__p0 = __p0;\
		const cairo_region_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_bool_t (*)(const cairo_region_t *, const cairo_region_t *))*(void**)(__base - 1942))(__t__p0, __t__p1));\
	})

#define cairo_matrix_multiply(__p0, __p1, __p2) \
	({ \
		cairo_matrix_t * __t__p0 = __p0;\
		const cairo_matrix_t * __t__p1 = __p1;\
		const cairo_matrix_t * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_matrix_t *, const cairo_matrix_t *, const cairo_matrix_t *))*(void**)(__base - 1432))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_raster_source_pattern_set_copy(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_raster_source_copy_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_raster_source_copy_func_t ))*(void**)(__base - 1804))(__t__p0, __t__p1));\
	})

#define cairo_user_to_device(__p0, __p1, __p2) \
	({ \
		cairo_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, double *, double *))*(void**)(__base - 226))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_set_source(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_pattern_t * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_pattern_t *))*(void**)(__base - 118))(__t__p0, __t__p1));\
	})

#define cairo_device_get_reference_count(__p0) \
	({ \
		cairo_device_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((unsigned int (*)(cairo_device_t *))*(void**)(__base - 1576))(__t__p0));\
	})

#define cairo_ps_get_levels(__p0, __p1) \
	({ \
		cairo_ps_level_t const ** __t__p0 = __p0;\
		int * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_ps_level_t const **, int *))*(void**)(__base - 2098))(__t__p0, __t__p1));\
	})

#define cairo_show_text(__p0, __p1) \
	({ \
		cairo_t * __t__p0 = __p0;\
		const char * __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, const char *))*(void**)(__base - 622))(__t__p0, __t__p1));\
	})

#define cairo_raster_source_pattern_set_snapshot(__p0, __p1) \
	({ \
		cairo_pattern_t * __t__p0 = __p0;\
		cairo_raster_source_snapshot_func_t  __t__p1 = __p1;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_pattern_t *, cairo_raster_source_snapshot_func_t ))*(void**)(__base - 1792))(__t__p0, __t__p1));\
	})

#define cairo_surface_get_device_offset(__p0, __p1, __p2) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		double * __t__p1 = __p1;\
		double * __t__p2 = __p2;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_surface_t *, double *, double *))*(void**)(__base - 1126))(__t__p0, __t__p1, __t__p2));\
	})

#define cairo_get_fill_rule(__p0) \
	({ \
		cairo_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((cairo_fill_rule_t (*)(cairo_t *))*(void**)(__base - 928))(__t__p0));\
	})

#define cairo_image_surface_get_height(__p0) \
	({ \
		cairo_surface_t * __t__p0 = __p0;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((int (*)(cairo_surface_t *))*(void**)(__base - 1198))(__t__p0));\
	})

#define cairo_mask_surface(__p0, __p1, __p2, __p3) \
	({ \
		cairo_t * __t__p0 = __p0;\
		cairo_surface_t * __t__p1 = __p1;\
		double  __t__p2 = __p2;\
		double  __t__p3 = __p3;\
		long __base = (long)(CAIRO_BASE_NAME);\
		__asm volatile("mr 12,%0": :"r"(__base):"r12");\
		(((void (*)(cairo_t *, cairo_surface_t *, double , double ))*(void**)(__base - 346))(__t__p0, __t__p1, __t__p2, __t__p3));\
	})

#endif /* !_PPCINLINE_CAIRO_H */
