/*
   2004/09/10 - Joseph Fenton, changed to use calculated
      samples per frame and to decode audio-only streams.
*/

#include "externs.h"

static unsigned long samples_per_frame;
unsigned long sample_frame = 0;
static int frames = 0;

#define NUM_BUFFERS 32

AVPicture picture_buffer[NUM_BUFFERS];
int pictures_in_buffer = 0;


void print_benchmark_result(void)
{
    sample_frame = get_elapsed_samples() - sample_frame;

    fprintf(stderr, 
            "\nBenchmark finished: %d frames in %f seconds -> %ffps\n\n",
            frames, ((float)sample_frame) / 1000.0,
            ((float)frames) * 1000 / ((float)sample_frame)
           );
}

void add_picture(AVPicture *p)
{
    if(pictures_in_buffer == NUM_BUFFERS) {
        fprintf(stderr, "Buffer immagini pieno!\n");
        return;
    }

    if(picture_buffer[pictures_in_buffer].data[0] == NULL) {
        char *c = malloc(p->linesize[0] * frame_height + 
                p->linesize[1] * frame_height);

        picture_buffer[pictures_in_buffer].data[0] = c;
        picture_buffer[pictures_in_buffer].data[1] = c + p->linesize[0] * frame_height;
        picture_buffer[pictures_in_buffer].data[2] = 
            picture_buffer[pictures_in_buffer].data[1] + p->linesize[1] * frame_height / 2;

        picture_buffer[pictures_in_buffer].linesize[0] = p->linesize[0];
        picture_buffer[pictures_in_buffer].linesize[1] = p->linesize[1];
        picture_buffer[pictures_in_buffer].linesize[2] = p->linesize[2];
    }
    
    memcpy(picture_buffer[pictures_in_buffer].data[0], p->data[0], 
            p->linesize[0] * frame_height);
    memcpy(picture_buffer[pictures_in_buffer].data[1], p->data[1],
            p->linesize[1] * frame_height / 2);
    memcpy(picture_buffer[pictures_in_buffer].data[2], p->data[2], 
            p->linesize[2] * frame_height / 2);

    pictures_in_buffer++;
}

void do_frame_skip(void)
{
    char *d[3];
    int i,j;

    if(pictures_in_buffer == 0) {
        return;
    }

    for(i = 0;i < 3; i++)
        d[i] = picture_buffer[0].data[i];
    
    pictures_in_buffer --;

    for(i = 0; i< pictures_in_buffer;i++) {
        for(j = 0; j < 3; j++) {
            picture_buffer[i].data[j] = picture_buffer[i+1].data[j];
        }
    }

    for(i = 0;i < 3; i++)
        picture_buffer[pictures_in_buffer].data[i] = d[i];
}

void display_picture_new(void)
{
    char *d[3];
    int i,j;

    if(pictures_in_buffer == 0) {
        fprintf(stderr, "Tentato play con buffer immagini vuoto!\n");
        return;
    }

    for(i = 0;i < 3; i++)
        d[i] = picture_buffer[0].data[i];
    
    display_picture(picture_buffer);

    pictures_in_buffer --;

    for(i = 0; i< pictures_in_buffer;i++) {
        for(j = 0; j < 3; j++) {
            picture_buffer[i].data[j] = picture_buffer[i+1].data[j];
        }
    }

    for(i = 0;i < 3; i++)
        picture_buffer[pictures_in_buffer].data[i] = d[i];
}


