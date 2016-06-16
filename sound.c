#include "sound.h"

#ifndef MUTE

#include <portaudio.h>
#include <sndfile.h>
#include <stdlib.h>

static PaStream *stream = NULL;
static float *sound = NULL;
static int no_frames = 0;
static int curr_frame_i = 0;

static int cb(const void *in, void *out, unsigned long fpb,
              const PaStreamCallbackTimeInfo *time_info,
              PaStreamCallbackFlags status, void *user_data)
{
    int i;
    float *output;

    (void) in;
    (void) fpb;
    (void) time_info;
    (void) status;
    (void) user_data;
    output = (float *) out;

    for (i = 0; i < BUFFER_SIZE; ++i)
    {
        *output++ = sound[curr_frame_i];
        *output++ = sound[curr_frame_i];
        curr_frame_i += 1;
        curr_frame_i %= no_frames;
    }

    return paContinue;
}

void init_sound_system(void)
{
    (void) Pa_Initialize();
    (void) Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, BUFFER_SIZE, cb, NULL);
}

void init_sound(const char *addr)
{
    int i;
    SNDFILE *sf = NULL;
    SF_INFO si;
    float *sound_tmp = NULL;
    sf = sf_open(addr, SFM_READ, &si);

    no_frames = si.frames;
    sound = malloc(no_frames * si.channels * sizeof(*sound));
    (void) sf_readf_float(sf, sound, no_frames);
    for (i = 0; i < no_frames; ++i) sound[i] = sound[i * si.channels];
    sound_tmp = realloc(sound, no_frames * sizeof(*sound));
    sound = sound_tmp ? sound_tmp : sound;

    (void) sf_close(sf);
}

void play_sound(void)
{
    curr_frame_i = 0;
    (void) Pa_StartStream(stream);
}

void stop_sound(void)
{
    (void) Pa_StopStream(stream);
}

void exit_sound(void)
{
    free(sound);
}

void exit_sound_system(void)
{
    (void) Pa_CloseStream(stream);
    Pa_Terminate();
}

#else

void init_sound_system(void) {}
void init_sound(const char *addr) {}
void play_sound(void) {}
void stop_sound(void) {}
void exit_sound(void) {}
void exit_sound_system(void) {}

#endif
