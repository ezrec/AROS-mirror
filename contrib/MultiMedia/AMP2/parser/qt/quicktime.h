#ifndef FUNCPROTOS_H
#define FUNCPROTOS_H

#include "qtprivate.h"

/* atom.c */
int quicktime_atom_reset(quicktime_atom_t *atom);
int quicktime_atom_read_header(quicktime_t *file, quicktime_atom_t *atom);
int quicktime_atom_is(quicktime_atom_t *atom, char *type);
unsigned long quicktime_atom_read_size(char *data);
longest quicktime_atom_read_size64(char *data);
void quicktime_atom_read_type(char *data, char *type);
int quicktime_atom_skip(quicktime_t *file, quicktime_atom_t *atom);

/* ctab.c */
int quicktime_ctab_init(quicktime_ctab_t *ctab);
int quicktime_ctab_delete(quicktime_ctab_t *ctab);
int quicktime_read_ctab(quicktime_t *file, quicktime_ctab_t *ctab);

/* dinf.c */
void quicktime_dinf_init(quicktime_dinf_t *dinf);
void quicktime_dinf_delete(quicktime_dinf_t *dinf);
void quicktime_dinf_init_all(quicktime_dinf_t *dinf);
void quicktime_read_dinf(quicktime_t *file, quicktime_dinf_t *dinf, quicktime_atom_t *dinf_atom);

/* dref.c */
void quicktime_dref_table_init(quicktime_dref_table_t *table);
void quicktime_dref_table_delete(quicktime_dref_table_t *table);
void quicktime_read_dref_table(quicktime_t *file, quicktime_dref_table_t *table);
void quicktime_dref_init(quicktime_dref_t *dref);
void quicktime_dref_init_all(quicktime_dref_t *dref);
void quicktime_dref_delete(quicktime_dref_t *dref);
void quicktime_read_dref(quicktime_t *file, quicktime_dref_t *dref);

/* edts.c */
void quicktime_edts_init(quicktime_edts_t *edts);
void quicktime_edts_delete(quicktime_edts_t *edts);
void quicktime_edts_init_table(quicktime_edts_t *edts);
void quicktime_read_edts(quicktime_t *file, quicktime_edts_t *edts, quicktime_atom_t *edts_atom);

/* elst.c */
void quicktime_elst_table_init(quicktime_elst_table_t *table);
void quicktime_elst_table_delete(quicktime_elst_table_t *table);
void quicktime_read_elst_table(quicktime_t *file, quicktime_elst_table_t *table);
void quicktime_elst_init(quicktime_elst_t *elst);
void quicktime_elst_init_all(quicktime_elst_t *elst);
void quicktime_elst_delete(quicktime_elst_t *elst);
void quicktime_read_elst(quicktime_t *file, quicktime_elst_t *elst);

/* hdlr.c */
void quicktime_hdlr_init(quicktime_hdlr_t *hdlr);
void quicktime_hdlr_init_video(quicktime_hdlr_t *hdlr);
void quicktime_hdlr_init_audio(quicktime_hdlr_t *hdlr);
void quicktime_hdlr_init_data(quicktime_hdlr_t *hdlr);
void quicktime_hdlr_delete(quicktime_hdlr_t *hdlr);
void quicktime_read_hdlr(quicktime_t *file, quicktime_hdlr_t *hdlr);

/* matrix.c */
void quicktime_matrix_init(quicktime_matrix_t *matrix);
void quicktime_matrix_delete(quicktime_matrix_t *matrix);
void quicktime_read_matrix(quicktime_t *file, quicktime_matrix_t *matrix);

/* mdat.c */
void quicktime_mdat_delete(quicktime_mdat_t *mdat);
void quicktime_read_mdat(quicktime_t *file, quicktime_mdat_t *mdat, quicktime_atom_t *parent_atom);

/* mdhd.c */
void quicktime_mdhd_init(quicktime_mdhd_t *mdhd);
void quicktime_mdhd_init_video(quicktime_t *file, quicktime_mdhd_t *mdhd, int frame_w, int frame_h, float frame_rate);
void quicktime_mdhd_init_audio(quicktime_t *file, quicktime_mdhd_t *mdhd, int channels, int sample_rate, int bits, char *compressor);
void quicktime_mdhd_delete(quicktime_mdhd_t *mdhd);
void quicktime_read_mdhd(quicktime_t *file, quicktime_mdhd_t *mdhd);

/* mdia.c */
void quicktime_mdia_init(quicktime_mdia_t *mdia);
void quicktime_mdia_init_video(quicktime_t *file, quicktime_mdia_t *mdia, int frame_w, int frame_h, float frame_rate, char *compressor);
void quicktime_mdia_init_audio(quicktime_t *file, quicktime_mdia_t *mdia, int channels, int sample_rate, int bits, char *compressor);
void quicktime_mdia_delete(quicktime_mdia_t *mdia);
int quicktime_read_mdia(quicktime_t *file, quicktime_mdia_t *mdia, quicktime_atom_t *trak_atom);

