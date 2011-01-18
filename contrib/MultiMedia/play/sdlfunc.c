/*
   2004/09/10 - Joseph Fenton, added calculation of
      samples per frame.
*/

#include "externs.h"

static SDL_Surface *screen;
static SDL_Overlay *yuv;
static SDL_Rect rect;

int silence, bytes_in_buffer = 0;

static unsigned char *audioport_buffer = NULL;
static volatile unsigned long samples_played = 0;

extern int use_audio, audio_sample_rate, audio_channels, frame_width, frame_height;

#ifndef min
#define min(a,b) ((a) > (b) ? (b) : (a))
#endif

void os_delay(int msec)
{
    SDL_Delay(msec);
}

static unsigned long starttick = 0;

unsigned long get_elapsed_samples(void)
{
    if(use_audio)
        return samples_played;
    else
        return (SDL_GetTicks() - starttick);
}

void start_audio(void)
{
    if(use_audio)
        SDL_PauseAudio(0);
}

void stop_audio(void)
{
    if(use_audio)
        SDL_PauseAudio(1);
}

int write_audio(char *buffer, int len)
{
	if( (bytes_in_buffer+len) <= AUDIOPORT_BUFFERSIZE) {
		SDL_LockAudio();

		memcpy(audioport_buffer+bytes_in_buffer, buffer, len);

                bytes_in_buffer += len;

		SDL_UnlockAudio();

		return len;
	}
	else if (bytes_in_buffer < AUDIOPORT_BUFFERSIZE) {
	    // XXX controllare questo codice, non mi convince
            fprintf(stderr, "Written in buffer %d bytes, trashed remaining %d bytes\n",
                AUDIOPORT_BUFFERSIZE - bytes_in_buffer,
                len -  (AUDIOPORT_BUFFERSIZE - bytes_in_buffer) );
		SDL_LockAudio();
        
		memcpy(audioport_buffer+bytes_in_buffer,
                buffer, AUDIOPORT_BUFFERSIZE - bytes_in_buffer);
        
		bytes_in_buffer = AUDIOPORT_BUFFERSIZE;
		SDL_UnlockAudio();

		return AUDIOPORT_BUFFERSIZE - bytes_in_buffer;
	}

        fprintf(stderr, "Audio port BUFFER full!!!!\n");

	return -1;
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
	int amount;
    
	if (bytes_in_buffer > 0) {

		if(bytes_in_buffer>len)
			amount = len;
		else
			amount = bytes_in_buffer;

		SDL_MixAudio(stream, audioport_buffer, amount,
			SDL_MIX_MAXVOLUME);

                samples_played += ((amount/audio_channels)/2);

//		memcpy(stream, audioport_buffer, amount);

		if(amount < bytes_in_buffer) {
			bytes_in_buffer -= amount;
			memcpy(audioport_buffer, audioport_buffer + amount, bytes_in_buffer);
		} else {
                    if(amount < len) {
                        memset(stream+amount, silence, len - amount);
	            }
		    bytes_in_buffer = 0;
		}

	} else {
            memset(stream, silence, len);
	}

}

void init_system(char *title)
{
    int sdlflags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;

    if(use_audio)
        sdlflags |= SDL_INIT_AUDIO;
  
    if(verbose)
        fprintf(stderr, "SDL initialization...\n");

    if(debugging)
        SDL_Delay(2000);

    if(SDL_Init(sdlflags) < 0) {
        fprintf(stderr, "Unable to open SDL.");
        exit(-1);
    }

    atexit(SDL_Quit);
    
    if(verbose)
        fprintf(stderr, "   Open the window %d,%d\n", frame_width, frame_height);

    if(debugging)
        SDL_Delay(2000);

    if(!(screen = SDL_SetVideoMode(frame_width, frame_height, 0, SDL_RESIZABLE))) {
        fprintf(stderr, "Unable to open SDL window: %s.\n", SDL_GetError());
        exit(-2);
    }

    if(verbose)
        fprintf(stderr, "   Overlay creation....");

    if(debugging)
        SDL_Delay(2000);

    if ( !(yuv = SDL_CreateYUVOverlay(frame_width, frame_height,
                    SDL_YV12_OVERLAY, screen)) ) {
		fprintf(stderr, "Couldn't create overlay: %s\n", SDL_GetError());
		exit(-3);
	}
    
	printf("   Created %dx%dx%d %s %s overlay\n",yuv->w,yuv->h,yuv->planes,
			yuv->hw_overlay?"hardware":"software",
			yuv->format==SDL_YV12_OVERLAY?"YV12":
			yuv->format==SDL_IYUV_OVERLAY?"IYUV":
			yuv->format==SDL_YUY2_OVERLAY?"YUY2":
			yuv->format==SDL_UYVY_OVERLAY?"UYVY":
			yuv->format==SDL_YVYU_OVERLAY?"YVYU":
			"Unknown");

    if(debugging)
        SDL_Delay(2000);
    
    if(use_audio) {
        SDL_AudioSpec desired;
        unsigned long samples_per_frame;

        if(verbose)
            fprintf(stderr, "   Opening audio...\n");
        
        if(debugging)
            SDL_Delay(2000);

        samples_per_frame = (unsigned long) ((double)audio_sample_rate / frame_rate + 0.5);
        desired.freq = audio_sample_rate;
        desired.channels = audio_channels;
        desired.samples = samples_per_frame;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        desired.format = AUDIO_S16LSB;
#else
        desired.format = AUDIO_S16MSB;
#endif

        desired.callback = my_audio_callback;
       
        if(SDL_OpenAudio(&desired, NULL) < 0) {
            fprintf(stderr, "Unable to open audio: %s.\n", SDL_GetError());
            exit(-4);
        }

        if(debugging)
           SDL_Delay(2000);

        if(!(audioport_buffer = malloc(AUDIOPORT_BUFFERSIZE))) {
            fprintf(stderr, "No memory for the audio buffer!\n");
            exit(-5);
        }
       
        
        use_audio = 1;
        
        silence = desired.silence;

        SDL_PauseAudio(1);
    }

    starttick = SDL_GetTicks();
    
    SDL_WM_SetCaption(title, NULL);

    rect.x = 0;
    rect.y = 0;
    rect.w = screen->w;
    rect.h = screen->h;

    if(debugging)
        SDL_Delay(2000);

    if(verbose)
        fprintf(stderr, "SDL initialized.\n");

    if(debugging)
        SDL_Delay(2000);
}

