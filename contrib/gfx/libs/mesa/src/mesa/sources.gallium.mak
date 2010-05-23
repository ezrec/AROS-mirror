### Lists of source files, included by Makefiles

AROS_STATE_TRACKER_SOURCES = \
            state_trackers/aros/arosmesa_api \

AROS_STATE_TRACKER_HIDD_SOURCES = \
            state_trackers/aros/arosmesa_api_hidd \
            
AROS_WINSYS_SOURCES = \
            winsys/aros/arosmesa_winsys \
            winsys/aros/arosmesa_softpipe \
            winsys/aros/arosmesa_nouveau \
            winsys/aros/arosmesa_intel \
            winsys/drm/intel/gem/intel_drm_batchbuffer \
            winsys/drm/intel/gem/intel_drm_fence \
            winsys/drm/intel/gem/intel_drm_buffer \

GALLIUM_SOFTPIPE_SOURCES = \
            drivers/softpipe/sp_fs_exec \
            drivers/softpipe/sp_fs_sse \
            drivers/softpipe/sp_clear \
            drivers/softpipe/sp_flush \
            drivers/softpipe/sp_query \
            drivers/softpipe/sp_context \
            drivers/softpipe/sp_draw_arrays \
            drivers/softpipe/sp_prim_vbuf \
            drivers/softpipe/sp_quad_pipe \
            drivers/softpipe/sp_quad_blend \
            drivers/softpipe/sp_quad_depth_test \
            drivers/softpipe/sp_quad_fs \
            drivers/softpipe/sp_quad_stipple \
            drivers/softpipe/sp_screen \
            drivers/softpipe/sp_setup \
            drivers/softpipe/sp_state_blend \
            drivers/softpipe/sp_state_clip \
            drivers/softpipe/sp_state_derived \
            drivers/softpipe/sp_state_fs \
            drivers/softpipe/sp_state_sampler \
            drivers/softpipe/sp_state_rasterizer \
            drivers/softpipe/sp_state_surface \
            drivers/softpipe/sp_state_vertex \
            drivers/softpipe/sp_texture \
            drivers/softpipe/sp_tex_sample \
            drivers/softpipe/sp_tile_cache \
            drivers/softpipe/sp_tex_tile_cache \
            drivers/softpipe/sp_surface

GALLIUM_NVIDIA_SOURCES = \
            drivers/nouveau/nouveau_screen \
            drivers/nouveau/nouveau_context \
            drivers/nouveau/nv04_surface_2d \
            drivers/nv40/nv40_clear \
            drivers/nv40/nv40_context \
            drivers/nv40/nv40_draw \
            drivers/nv40/nv40_fragprog \
            drivers/nv40/nv40_fragtex \
            drivers/nv40/nv40_miptree \
            drivers/nv40/nv40_query \
            drivers/nv40/nv40_screen \
            drivers/nv40/nv40_state \
            drivers/nv40/nv40_state_blend \
            drivers/nv40/nv40_state_emit \
            drivers/nv40/nv40_state_fb \
            drivers/nv40/nv40_state_rasterizer \
            drivers/nv40/nv40_state_scissor \
            drivers/nv40/nv40_state_stipple \
            drivers/nv40/nv40_state_viewport \
            drivers/nv40/nv40_state_zsa \
            drivers/nv40/nv40_surface \
            drivers/nv40/nv40_transfer \
            drivers/nv40/nv40_vbo \
            drivers/nv40/nv40_vertprog \
            drivers/nv30/nv30_clear \
            drivers/nv30/nv30_context \
            drivers/nv30/nv30_draw \
            drivers/nv30/nv30_fragprog \
            drivers/nv30/nv30_fragtex \
            drivers/nv30/nv30_miptree \
            drivers/nv30/nv30_query \
            drivers/nv30/nv30_screen \
            drivers/nv30/nv30_state \
            drivers/nv30/nv30_state_blend \
            drivers/nv30/nv30_state_emit \
            drivers/nv30/nv30_state_fb \
            drivers/nv30/nv30_state_rasterizer \
            drivers/nv30/nv30_state_scissor \
            drivers/nv30/nv30_state_stipple \
            drivers/nv30/nv30_state_viewport \
            drivers/nv30/nv30_state_zsa \
            drivers/nv30/nv30_surface \
            drivers/nv30/nv30_transfer \
            drivers/nv30/nv30_vbo \
            drivers/nv30/nv30_vertprog \
            drivers/nv50/nv50_clear \
            drivers/nv50/nv50_context \
            drivers/nv50/nv50_draw \
            drivers/nv50/nv50_miptree \
            drivers/nv50/nv50_query \
            drivers/nv50/nv50_program \
            drivers/nv50/nv50_screen \
            drivers/nv50/nv50_state \
            drivers/nv50/nv50_state_validate \
            drivers/nv50/nv50_tex \
            drivers/nv50/nv50_vbo \
            drivers/nv50/nv50_surface \
            drivers/nv50/nv50_transfer \

GALLIUM_I915_SOURCES = \
            drivers/i915/i915_blit \
            drivers/i915/i915_buffer \
            drivers/i915/i915_clear \
            drivers/i915/i915_context \
            drivers/i915/i915_debug \
            drivers/i915/i915_debug_fp \
            drivers/i915/i915_flush \
            drivers/i915/i915_fpc_emit \
            drivers/i915/i915_fpc_translate \
            drivers/i915/i915_prim_emit \
            drivers/i915/i915_prim_vbuf \
            drivers/i915/i915_screen \
            drivers/i915/i915_state \
            drivers/i915/i915_state_derived \
            drivers/i915/i915_state_dynamic \
            drivers/i915/i915_state_emit \
            drivers/i915/i915_state_immediate \
            drivers/i915/i915_state_sampler \
            drivers/i915/i915_surface \
            drivers/i915/i915_texture \
            
AROS_GALLIUM_SOURCES = \
            $(AROS_STATE_TRACKER_SOURCES) \
            $(AROS_WINSYS_SOURCES) \
            $(GALLIUM_SOFTPIPE_SOURCES) \
            $(GALLIUM_NVIDIA_SOURCES) \
            $(GALLIUM_I915_SOURCES) \