/* minf.c */
void quicktime_minf_init(quicktime_minf_t *minf);
void quicktime_minf_init_video(quicktime_t *file, quicktime_minf_t *minf, int frame_w, int frame_h, int time_scale, float frame_rate, char *compressor);
void quicktime_minf_init_audio(quicktime_t *file, quicktime_minf_t *minf, int channels, int sample_rate, int bits, char *compressor);
void quicktime_minf_delete(quicktime_minf_t *minf);
int quicktime_read_minf(quicktime_t *file, quicktime_minf_t *minf, quicktime_atom_t *parent_atom);

/* moov.c */
int quicktime_moov_init(quicktime_moov_t *moov);
int quicktime_moov_delete(quicktime_moov_t *moov);
int quicktime_read_moov(quicktime_t *file, quicktime_moov_t *moov, quicktime_atom_t *parent_atom);
void quicktime_update_durations(quicktime_moov_t *moov);
int quicktime_shift_offsets(quicktime_moov_t *moov, longest offset);

/* mvhd.c */
int quicktime_mvhd_init(quicktime_mvhd_t *mvhd);
int quicktime_mvhd_delete(quicktime_mvhd_t *mvhd);
void quicktime_read_mvhd(quicktime_t *file, quicktime_mvhd_t *mvhd);
void quicktime_mhvd_init_video(quicktime_t *file, quicktime_mvhd_t *mvhd, float frame_rate);

/* quicktime.c */
longest quicktime_samples_to_bytes(quicktime_trak_t *track, long samples);
char* quicktime_get_copyright(quicktime_t *file);
char* quicktime_get_name(quicktime_t *file);
char* quicktime_get_info(quicktime_t *file);
int quicktime_video_tracks(quicktime_t *file);
int quicktime_audio_tracks(quicktime_t *file);
quicktime_trak_t* quicktime_add_track(quicktime_moov_t *moov);
int quicktime_init(quicktime_t *file);
int quicktime_delete(quicktime_t *file);
void quicktime_set_preload(quicktime_t *file, longest preload);
int quicktime_get_timescale(float frame_rate);
int quicktime_seek_end(quicktime_t *file);
int quicktime_seek_start(quicktime_t *file);
long quicktime_audio_length(quicktime_t *file, int track);
long quicktime_video_length(quicktime_t *file, int track);
long quicktime_audio_position(quicktime_t *file, int track);
long quicktime_video_position(quicktime_t *file, int track);
int quicktime_update_positions(quicktime_t *file);
int quicktime_set_audio_position(quicktime_t *file, longest sample, int track);
int quicktime_set_video_position(quicktime_t *file, longest frame, int track);
int quicktime_has_audio(quicktime_t *file);
long quicktime_sample_rate(quicktime_t *file, int track);
int quicktime_audio_bits(quicktime_t *file, int track);
char* quicktime_audio_compressor(quicktime_t *file, int track);
int quicktime_track_channels(quicktime_t *file, int track);
int quicktime_channel_location(quicktime_t *file, int *quicktime_track, int *quicktime_channel, int channel);
int quicktime_has_video(quicktime_t *file);
int quicktime_video_width(quicktime_t *file, int track);
int quicktime_video_height(quicktime_t *file, int track);
int quicktime_video_depth(quicktime_t *file, int track);
float quicktime_frame_rate(quicktime_t *file, int track);
char* quicktime_video_compressor(quicktime_t *file, int track);
long quicktime_read_audio(quicktime_t *file, char *audio_buffer, long samples, int track);
int quicktime_read_chunk(quicktime_t *file, char *output, int track, longest chunk, longest byte_start, longest byte_len);
long quicktime_frame_size(quicktime_t *file, long frame, int track);
long quicktime_frame_size_audio(quicktime_t *file, int track, char *buffer);
long quicktime_audio_frame_size(quicktime_t *file, int track);
long quicktime_read_frame(quicktime_t *file, unsigned char *video_buffer, int track);
long quicktime_get_keyframe_before(quicktime_t *file, long frame, int track);
int quicktime_has_keyframes(quicktime_t *file, int track);
int quicktime_read_frame_init(quicktime_t *file, int track);
int quicktime_read_frame_end(quicktime_t *file, int track);
int quicktime_init_video_map(quicktime_video_map_t *vtrack, quicktime_trak_t *trak);
int quicktime_delete_video_map(quicktime_video_map_t *vtrack);
int quicktime_init_audio_map(quicktime_audio_map_t *atrack, quicktime_trak_t *trak);
int quicktime_delete_audio_map(quicktime_audio_map_t *atrack);
int quicktime_read_info(quicktime_t *file);
int quicktime_check_sig(char *path);
quicktime_t* quicktime_open(char *filename);
int quicktime_close(quicktime_t *file);

