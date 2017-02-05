#include "cdxlplay.h"
#include "usleep.h"

uint32_t get_frame_duration (struct player_data *pd, struct cdxl_frame *frame);
void start_timer (struct player_data *pd);
void start_audio (struct player_data *pd);
void stop_timer (struct player_data *pd);
void stop_audio (struct player_data *pd);
struct cdxl_frame *get_next_frame(struct list *list, struct cdxl_frame *frame, int loop);
void audio_callback (void *userdata, uint8_t *stream, int len);
uint32_t timer_callback (uint32_t interval, void *param);
int do_cdxl_frame (struct player_data *pd);
int do_cdxl_audio (struct player_data *pd);
int decode_cdxl_audio (struct cdxl_file *cdxl, struct cdxl_frame *frame, int8_t **pcm, int *alen);

int main (int argc, char *argv[]) {
	struct player_data pd;
	int sdl_init = 0;
	int gl_init = 0;
	int quit = 0;
	SDL_Event event;
	SDL_AudioSpec desired;
	int audio_init = 0;
	uint64_t t1, t2;
	int x_factor, y_factor;
	int min_width, min_height;
	
	memset(&pd, 0, sizeof(pd));
	pd.sdl_video_flags = SDL_ANYFORMAT|SDL_RESIZABLE|SDL_OPENGL;
	init_list(&pd.audio_list);
	pd.aprebuf = 1;
	pd.fps = 50;
	pd.freq = 11025;
	pd.loop = 0;
	pd.status = PLAYING;
	pd.pixel_ratio = 0x10000;
	
	if (!get_options(argc, argv, &pd)) {
		goto out;
	}
	
	pd.cdxl = open_cdxl(pd.filename);
	if (!pd.cdxl) {
		goto out;
	}
	
	if (!setup_usleep()) {
		goto out;
	}
	
	if (SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO) == -1) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		goto out;
	}
	sdl_init = 1;
	
	pd.frame = (struct cdxl_frame *)pd.cdxl->list.head;
	if (pd.pixel_ratio < 0x10000UL) {
		x_factor = 1;
		y_factor = 0x10000UL / pd.pixel_ratio;
	} else {
		x_factor = pd.pixel_ratio / 0x10000UL;
		y_factor = 1;
	}
	min_width = pd.frame->pan.XSize * x_factor;
	min_height = pd.frame->pan.YSize * y_factor;
	
	pd.width = MAX(pd.width, min_width);
	pd.height = MAX(pd.height, min_height);
	pd.screen = SDL_SetVideoMode(pd.width, pd.height, 32, pd.sdl_video_flags);
