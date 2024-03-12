/**
 * @file ul_sound.h
 * @author Piotr "UjemnyGH" Plombon
 * @brief 
 * @version 0.1
 * @date 2024-03-10
 *
 * @copyright Copyleft (c) 2024
 * 
 * 
 * This library wraps audio APIs on diffrent OS, but currently use onlu ALSA on linux
 * 
 * Have fun!
 */

// TODO: Windows handler

#ifndef _UL_SOUND_
#define _UL_SOUND_

#if defined(__linux)

#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct ul_sound_s {
    // snd_pcm_t = void
    void *handle;
    // snd_pcm_sframes_t = long
    long frame;
    char device[1024];
    int format;
    int access;
    uint32_t channels;
    uint32_t rate;
    int softResample;
    uint32_t latency;
} ul_sound_t;

#define DEVICE_DEFAULT                  "default"

#define PLAYBACK                        SND_PCM_STREAM_PLAYBACK
#define CAPTURE                         SND_PCM_STREAM_CAPTURE

#define LATENCY_FULL                    1000000
#define LATENCY_HALF                    500000
#define LATENCY_QUARTER                 250000

#define RATE_48kHz                      48000
#define RATE_44_1kHz                    44100
#define RATE_32kHz                      32000

#define FORMAT_S8                       SND_PCM_FORMAT_S8
#define FORMAT_U8                       SND_PCM_FORMAT_U8
#define FORMAT_S16_LE                   SND_PCM_FORMAT_S16_LE
#define FORMAT_S16_BE                   SND_PCM_FORMAT_S16_BE
#define FORMAT_U16_LE                   SND_PCM_FORMAT_U16_LE
#define FORMAT_U16_BE                   SND_PCM_FORMAT_U16_BE
#define FORMAT_S24_LE                   SND_PCM_FORMAT_S24_LE
#define FORMAT_S24_BE                   SND_PCM_FORMAT_S24_BE
#define FORMAT_U24_LE                   SND_PCM_FORMAT_U24_LE
#define FORMAT_U24_BE                   SND_PCM_FORMAT_U24_BE
#define FORMAT_S32_LE                   SND_PCM_FORMAT_S32_LE
#define FORMAT_S32_BE                   SND_PCM_FORMAT_S32_BE
#define FORMAT_U32_LE                   SND_PCM_FORMAT_U32_LE
#define FORMAT_U32_BE                   SND_PCM_FORMAT_U32_BE
#define FORMAT_FLOAT_LE                 SND_PCM_FORMAT_FLOAT_LE
#define FORMAT_FLOAT_BE                 SND_PCM_FORMAT_FLOAT_BE
#define FORMAT_DOUBLE_LE                SND_PCM_FORMAT_FLOAT64_LE
#define FORMAT_DOUBLE_BE                SND_PCM_FORMAT_FLOAT64_BE

#define ACCESS_MMAP_INTER               SND_PCM_ACCESS_MMAP_INTERLEAVED
#define ACCESS_MMAP_NONINTERLEAVED      SND_PCM_ACCESS_MMAP_NONINTERLEAVED
#define ACCESS_MMAP_COMPLEX             SND_PCM_ACCESS_MMAP_COMPLEX
#define ACCESS_RW_INTERLEAVED           SND_PCM_ACCESS_RW_INTERLEAVED
#define ACCESS_RW_NONINTERLEAVED        SND_PCM_ACCESS_RW_NONINTERLEAVED

void ulSoundOpenHandle(ul_sound_t* pSound, const char* device, int capture) {
    strcpy(pSound->device, device);

    int err = 0;

    if((err = snd_pcm_open((snd_pcm_t**)&pSound->handle, pSound->device, (snd_pcm_stream_t)capture, 0)) < 0) {
        printf("%s open error: %s\n", capture == CAPTURE ? "Capture" : "Playback", snd_strerror(err));
        exit(1);
    }
}

void ulSoundSetParameters(ul_sound_t* pSound, int format, int access, uint32_t channels, uint32_t rate, int softResample, uint32_t latency) {
    pSound->format = format;
    pSound->access = access;
    pSound->channels = channels;
    pSound->rate = rate;
    pSound->softResample = softResample;
    pSound->latency = latency;
    
    int err = 0;

    if((err = snd_pcm_set_params((snd_pcm_t*)pSound->handle, (snd_pcm_format_t)pSound->format, (snd_pcm_access_t)pSound->access, pSound->channels, pSound->rate, pSound->softResample, pSound->latency)) < 0) {
        printf("Parameters error: %s\n", snd_strerror(err));
        exit(1);
    }
}

void ulSoundWriteFrames(ul_sound_t* pSound, void* buffer, uint64_t size) {
    pSound->frame = snd_pcm_writei((snd_pcm_t*)pSound->handle, buffer, size);
}

int ulSoundRecoverFrames(ul_sound_t* pSound, int silent) {
    pSound->frame = snd_pcm_recover((snd_pcm_t*)pSound->handle, pSound->frame, silent);

    if(pSound->frame < 0) {
        printf("snd_pcm_recover failed: %s\n", snd_strerror(pSound->frame));

        return 0;
    }

    return 1;
}

int ulSoundPlayBuffer(ul_sound_t* pSound, void* buffer, uint64_t size, int silent) {
    ulSoundWriteFrames(pSound, buffer, size);

    if(pSound->frame < 0) {
        return ulSoundRecoverFrames(pSound, silent);
    }

    return 1;
}

void ulSoundDrain(ul_sound_t* pSound) {
    int err = 0;
    if((err = snd_pcm_drain((snd_pcm_t*)pSound->handle)) < 0) printf("snd_pcm_drain() failed: %s\n", snd_strerror(err));
}

void ulSoundClose(ul_sound_t* pSound) {
    ulSoundDrain(pSound);

    snd_pcm_close((snd_pcm_t*)pSound->handle);
}

#elif defined(_WIN32)

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiosessiontypes.h>
#include <audiopolicy.h>

#include <endpointvolume.h>
#include <mmreg.h>
#include <functiondiscoverykeys_devpkey.h>

#endif

#endif