/* smhd.c */
void quicktime_smhd_init(quicktime_smhd_t *smhd);
void quicktime_smhd_delete(quicktime_smhd_t *smhd);
void quicktime_read_smhd(quicktime_t *file, quicktime_smhd_t *smhd);

/* stbl.c */
void quicktime_stbl_init(quicktime_stbl_t *stbl);
void quicktime_stbl_init_video(quicktime_t *file, quicktime_stbl_t *stbl, int frame_w, int frame_h, int time_scale, float frame_rate, char *compressor);
void quicktime_stbl_init_audio(quicktime_t *file, quicktime_stbl_t *stbl, int channels, int sample_rate, int bits, char *compressor);
void quicktime_stbl_delete(quicktime_stbl_t *stbl);
int quicktime_read_stbl(quicktime_t *file, quicktime_minf_t *minf, quicktime_stbl_t *stbl, quicktime_atom_t *parent_atom);

/* stco.c */
void quicktime_stco_init(quicktime_stco_t *stco);
void quicktime_stco_delete(quicktime_stco_t *stco);
void quicktime_stco_init_common(quicktime_t *file, quicktime_stco_t *stco);
void quicktime_read_stco(quicktime_t *file, quicktime_stco_t *stco);
void quicktime_read_stco64(quicktime_t *file, quicktime_stco_t *stco);
void quicktime_update_stco(quicktime_stco_t *stco, long chunk, longest offset);

/* stsc.c */
void quicktime_stsc_init(quicktime_stsc_t *stsc);
void quicktime_stsc_init_table(quicktime_t *file, quicktime_stsc_t *stsc);
void quicktime_stsc_init_video(quicktime_t *file, quicktime_stsc_t *stsc);
void quicktime_stsc_init_audio(quicktime_t *file, quicktime_stsc_t *stsc, int sample_rate);
void quicktime_stsc_delete(quicktime_stsc_t *stsc);
void quicktime_read_stsc(quicktime_t *file, quicktime_stsc_t *stsc);
int quicktime_update_stsc(quicktime_stsc_t *stsc, long chunk, long samples);

/* stsd.c */
void quicktime_stsd_init(quicktime_stsd_t *stsd);
void quicktime_stsd_init_table(quicktime_stsd_t *stsd);
void quicktime_stsd_init_video(quicktime_t *file, quicktime_stsd_t *stsd, int frame_w, int frame_h, float frame_rate, char *compression);
void quicktime_stsd_init_audio(quicktime_t *file, quicktime_stsd_t *stsd, int channels, int sample_rate, int bits, char *compressor);
void quicktime_stsd_delete(quicktime_stsd_t *stsd);
void quicktime_read_stsd(quicktime_t *file, quicktime_minf_t *minf, quicktime_stsd_t *stsd);

/* stsdtable.c */
void quicktime_mjqt_init(quicktime_mjqt_t *mjqt);
void quicktime_mjqt_delete(quicktime_mjqt_t *mjqt);
void quicktime_mjht_init(quicktime_mjht_t *mjht);
void quicktime_mjht_delete(quicktime_mjht_t *mjht);
void quicktime_read_stsd_audio(quicktime_t *file, quicktime_stsd_table_t *table, quicktime_atom_t *parent_atom);
void quicktime_read_stsd_video(quicktime_t *file, quicktime_stsd_table_t *table, quicktime_atom_t *parent_atom);
void quicktime_read_stsd_table(quicktime_t *file, quicktime_minf_t *minf, quicktime_stsd_table_t *table);
void quicktime_stsd_table_init(quicktime_stsd_table_t *table);
void quicktime_stsd_table_delete(quicktime_stsd_table_t *table);

/* stss.c */
void quicktime_stss_init(quicktime_stss_t *stss);
void quicktime_stss_delete(quicktime_stss_t *stss);
void quicktime_read_stss(quicktime_t *file, quicktime_stss_t *stss);

/* stsz.c */
void quicktime_stsz_init(quicktime_stsz_t *stsz);
void quicktime_stsz_init_video(quicktime_t *file, quicktime_stsz_t *stsz);
void quicktime_stsz_init_audio(quicktime_t *file, quicktime_stsz_t *stsz, int channels, int bits, char *compressor);
void quicktime_stsz_delete(quicktime_stsz_t *stsz);
void quicktime_read_stsz(quicktime_t *file, quicktime_stsz_t *stsz);
void quicktime_update_stsz(quicktime_stsz_t *stsz, long sample, long sample_size);

