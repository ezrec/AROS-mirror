#include "externs.h"
#include "freq.h"
#include <exec/types.h>

int audio_sample_rate, audio_channels = -1;
int frame_width, frame_height, use_audio = 0;
int verbose = 0, debugging = 0;
int benchmark_mode = 0;
double frame_rate = 0.0;

LONG __stack = 600000;  /* Make it big!! */

void show_formats(void)
{
    AVInputFormat *ifmt;
    AVCodec *p;

    printf("Input audio/video file formats:");
    for(ifmt = first_iformat; ifmt != NULL; ifmt = ifmt->next) {
        printf(" %s", ifmt->name);
    }
    printf("\n\n");
    
    printf("  Decoders:");
    for(p = first_avcodec; p != NULL; p = p->next) {
        if (p->decode)
            printf(" %s", p->name);
    }

    printf("\n");
}

#include "getopt.h"

#if defined(AMIGA) || defined(AROS)
#include <proto/dos.h>
BPTR lock = NULL, oldlock;

void amigaquit()
{
    if (lock) {
        CurrentDir(oldlock);
        UnLock(lock);
    }
}
#endif

int main(int argc, char *argv[])
{
    int i;
    char *filename;
    AVFormatContext *ic;
    AVInputFormat *file_iformat = NULL;
    AVFormatParameters params;
    char *c;
    
    av_register_all();

    while ((i = getopt(argc, argv, "hbBndvi")) != EOF) {
        switch(i) {
            case 'h':
                printf("Usage: %s [options] [filename]\n"
                        "-h\tThis help.\n"
                        "-n\tDisable audio.\n"
                        "-b\tBenchmark mode (no display, no audio)\n"
                        "-B\tBenchmark mode (no audio)\n"
                        "-v\tVerbose mode\n"
                        "-i\tShow supported formats and exits\n"
                        "-d\tDelay initialization for debugging purposes\n"
                        , argv[0]);

                return 0;
            case 'v':
                verbose = 1;
                break;
            case 'b':
                benchmark_mode = 2;
                break;
            case 'B':
                benchmark_mode = 1;
                break;
            case 'n':
                use_audio = -1;
                break;
            case 'i':
                show_formats();
                return 0;
            case 'd':
                debugging = 1;
                break;
            default:
                printf("Unknown option %c\n", i);
                return 0;
        }
    }

#ifdef AROS
    use_audio = -1; // XXX to fix once we have a working AHI
#endif
    
    if((argc-optind) != 1) {
        FRequest f;
    
#ifdef linux
        gtk_init(&argc, &argv);
#endif

        f.Title = "Select a movie to play...";
        f.Dir = NULL;
       	f.Filter = "All supported files|*.mpg;*.avi;*.mov;*.qt;*.asf;*.mpeg|MPEG Files|*.mpg;*.mpeg|AVI Files|*.avi|Quicktime Files|*.qt;*.mov|All files|*.*\0\0";
        f.Save = 0;
        
        if(Request(&f)) {
// the following code is due to the fact avformat has a few problems with Amigalike paths...
#if defined(AMIGA) || defined(AROS)
            filename = FilePart(f.File);
            if (strcmp(filename, f.File)) {
                lock = Lock(PathPart(f.File), ACCESS_READ);

                if (lock) {
                    oldlock = CurrentDir(lock);
                    atexit(amigaquit);
                }
            }
#else
            filename = f.File;
#endif
        }
        else
            return 0;
    }
    else
        filename = argv[optind];
        
    if((c = strrchr(filename, '.'))) {
        file_iformat = av_find_input_format(c + 1);

        printf("First guess of fileformat: %s\n", 
                file_iformat ? file_iformat->name : "UNKNOWN");
    }
    
    memset(&params, 0, sizeof(params));

    if(av_open_input_file(&ic, filename, NULL /*file_iformat*/, 0, &params) < 0) {
        fprintf(stderr, "Unable to find format of %s\n", filename);
        return 0;
    }

    if(av_find_stream_info(ic) < 0) {
        fprintf(stderr, "%s: could not find codec parameters\n", filename);
        return 0;
    }

    
    dump_format(ic, 0, filename, 0);

    for(i=0;i<ic->nb_streams;i++) {
        AVCodecContext *enc = &ic->streams[i]->codec;
        switch(enc->codec_type) {
            case CODEC_TYPE_AUDIO:
                if(use_audio != -1) {
                    use_audio = 1;
                    audio_channels = enc->channels;
                    audio_sample_rate = enc->sample_rate;

                    if(verbose)
                        fprintf(stderr, "audio: %d/%d\n",
                                enc->sample_rate, enc->channels);
                }

                break;
            case CODEC_TYPE_VIDEO:
                frame_height = enc->height;
                frame_width = enc->width;
                frame_rate = (double)ic->streams[i]->r_frame_rate / (double)ic->streams[i]->r_frame_rate_base;
#if 0            
                enc->workaround_bugs = FF_BUG_AUTODETECT;
                enc->error_resilience = 2; 
                enc->error_concealment = 3; 
                enc->idct_algo= 0;
#endif
                if(verbose)
                    fprintf(stderr, "video: %dx%d %f fps\n",
                            frame_width, frame_height, frame_rate);
                break;
        }

    }

   if(!(c = strrchr(filename, 
#ifdef WIN32
            '\\'
#else
            '/'
#endif
                   )))
        c = filename;
    else
        c++;
   
    if(benchmark_mode || use_audio == -1)
        use_audio = 0;
   
    if(verbose) {
        fprintf(stderr, "Initializing the audio/video section...\n");

        if(debugging)
            os_delay(2000);
    }
    
    init_system(c);
    
    if(verbose) {
        fprintf(stderr, "Start decoding %s (%lx/%d)...\n", c, ic, ic->nb_streams);

        if(debugging)
            os_delay(2000);
    }
    play_decode_stream(ic);
}