#ifdef WORDS_BIGENDIAN
	pd.surf = SDL_CreateRGBSurface(SDL_SWSURFACE,
		pd.frame->pan.XSize, pd.frame->pan.YSize, 32,
		0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
	pd.surf = SDL_CreateRGBSurface(SDL_SWSURFACE,
		pd.frame->pan.XSize, pd.frame->pan.YSize, 32,
		0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
	if (!pd.screen || !pd.surf) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		goto out;
	}
	
	SDL_WM_SetCaption(PROGNAME" "VSTRING, PROGNAME);
	
	SDL_ShowCursor((pd.sdl_video_flags & SDL_FULLSCREEN) ? SDL_DISABLE : SDL_ENABLE);
	
	GLInit(pd.width, pd.height);
	gl_init = 1;
	
	pd.audio_mutex = SDL_CreateMutex();
	pd.audio_cond = SDL_CreateCond();
	pd.audio_enable = 0;
	if (!pd.audio_mutex || !pd.audio_cond) {
		goto out;
	}
	
	memset(&desired, 0, sizeof(desired));
	desired.freq = pd.freq;
	desired.format = AUDIO_S8;
	desired.channels = 2;
	desired.samples = 1024;
	desired.callback = audio_callback;
	desired.userdata = &pd;
	if (SDL_OpenAudio(&desired, NULL) == -1) {
		fprintf(stderr, "Audio initialization failed: %s\n", SDL_GetError());
		goto out;
	}
	audio_init = 1;
	SDL_PauseAudio(1);
	
	gettimeofday(&pd.tv, NULL);
	do_cdxl_frame(&pd);
	start_audio(&pd);
	start_timer(&pd);
	while (!quit) {
		if (SDL_WaitEvent(&event)) {
			switch (event.type) {
				case SDL_USEREVENT:
					if (pd.status != PLAYING) {
						break;
					}
					free_cdxl_frame(pd.cdxl, pd.frame);
					t2 = pd.tv.tv_sec * 1000000ULL + pd.tv.tv_usec;
					gettimeofday(&pd.tv, NULL);
					t1 = pd.tv.tv_sec * 1000000ULL + pd.tv.tv_usec;
					t2 += get_frame_duration(&pd, pd.frame);
					if (t2 > t1) {
						usleep(t2 - t1);
					}
					pd.tv.tv_sec = t2 / 1000000ULL;
					pd.tv.tv_usec = t2 % 1000000ULL;
					pd.frame = get_next_frame(&pd.cdxl->list, pd.frame, pd.loop);
					if (!pd.frame) {
						pd.status = PAUSED;
						quit = 1;
						break;
					}
					do_cdxl_frame(&pd);
					if (pd.cdxl->has_audio) {
						pd.aframe = get_next_frame(&pd.cdxl->list, pd.aframe, pd.loop);
						if (!pd.aframe) {
							break;
						}
						do_cdxl_audio(&pd);
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							pd.status = PAUSED;
							quit = 1;
							break;
						case SDLK_SPACE:
							if (pd.status == PLAYING) {
								pd.status = PAUSED;
								stop_audio(&pd);
								stop_timer(&pd);
							} else {
								pd.status = PLAYING;
								gettimeofday(&pd.tv, NULL);
								do_cdxl_frame(&pd);
								start_audio(&pd);
								start_timer(&pd);
							}
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					pd.status = PAUSED;
					quit = 1;
					break;
				case SDL_VIDEORESIZE:
					GLExit();
					gl_init = 0;
					pd.width = MAX(event.resize.w, min_width);
					pd.height = MAX(event.resize.h, min_height);
					pd.screen = SDL_SetVideoMode(pd.width, pd.height, 32, pd.sdl_video_flags);
					if (!pd.screen) {
						fprintf(stderr, "Resize failed: %s\n", SDL_GetError());
						goto out;
					}
					GLInit(pd.width, pd.height);
					gl_init = 1;
					break;
			}
		}
	}
	
out:
	stop_timer(&pd);
	if (audio_init) {
		stop_audio(&pd);
		SDL_CloseAudio();
	}
	if (pd.audio_cond) SDL_DestroyCond(pd.audio_cond);
	if (pd.audio_mutex) SDL_DestroyMutex(pd.audio_mutex);
	if (gl_init) GLExit();
	if (pd.surf) SDL_FreeSurface(pd.surf);
	if (sdl_init) SDL_Quit();
	cleanup_usleep();
	if (pd.cdxl) close_cdxl(pd.cdxl);
	free_options();
	return 0;
}

uint32_t get_frame_duration (struct player_data *pd, struct cdxl_frame *frame) {
	uint32_t usec;
	if (!frame) {
		return 0;
	}
	if (frame->audio_size > 0) {
		uint32_t audio_size = frame->audio_size;
		if (frame->is_stereo) audio_size >>= 1;
		usec = audio_size * 1000000UL / pd->freq;
	} else {
		usec = 1000000UL / pd->fps;
	}
	return usec;
}

void start_timer(struct player_data *pd) {
	struct cdxl_frame *frame = pd->frame;
	uint32_t frame_ms;
	frame_ms = (get_frame_duration(pd, frame) / 10000) * 10;
	pd->timer_id = SDL_AddTimer(frame_ms, timer_callback, pd);
}

void start_audio(struct player_data *pd) {
	int i;
	if (!pd->cdxl->has_audio) {
		return;
	}
	pd->aframe = pd->frame;
	do_cdxl_audio(pd);
	for (i = 0; i < pd->aprebuf; i++) {
		pd->aframe = get_next_frame(&pd->cdxl->list, pd->aframe, pd->loop);
		if (!pd->aframe) {
			break;
		}
		do_cdxl_audio(pd);
	}
	pd->audio_enable = 1;
	SDL_PauseAudio(0);
}

void stop_timer(struct player_data *pd) {
	if (pd->timer_id) {
		SDL_RemoveTimer(pd->timer_id);
		pd->timer_id = 0;
	}
}

void stop_audio(struct player_data *pd) {
	if (!pd->cdxl->has_audio) {
		return;
	}
	if (pd->audio_enable) {
		struct audio_node *anode;
		SDL_LockMutex(pd->audio_mutex);
		while ((anode = (struct audio_node *)rem_head(&pd->audio_list))) {
			if (anode->pcm) free(anode->pcm);
			free(anode);
		}
		pd->audio_enable = 0;
		SDL_CondSignal(pd->audio_cond);
		SDL_UnlockMutex(pd->audio_mutex);
		SDL_PauseAudio(1);
	}
}

struct cdxl_frame *get_next_frame(struct list *list, struct cdxl_frame *frame, int loop) {
	if (!frame) {
		return NULL;
	}
	frame = (struct cdxl_frame *)frame->node.succ;
	if (frame->node.succ) {
		return frame;
	} else if (loop) {
		return (struct cdxl_frame *)list->head;
	} else {
		return NULL;
	}
}

void audio_callback (void *userdata, uint8_t *stream, int len) {
	struct player_data *pd = userdata;
	struct audio_node *anode, *next;
	const int8_t *source;
	int8_t *dest = stream;
	int alen;
	if (len <= 0) {
		return;
	}
	for (;;) {
		if (!pd->audio_enable) {
			memset(dest, 0, len);
			return;
		}
		SDL_LockMutex(pd->audio_mutex);
		anode = (struct audio_node *)pd->audio_list.head;
		while (anode->node.succ) {
			next = (struct audio_node *)anode->node.succ;
		
			source = anode->pcm + anode->apos;
			alen = anode->alen - anode->apos;
			if (alen > len) alen = len;
			if (anode->pcm)
				memcpy(dest, source, alen);
			else
				memset(dest, 0, alen);
			dest += alen;
			anode->apos += alen;
			len -= alen;
			if (anode->apos == anode->alen) {
				rem_node(&anode->node);
				if (anode->pcm) free(anode->pcm);
				free(anode);
			}
			if (len == 0) {
				SDL_UnlockMutex(pd->audio_mutex);
				return;
			}
		
			anode = next;
		}
		SDL_CondWait(pd->audio_cond, pd->audio_mutex);
		SDL_UnlockMutex(pd->audio_mutex);
	}
}

uint32_t timer_callback (uint32_t interval, void *param) {
	struct player_data *pd = param;
	struct cdxl_frame *frame;
	uint32_t frame_ms;
	SDL_Event event;
	
	frame = get_next_frame(&pd->cdxl->list, pd->frame, pd->loop);
	frame_ms = (get_frame_duration(pd, frame) / 10000) * 10;
	
	event.type = SDL_USEREVENT;
	event.user.code = 0;
	event.user.data1 = NULL;
	event.user.data2 = NULL;
	SDL_PushEvent(&event);
	
	return frame_ms;
}

int do_cdxl_frame (struct player_data *pd) {
	struct cdxl_frame *frame = pd->frame;
	if (read_cdxl_frame(pd->cdxl, frame) == -1) {
		return -1;
	}
	if (decode_cdxl_frame(pd->cdxl, frame, pd->surf) == -1) {
		return -1;
	}
	GLScaleSurface(pd->surf, pd->screen, pd->pixel_ratio);
	SDL_GL_SwapBuffers();
	return 0;
}

int do_cdxl_audio (struct player_data *pd) {
	struct cdxl_frame *frame = pd->aframe;
	if (!pd->cdxl->has_audio) {
		return -1;
	}
	if (read_cdxl_frame(pd->cdxl, frame) == -1) {
		return -1;
	}
	if (frame->audio_size > 0) {
		struct audio_node *anode;
		anode = malloc(sizeof(*anode));
		if (!anode) {
			return -1;
		}
		if (decode_cdxl_audio(pd->cdxl, frame, &anode->pcm, &anode->alen)) {
			free(anode);
			return -1;
		}
		anode->frame = frame->index;
		anode->apos = 0;
		SDL_LockMutex(pd->audio_mutex);
		add_tail(&pd->audio_list, &anode->node);
		SDL_CondSignal(pd->audio_cond);
		SDL_UnlockMutex(pd->audio_mutex);
		return 0;
	} else {
		struct audio_node *anode;
		anode = malloc(sizeof(*anode));
		if (!anode) {
			return -1;
		}
		anode->pcm = NULL;
		anode->alen = pd->freq / pd->fps;
		if (frame->is_stereo) {
			anode->alen <<= 1;
		}
		SDL_LockMutex(pd->audio_mutex);
		add_tail(&pd->audio_list, &anode->node);
		SDL_CondSignal(pd->audio_cond);
		SDL_UnlockMutex(pd->audio_mutex);
		return 0;
	}
}

int decode_cdxl_audio (struct cdxl_file *cdxl, struct cdxl_frame *frame, int8_t **pcm, int *alen) {
	if (cdxl && frame && frame->buffer) {
		int len, i, samples;
		const int8_t *src_l = (int8_t *)frame->buffer + frame->cmap_size + frame->video_size;
		const int8_t *src_r = src_l;
		int8_t *dest;
		len = samples = frame->audio_size;
		if (frame->is_stereo) {
			len &= ~1;
			samples >>= 1;
			src_r += samples;
		} else {
			len <<= 1;
		}
		if (len <= 0) {
			return -1;
		}
		dest = malloc(len);
		if (!dest) {
			return -1;
		}
		*pcm = dest;
		*alen = len;
		for (i = 0; i < samples; i++) {
			*dest++ = *src_l++;
			*dest++ = *src_r++;
		}
		return 0;
	}
	return -1;
}