/* stts.c */
void quicktime_stts_init(quicktime_stts_t *stts);
void quicktime_stts_init_table(quicktime_stts_t *stts);
void quicktime_stts_init_video(quicktime_t *file, quicktime_stts_t *stts, int time_scale, float frame_rate);
void quicktime_stts_init_audio(quicktime_t *file, quicktime_stts_t *stts, int sample_rate);
void quicktime_stts_delete(quicktime_stts_t *stts);
void quicktime_read_stts(quicktime_t *file, quicktime_stts_t *stts);

/* tkhd.c */
int quicktime_tkhd_init(quicktime_tkhd_t *tkhd);
int quicktime_tkhd_delete(quicktime_tkhd_t *tkhd);
void quicktime_read_tkhd(quicktime_t *file, quicktime_tkhd_t *tkhd);
void quicktime_tkhd_init_video(quicktime_t *file, quicktime_tkhd_t *tkhd, int frame_w, int frame_h);

/* trak.c */
int quicktime_trak_init(quicktime_trak_t *trak);
int quicktime_trak_init_video(quicktime_t *file, quicktime_trak_t *trak, int frame_w, int frame_h, float frame_rate, char *compressor);
int quicktime_trak_init_audio(quicktime_t *file, quicktime_trak_t *trak, int channels, int sample_rate, int bits, char *compressor);
int quicktime_trak_delete(quicktime_trak_t *trak);
quicktime_trak_t* quicktime_add_trak(quicktime_moov_t *moov);
int quicktime_delete_trak(quicktime_moov_t *moov);
int quicktime_read_trak(quicktime_t *file, quicktime_trak_t *trak, quicktime_atom_t *trak_atom);
longest quicktime_track_end(quicktime_trak_t *trak);
long quicktime_track_samples(quicktime_t *file, quicktime_trak_t *trak);
long quicktime_sample_of_chunk(quicktime_trak_t *trak, long chunk);
int quicktime_chunk_of_sample(longest *chunk_sample, longest *chunk, quicktime_trak_t *trak, long sample);
longest quicktime_chunk_to_offset(quicktime_trak_t *trak, long chunk);
long quicktime_offset_to_chunk(longest *chunk_offset, quicktime_trak_t *trak, longest offset);
longest quicktime_sample_range_size(quicktime_trak_t *trak, long chunk_sample, long sample);
longest quicktime_sample_to_offset(quicktime_trak_t *trak, long sample);
long quicktime_offset_to_sample(quicktime_trak_t *trak, longest offset);
int quicktime_update_tables(quicktime_t *file, quicktime_trak_t *trak, longest offset, longest chunk, longest sample, longest samples, longest sample_size);
int quicktime_trak_duration(quicktime_trak_t *trak, long *duration, long *timescale);
int quicktime_trak_fix_counts(quicktime_t *file, quicktime_trak_t *trak);
long quicktime_chunk_samples(quicktime_trak_t *trak, long chunk);
int quicktime_trak_shift_offsets(quicktime_trak_t *trak, longest offset);

/* udta.c */
int quicktime_udta_init(quicktime_udta_t *udta);
int quicktime_udta_delete(quicktime_udta_t *udta);
int quicktime_read_udta(quicktime_t *file, quicktime_udta_t *udta, quicktime_atom_t *udta_atom);
int quicktime_read_udta_string(quicktime_t *file, char **string, int *size);
int quicktime_set_udta_string(char **string, int *size, char *new_string);

/* util.c */
longest quicktime_ftell(quicktime_t *file);
int quicktime_fseek(quicktime_t *file, longest offset);
int quicktime_read_data(quicktime_t *file, char *data, longest size);
longest quicktime_byte_position(quicktime_t *file);
void quicktime_read_pascal(quicktime_t *file, char *data);
float quicktime_read_fixed32(quicktime_t *file);
float quicktime_read_fixed16(quicktime_t *file);
unsigned long quicktime_read_uint32(quicktime_t *file);
long quicktime_read_int32(quicktime_t *file);
longest quicktime_read_int64(quicktime_t *file);
long quicktime_read_int24(quicktime_t *file);
int quicktime_read_int16(quicktime_t *file);
int quicktime_read_char(quicktime_t *file);
void quicktime_read_char32(quicktime_t *file, char *string);
longest quicktime_position(quicktime_t *file); 
int quicktime_set_position(quicktime_t *file, longest position);
void quicktime_copy_char32(char *output, char *input);
unsigned long quicktime_current_time(void);
int quicktime_match_32(char *input, char *output);

/* vmhd.c */
void quicktime_vmhd_init(quicktime_vmhd_t *vmhd);
void quicktime_vmhd_init_video(quicktime_t *file, quicktime_vmhd_t *vmhd, int frame_w, int frame_h, float frame_rate);
void quicktime_vmhd_delete(quicktime_vmhd_t *vmhd);
void quicktime_read_vmhd(quicktime_t *file, quicktime_vmhd_t *vmhd);

#endif
