/*
**  CDXLPlay (C) 2009 Fredrik Wikstrom
**/

#include "cdxlplay.h"

struct cdxl_file *open_cdxl(const char *filename) {
	struct cdxl_file *cdxl;
	struct cdxl_frame frame;
	struct node *node;
	off_t offset = 0;
	int index = -1;
	int num_bytes;
	int depth = 0;
	int width = 0;
	int height = 0;
	int skip;
	
	cdxl = malloc(sizeof(*cdxl));
	if (!cdxl) {
		fprintf(stderr, "%s: not enough memory\n", filename);
		goto out;
	}
	
	memset(cdxl, 0, sizeof(*cdxl));
	init_list(&cdxl->list);
	
	cdxl->filename = strdup(filename);
	if (!cdxl->filename) {
		fprintf(stderr, "%s: not enough memory\n", filename);
		goto out;
	}
	
	cdxl->file = fopen(filename, "rb");
	if (!cdxl->file) {
		fprintf(stderr, "%s: file not found\n", filename);
		goto out;
	}
	
	index++;
	for (;;) {
		memset(&frame, 0, sizeof(frame));
		num_bytes = fread(&frame.pan, 1, PAN_SIZE, cdxl->file);
		if (num_bytes != PAN_SIZE) {
			if (num_bytes != 0) {
				fprintf(stderr, "%s: read error\n", filename);
				goto out;
			} else {
				break;
			}
		}
		offset += PAN_SIZE;

		frame.index = index;
		frame.offset = offset;
		wbe32(&frame.pan.Size, frame.pan.Size);
		wbe32(&frame.pan.Back, frame.pan.Back);
		wbe32(&frame.pan.Frame, frame.pan.Frame);
		wbe16(&frame.pan.XSize, frame.pan.XSize);
		wbe16(&frame.pan.YSize, frame.pan.YSize);
		wbe16(&frame.pan.ColorMapSize, frame.pan.ColorMapSize);
		wbe16(&frame.pan.AudioSize, frame.pan.AudioSize);
		frame.cmap_size = CMAP_SIZE(&frame.pan);
		frame.plane_size = PLANE_SIZE(&frame.pan);
		frame.video_size = IMAGE_SIZE(&frame.pan);
		frame.audio_size = AUDIO_SIZE(&frame.pan);
		frame.size = frame.cmap_size + frame.video_size + frame.audio_size;
		
		if (frame.pan.Type != PAN_STANDARD) {
			fprintf(stderr, "%s: non-standard cdxl file\n", filename);
			goto out;
		}
		
		if (frame.pan.Size < (PAN_SIZE + frame.size)) {
			fprintf(stderr, "%s: illegal frame size\n", filename);
			goto out;
		}
		
		if (frame.pan.Reserved != 0) {
			fprintf(stderr, "%s: non-zero reserved field\n", filename);
			goto out;
		}
		
		switch (PI_VIDEO(&frame.pan)) {
			case PIV_STANDARD:
				frame.is_ham = 0;
				break;
			case PIV_HAM:
				frame.is_ham = 1;
				break;
			default:
				fprintf(stderr, "%s: unsupported video format\n", filename);
				goto out;
		}
		switch (PI_PIXEL(&frame.pan)) {
			case PIF_PLANES:
				break;
			default:
				fprintf(stderr, "%s: unsupported pixel format\n", filename);
				goto out;
		}
		frame.is_stereo = (PI_AUDIO(&frame.pan) == PIA_STEREO);
		if (!depth && !width && !height) {
			depth = frame.pan.PixelSize;
			width = frame.pan.XSize;
			height = frame.pan.YSize;
			if (depth < 1 || depth > 8) {
				fprintf(stderr, "%s: unsupported pixel size %d\n", filename, depth);
				goto out;
			}
			if (!width || !height) {
				fprintf(stderr, "%s: illegal frame dimensions %dx%d\n", filename, width, height);
				goto out;
			}
		} else {
			if (frame.pan.PixelSize != depth || frame.pan.XSize != width || frame.pan.YSize != height) {
				fprintf(stderr, "%s: different sized frames in cdxl file\n", filename);
				goto out;
			}
		}
		
		skip = frame.pan.Size - PAN_SIZE;
		if (fseek(cdxl->file, skip, SEEK_CUR) == -1) {
			fprintf(stderr, "%s: seek error\n", filename);
			goto out;
		}
		offset += skip;
		
		node = malloc(sizeof(frame));
		if (!node) {
			fprintf(stderr, "%s: not enough memory\n", filename);
			goto out;
		}
		
		memcpy(node, &frame, sizeof(frame));
		add_tail(&cdxl->list, node);
		index++;
		if (frame.audio_size > 0) {
			cdxl->has_audio++;
		}
	}
	cdxl->num_frames = index;
	index = -1;
	if (!cdxl->num_frames || list_is_empty(&cdxl->list)) {
		fprintf(stderr, "%s: file is empty\n", filename);
		goto out;
	}
	cdxl->padded_width = (width + 15) & ~15;
	cdxl->chunky = malloc(cdxl->padded_width * height);
	if (!cdxl->chunky) {
		fprintf(stderr, "%s: not enough memory\n", filename);
		goto out;
	}
	return cdxl;
	
out:
	if (index != -1) fprintf(stderr, "%s: error at frame %d\n", filename, index);
	if (cdxl) close_cdxl(cdxl);
	return NULL;
}

