/*
 *
 * mpeg2_internal.h
 *
 */

/* macroblock modes */
#define MACROBLOCK_INTRA 1
#define MACROBLOCK_PATTERN 2
#define MACROBLOCK_MOTION_BACKWARD 4
#define MACROBLOCK_MOTION_FORWARD 8
#define MACROBLOCK_QUANT 16
#define DCT_TYPE_INTERLACED 32
/* motion_type */
#define MOTION_TYPE_MASK (3*64)
#define MOTION_TYPE_BASE 64
#define MC_FIELD (1*64)
#define MC_FRAME (2*64)
#define MC_16X8 (2*64)
#define MC_DMV (3*64)

/* picture structure */
#define TOP_FIELD 1
#define BOTTOM_FIELD 2
#define FRAME_PICTURE 3

/* picture coding type */
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4

typedef struct motion_s {
    uint8_t * ref[2][3];
    int pmv[2][2];
    int f_code[2];
} motion_t;

struct vo_frame_s {
    uint8_t * base[3];   /* pointer to 3 planes */
};

typedef struct picture_s {
    /* first, state that carries information from one macroblock to the */
    /* next inside a slice, and is never used outside of slice_process() */

    /* DCT coefficients - should be kept aligned ! */
    int16_t DCTblock[64];

    /* bit parsing stuff */
    uint32_t bitstream_buf;   /* current 32 bit working set of buffer */
    int bitstream_bits;       /* used bits in working set */
    uint8_t * bitstream_ptr;  /* buffer with stream data */

    /* Motion vectors */
    /* The f_ and b_ correspond to the forward and backward motion */
    /* predictors */
    motion_t b_motion;
    motion_t f_motion;

    /* predictor for DC coefficients in intra blocks */
    int16_t dc_dct_pred[3];

    int quantizer_scale; /* remove */
    int current_field;   /* remove */

    /* now non-slice-specific information */

    /* sequence header stuff */
    uint8_t intra_quantizer_matrix [64];
    uint8_t non_intra_quantizer_matrix [64];

    /* The width and height of the picture */
    int picture_width;
    int picture_height;

    /* The width and height of the picture snapped to macroblock units */
    int coded_picture_width;
    int coded_picture_height;

    /* picture header stuff */

    /* what type of picture this is (I, P, B, D) */
    int picture_coding_type;
     
    /* picture coding extension stuff */
     
    /* quantization factor for intra dc coefficients */
    int intra_dc_precision;
    /* top/bottom/both fields */
    int picture_structure;
    /* bool to indicate all predictions are frame based */
    int frame_pred_frame_dct;
    /* bool to indicate whether intra blocks have motion vectors */
    /* (for concealment) */
    int concealment_motion_vectors;
    /* bit to indicate which quantization table to use */
    int q_scale_type;
    /* bool to use different vlc tables */
    int intra_vlc_format;
    /* used for DMV MC */
    int top_field_first;

    /* stuff derived from bitstream */

    /* pointer to the zigzag scan we're supposed to be using */
    uint8_t * scan;

    struct vo_frame_s * current_frame;
    struct vo_frame_s * forward_reference_frame;
    struct vo_frame_s * backward_reference_frame;

    int second_field;

    int mpeg1;

    /* these things are not needed by the decoder */
    /* this is a temporary interface, we will build a better one later. */
    int aspect_ratio_information;
    int frame_rate_code;
    int progressive_sequence;
    int repeat_first_field;
    int progressive_frame;
    int bitrate;

    /* AmiDog, controls y(uv) decoding */
    int gray;
} picture_t;

/* slice.c */
void header_state_init (picture_t * picture);
int header_process_picture_header (picture_t * picture, uint8_t * buffer);
int header_process_sequence_header (picture_t * picture, uint8_t * buffer);
int header_process_extension (picture_t * picture, uint8_t * buffer);

/* idct.c */
void idct_init (void);

/* motion_comp.c */
void motion_comp_init (void);

typedef struct mc_functions_s
{
    void (* put [8]) (uint8_t *dst, uint8_t *, int32_t, int32_t);
    void (* avg [8]) (uint8_t *dst, uint8_t *, int32_t, int32_t);
} mc_functions_t;

#define MOTION_COMP_EXTERN(x) mc_functions_t mc_functions_##x =       \
{                                            \
    {MC_put_16_##x, MC_put_x16_##x, MC_put_y16_##x, MC_put_xy16_##x,  \
     MC_put_8_##x,  MC_put_x8_##x,  MC_put_y8_##x,  MC_put_xy8_##x},  \
    {MC_avg_16_##x, MC_avg_x16_##x, MC_avg_y16_##x, MC_avg_xy16_##x,  \
     MC_avg_8_##x,  MC_avg_x8_##x,  MC_avg_y8_##x,  MC_avg_xy8_##x}   \
};

extern mc_functions_t mc_functions_c;

/* slice.c */
void slice_process (picture_t *picture, uint8_t code, uint8_t * buffer);

/* align */
#define ATTR_ALIGN(align) __attribute__ ((__aligned__ (align)))