int play_decode_stream(AVFormatContext *is)
{
    int ret, i;
    int delays = 0, skips = 0;
    int audio_started = 0;
    int audiobytes_per_frame;

    if(verbose)
        fprintf(stderr, "Cleaning buffers...\n");

    if(debugging)
        os_delay(2000);
    
    for(i = 0;i < NUM_BUFFERS;i++) {
        picture_buffer[i].data[0] = NULL;
    }
    
    if(use_audio) {
        samples_per_frame = (unsigned long) ((double)audio_sample_rate / frame_rate + 0.5);
        audiobytes_per_frame = samples_per_frame * audio_channels * 2;

        printf("Opened Audio %d/%d/16bit (%ffps, %lu samples per frame).\n",
                audio_sample_rate, audio_channels,
                frame_rate, samples_per_frame);
    } else if(!benchmark_mode) {
       samples_per_frame = (int) (1000.0f / frame_rate);
       audio_sample_rate = frame_rate * samples_per_frame;
       audiobytes_per_frame = -1;
       
       fprintf(stderr, "Playing a frame every %lu msec\n", samples_per_frame);
    } else {
       fprintf(stderr, "*** BENCHMARK MODE *** (press Q to end)\n");
    }

    if(debugging)
        os_delay(2000);

    /* open each decoder */
    for(i=0;i<is->nb_streams;i++) {
         AVCodec *codec;
        codec = avcodec_find_decoder(is->streams[i]->codec.codec_id);
        if (!codec) {
            fprintf(stderr, "Unsupported codec (id=%d) for input stream %d\n", 
                    is->streams[i]->codec.codec_id, i);
            exit(1);
        }
        if (avcodec_open(&is->streams[i]->codec, codec) < 0) {
            fprintf(stderr, "Error while opening codec for input stream %d\n", i);
            exit(1);
        }

        if(debugging)
            fprintf(stderr, "Opened codec: id %d, subid: %d extradata: %lx extradata_size: %ld\n", 
                    is->streams[i]->codec.codec_id, is->streams[i]->codec.sub_id,
                    is->streams[i]->codec.extradata, is->streams[i]->codec.extradata_size);
    }
    if(debugging)
        os_delay(2000);

    sample_frame = get_elapsed_samples();
       
    for(;;) {
        AVPacket pkt;
        uint8_t *ptr;
        int len, index;
        int data_size, got_picture;
        short samples[AVCODEC_MAX_AUDIO_FRAME_SIZE / 2];

redo:
        handle_events();
        
        if (av_read_packet(is, &pkt) < 0) {
            break; // fine file
        }

        if (!pkt.size) {
            continue;
        }

        if(verbose && debugging)
            printf("stream #%d, size=%d \n", 
                    pkt.stream_index, pkt.size);

#ifdef notdef
        if(pkt.stream_index == 0 && frames == 0) {
            FILE *f = fopen("/tmp/frame.000", "wb");

            fwrite(pkt.data, 1, pkt.size, f);

            frames++;

            fclose(f);
        }
#endif
        len = pkt.size;
        ptr = pkt.data;
        index = pkt.stream_index;

        if(!benchmark_mode) {
            while(((pictures_in_buffer > 1) || no_video) &&
                    bytes_in_buffer > (audiobytes_per_frame * 2)) {

                if(!audio_started) {

                    fprintf(stderr, "Avvio audio: elapsed: %d inbuf:%d\n\n", get_elapsed_samples(),
                            bytes_in_buffer);

                    start_audio();

                    audio_started = 1;
                }

                sample_frame += samples_per_frame;

                if(get_elapsed_samples() > samples_per_frame && 
                   sample_frame < (get_elapsed_samples() - samples_per_frame) && !no_video) {
                    skips++;
                    do_frame_skip();
                } else {
                    while(sample_frame > get_elapsed_samples()) {
                        delays ++;
                        os_delay(1);
                    }

                    if (!no_video)
                        display_picture_new();
                }

                frames++;

//                fprintf(stderr, "%lu %lu - ", sample_frame, get_elapsed_samples());
                
                if((frames % 25) == 1)
                    fprintf(stderr, "FRM:%d VBUF:%d ABUF:%d PLD:%lu Wait:%d Skip:%d FPS:%f\r",
                            frames, pictures_in_buffer, bytes_in_buffer,
                            get_elapsed_samples(), delays, skips,

                            ( (double)frames * (double)audio_sample_rate /(double)get_elapsed_samples() ) );

            }
        }
        
        while (len > 0) {
            
            /* decode the packet if needed */
            data_size = 0;

            switch(is->streams[index]->codec.codec_type) {
                case CODEC_TYPE_AUDIO:
                    if(!use_audio || benchmark_mode)
                        goto discard_packet;

                    /* XXX: could avoid copy if PCM 16 bits with same
                       endianness as CPU */
                    ret = avcodec_decode_audio(&is->streams[index]->codec, samples, &data_size,
                            ptr, len);
                    if (ret < 0)
                        goto fail_decode;
                    /* Some bug in mpeg audio decoder gives */
                    /* data_size < 0, it seems they are overflows */
                    if (data_size <= 0) {
                        /* no audio frame */
                        ptr += ret;
                        len -= ret;
                        continue;
                    }

                    write_audio((unsigned char *)samples, data_size);
                           
                    break;
                case CODEC_TYPE_VIDEO:
                    {
                        AVFrame big_picture;

                        data_size = (is->streams[index]->codec.width * 
                                is->streams[index]->codec.height * 3) / 2;
                        ret = avcodec_decode_video(&is->streams[index]->codec, 
                                &big_picture, &got_picture, ptr, len);

                        if (ret < 0) {
fail_decode:
                            fprintf(stderr, "Error while decoding stream %d\n",
                                    index);
                            av_free_packet(&pkt);
                            goto redo;
                        }
                        if (!got_picture) {
                            /* no picture yet */
                            ptr += ret;
                            len -= ret;
                            continue;
                        }
                        
                        if(!benchmark_mode)
                            add_picture((AVPicture *) &big_picture);
                        else {
                            frames++;
                            
                            if(benchmark_mode == 1) 
                                display_picture((AVPicture *) &big_picture);
                        }
                            
                    }
                    break;
                default:
                    goto fail_decode;
            }
            ptr += ret;
            len -= ret;

        }
discard_packet:
        av_free_packet(&pkt);
    }

    if(benchmark_mode)
       print_benchmark_result();
    
    /* close each decoder */
    for(i=0;i<is->nb_streams;i++) {
        avcodec_close(&is->streams[i]->codec);
    }
        
    return 0;
}