void close_cdxl(struct cdxl_file *cdxl) {
	if (cdxl) {
		struct cdxl_frame *frame;
		if (cdxl->chunky) free(cdxl->chunky);
		while ((frame = (struct cdxl_frame *)rem_head(&cdxl->list))) {
			if (frame->buffer) free(frame->buffer);
			free(frame);
		}
		if (cdxl->file) fclose(cdxl->file);
		free(cdxl);
	}
}

int read_cdxl_frame(struct cdxl_file *cdxl, struct cdxl_frame *frame) {
	if (cdxl && frame) {
		if (frame->buffer) {
			return 0;
		}
		if (fseek(cdxl->file, frame->offset, SEEK_SET) == -1) {
			fprintf(stderr, "%s: seek error\n", cdxl->filename);
			return -1;
		}
		frame->buffer = malloc(frame->size);
		if (!frame->buffer) {
			fprintf(stderr, "%s: not enough memory\n", cdxl->filename);
			return -1;
		}
		if (fread(frame->buffer, 1, frame->size, cdxl->file) != frame->size) {
			fprintf(stderr, "%s: read error\n", cdxl->filename);
			free_cdxl_frame(cdxl, frame);
			return -1;
		}
		return 0;
	}
	return -1;
}

void free_cdxl_frame(struct cdxl_file *cdxl, struct cdxl_frame *frame) {
	if (cdxl && frame && frame->buffer) {
		free(frame->buffer);
		frame->buffer = NULL;
	}
}

#ifdef WORDS_BIGENDIAN
#define CMAP_RED(rgb) ((((rgb) >> 8) & 0xf) * 0x11)
#define CMAP_GREEN(rgb) ((((rgb) >> 4) & 0xf) * 0x11)
#define CMAP_BLUE(rgb) ((((rgb)) & 0xf) * 0x11)
#define STORE_RGB24(ptr, rgb) do { \
		*ptr++ = rgb >> 16; \
		*ptr++ = rgb >> 8; \
		*ptr++ = rgb; \
	} while (0);
#else
#define CMAP_RED(rgb) ((((rgb)) & 0xf) * 0x11)
#define CMAP_GREEN(rgb) ((((rgb) >> 12) & 0xf) * 0x11)
#define CMAP_BLUE(rgb) ((((rgb) >> 8) & 0xf) * 0x11)
#define STORE_RGB24(ptr, rgb) do { \
		*ptr++ = rgb; \
		*ptr++ = rgb >> 8; \
		*ptr++ = rgb >> 16; \
	} while (0);
#endif

static void decode_cdxl_ham6(uint16_t *cmap, const uint8_t *chunky, SDL_Surface *surf, int w, int h) {
	SDL_PixelFormat *fmt = surf->format;
	uint8_t *dest = surf->pixels;
	uint32_t modulo = surf->pitch - w*fmt->BytesPerPixel;
	uint8_t op, index, r, g, b;
	uint32_t rgb;
	int x, y;
	for (y = 0; y < h; y++) {
		r = CMAP_RED(cmap[0]);
		g = CMAP_GREEN(cmap[0]);
		b = CMAP_BLUE(cmap[0]);
		for (x = 0; x < w; x++) {
			index = *chunky++;
			op = index >> 4;
			index &= 15;
			switch (op) {
				case 0:
					r = CMAP_RED(cmap[index]);
					g = CMAP_GREEN(cmap[index]);
					b = CMAP_BLUE(cmap[index]);
					break;
				case 1:
					b = index * 0x11;
					break;
				case 2:
					r = index * 0x11;
					break;
				case 3:
					g = index * 0x11;
					break;
			}
			rgb = SDL_MapRGB(fmt, r, g, b);
			switch (fmt->BytesPerPixel) {
				case 1:
					*dest++ = rgb;
					break;
				case 2:
					*(uint16_t *)dest = rgb;
					dest += 2;
					break;
				case 3:
					STORE_RGB24(dest, rgb);
					break;
				case 4:
					*(uint32_t *)dest = rgb;
					dest += 4;
					break;
			}
		}
		dest += modulo;
	}
}