void handle_events(void)
{
    SDL_Event event;
    static int fullscreen = 0;
    static int paused = 0;
    
    do {
        if(paused)
            SDL_WaitEvent(NULL);
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
#ifdef WIN32
                // per qualche strano motivo win genera questo evento quasi a raffica...
                case 204:
                    break;
#endif
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_KEYUP:
                case SDL_MOUSEMOTION:
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_q:
terminate_app:
                            if(benchmark_mode)
                               print_benchmark_result();
                            else
                               fprintf(stderr, "\nTerminated by user request.\n");
                            exit(0);
                        case SDLK_p:
                            paused ^= 1;

                            if(paused) {
                                if(verbose)
                                    fprintf(stderr, "\nEntering pause mode.\n");
                                stop_audio();
                            }
                            else {
                                if(verbose)
                                    fprintf(stderr, "\nExiting pause mode.\n");
                                start_audio();
                            }
                            
                            sample_frame = get_elapsed_samples();
                                
                            break;
                        case SDLK_SPACE:
                            SDL_FreeYUVOverlay(yuv);

                            rect.x = 0;

                            if(fullscreen) {
                                if(verbose)
                                    fprintf(stderr, "\nEntering windowed mode...\n");
                                
                                screen = SDL_SetVideoMode(frame_width, frame_height,
                                        0 , SDL_RESIZABLE);
                                fullscreen = 0;

                                rect.y = 0;
                                rect.h = screen->h;
                            }
                            else {
                                if(verbose)
                                    fprintf(stderr, "\nEntering fullscreen mode..\n");

                                screen = SDL_SetVideoMode(800, 600,
                                        32 , SDL_FULLSCREEN|SDL_SWSURFACE);

                                fullscreen = 1;

                                rect.h = min(screen->h, (screen->w * frame_height)/frame_width);

                                if(rect.h < screen->h) {
                                    rect.y = (screen->h - rect.h) / 2;
                                }
                            }

                            rect.w=screen->w;

                            if(!screen) {
                                fprintf(stderr, "Impossible to switch to %s mode.\n",
                                        fullscreen ? "fullscreen" : "windowed");
                                exit(0);
                            }

                            if(!(yuv = SDL_CreateYUVOverlay(frame_width, frame_height, 
                                            SDL_YV12_OVERLAY, screen))) {
                                fprintf(stderr,  "Impossible to recreate video overlay!\n");
                                exit(0);
                            }
                               
                            if(verbose)
                                fprintf(stderr, "Setting rectangle to (%d,%d) %dx%d\n",
                                        rect.x, rect.y, rect.w, rect.h);
                            break;
                    }
                    break;
                case SDL_QUIT:
                    goto terminate_app;
                    
                case SDL_VIDEORESIZE:
                    SDL_FreeYUVOverlay(yuv);

                    screen=SDL_SetVideoMode(event.resize.w, event.resize.h,
                            0 , SDL_RESIZABLE);

                    if(!screen) {
                        fprintf(stderr, "Non riesco a ridimensionare la finestra!\n");
                        return;
                    }

                    if(!(yuv = SDL_CreateYUVOverlay(frame_width, frame_height, 
                                    SDL_YV12_OVERLAY, screen))) {
                        fprintf(stderr,  "Errore nell'inizializzazione dell'overlay!\n");
                        return;
                    }

                    rect.w=event.resize.w;
                    rect.h=event.resize.h;
                    break;
            }
        }
    } while(paused);
    
    return;
}

void display_picture(AVPicture *picture)
{
    if(yuv && !SDL_LockYUVOverlay(yuv) ) {

        if(yuv->pitches[0] == picture->linesize[0]) {
            int size = yuv->pitches[0] * yuv->h;

            memcpy(yuv->pixels[0], picture->data[0], size);
            size /= 4;
            memcpy(yuv->pixels[2], picture->data[1], size);
            memcpy(yuv->pixels[1], picture->data[2], size);
        }
        else {
            register unsigned char *y1,*y2,*y3,*i1,*i2,*i3;
            int i;
            y1 = yuv->pixels[0];
            y3 = yuv->pixels[1]; // invertiti xche' avevo i colori sballati!
            y2 = yuv->pixels[2];

            i1=picture->data[0];
            i2=picture->data[1];
            i3=picture->data[2];

            for (i = 0; i<(yuv->h/2); i++) {
                memcpy(y1,i1,yuv->pitches[0]);
                i1+=picture->linesize[0];
                y1+=yuv->pitches[0];
                memcpy(y1,i1,yuv->pitches[0]);

                memcpy(y2,i2,yuv->pitches[1]);
                memcpy(y3,i3,yuv->pitches[2]);

                y1+=yuv->pitches[0];
                y2+=yuv->pitches[1];
                y3+=yuv->pitches[2];
                i1+=picture->linesize[0];
                i2+=picture->linesize[1];
                i3+=picture->linesize[2];
            }
        }
        SDL_UnlockYUVOverlay(yuv);
    }

    SDL_DisplayYUVOverlay(yuv, &rect);
}