static void decode_cdxl_ham8(uint16_t *cmap, const uint8_t *chunky, SDL_Surface *surf, int w, int h) {
	SDL_PixelFormat *fmt = surf->format;
	uint8_t *dest = surf->pixels;
	uint32_t modulo = surf->pitch - w*fmt->BytesPerPixel;
	uint8_t op, index, r, g, b;
	uint32_t rgb;
	int x, y;
	for (y = 0; y < h; y++) {
		r = CMAP_RED(cmap[0]);
		g = CMAP_GREEN(cmap[0]);
		b = CMAP_BLUE(cmap[0]);
		for (x = 0; x < w; x++) {
			index = *chunky++;
			op = index >> 6;
			index &= 63;
			switch (op) {
				case 0:
					r = CMAP_RED(cmap[index]);
					g = CMAP_GREEN(cmap[index]);
					b = CMAP_BLUE(cmap[index]);
					break;
				case 1:
					b = (index << 2)|(b & 3);
					break;
				case 2:
					r = (index << 2)|(r & 3);
					break;
				case 3:
					g = (index << 2)|(g & 3);
					break;
			}
			rgb = SDL_MapRGB(fmt, r, g, b);
			switch (fmt->BytesPerPixel) {
				case 1:
					*dest++ = rgb;
					break;
				case 2:
					*(uint16_t *)dest = rgb;
					dest += 2;
					break;
				case 3:
					STORE_RGB24(dest, rgb);
					break;
				case 4:
					*(uint32_t *)dest = rgb;
					dest += 4;
					break;
			}
		}
		dest += modulo;
	}
}

static void decode_cdxl_clut(uint16_t *cmap, const uint8_t *chunky, SDL_Surface *surf, int w, int h) {
	SDL_PixelFormat *fmt = surf->format;
	uint8_t *dest = surf->pixels;
	uint32_t modulo = surf->pitch - w*fmt->BytesPerPixel;
	uint8_t index, r, g, b;
	uint32_t rgb;
	int x, y;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			index = *chunky++;
			r = CMAP_RED(cmap[index]);
			g = CMAP_GREEN(cmap[index]);
			b = CMAP_BLUE(cmap[index]);
			rgb = SDL_MapRGB(fmt, r, g, b);
			switch (fmt->BytesPerPixel) {
				case 1:
					*dest++ = rgb;
					break;
				case 2:
					*(uint16_t *)dest = rgb;
					dest += 2;
					break;
				case 3:
					STORE_RGB24(dest, rgb);
					break;
				case 4:
					*(uint32_t *)dest = rgb;
					dest += 4;
					break;
			}
		}
		dest += modulo;
	}
}

int decode_cdxl_frame(struct cdxl_file *cdxl, struct cdxl_frame *frame, SDL_Surface *surf) {
	if (cdxl && frame && frame->buffer && surf) {
		void *cmap = frame->buffer;
		void *video = frame->buffer + frame->cmap_size;
		int depth = frame->pan.PixelSize;
		int w = cdxl->padded_width;
		int h = frame->pan.YSize;
		void *chunky = cdxl->chunky;
		if (SDL_LockSurface(surf) == -1) {
			fprintf(stderr, "%s: surface lock failed\n", cdxl->filename);
			return -1;
		}
		planar2chunky(video, frame->plane_size, chunky, w, h, depth);
		if (frame->is_ham) {
			if (depth <= 6) {
				decode_cdxl_ham6(cmap, chunky, surf, w, h);
			} else {
				decode_cdxl_ham8(cmap, chunky, surf, w, h);
			}
		} else {
			decode_cdxl_clut(cmap, chunky, surf, w, h);
		}
		SDL_UnlockSurface(surf);
		return 0;
	}
	return -1;
}
