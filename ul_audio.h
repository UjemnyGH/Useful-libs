/**
 * @file ul_audio.h
 * @author Piotr "UjemnyGH" Plombon
 * @brief Currently only wave files are handled
 * @version 0.1
 * @date 2024-03-10
 * 
 * @copyright Copyleft (c) 2024
 * 
 * To use this library just add #include "ul_audio.h" in your file
 * 
 * Below you have example program how to use this library among with sound library

#include "ul_audio.h"
#include "ul_sound.h"

int main(int argc, char **argv) {
    ul_sound_t sound;

    ulSoundOpenHandle(&sound, DEVICE_DEFAULT, PLAYBACK);

    ul_audio_wav_t wav;

    ulAudioLoadFromWAV(&wav, argv[1]);

    ulSoundSetParameters(&sound, wav.bitsPerSample == 8 ? FORMAT_U8 : FORMAT_S16_LE, ACCESS_RW_INTERLEAVED, wav.channels, wav.samplerate, 1, LATENCY_QUARTER);

    ulSoundPlayBuffer(&sound, wav.data, wav.subchunk2Size / 4, 0);

    ulAudioFreeWAV(&wav);

    ulSoundClose(&sound);

    return 0;
}

 *  Have fun!
 */

#ifndef _UL_AUDIO_
#define _UL_AUDIO_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MP3_LAYER_1 0x6000
#define MP3_LAYER_2 0x4000
#define MP3_LAYER_3 0x2000
#define MP3_VERSION 0x1000
#define MP3_FRAME_SYNC 0xfff
#define MP3_UNPROTECTED 0x8000

// These bitrates (in kbps) are for layer 3!
#define MP3_BITRATE_32 0x10000
#define MP3_BITRATE_40 0x20000
#define MP3_BITRATE_48 0x30000
#define MP3_BITRATE_56 0x40000
#define MP3_BITRATE_64 0x50000
#define MP3_BITRATE_80 0x60000
#define MP3_BITRATE_96 0x70000
#define MP3_BITRATE_112 0x80000
#define MP3_BITRATE_128 0x90000
#define MP3_BITRATE_160 0xa0000
#define MP3_BITRATE_192 0xb0000
#define MP3_BITRATE_224 0xc0000
#define MP3_BITRATE_256 0xd0000
#define MP3_BITRATE_320 0xe0000

#define MP3_FREQUENCY_44_1 0x0
#define MP3_FREQUENCY_48 0x100000
#define MP3_FREQUENCY_32 0x200000

#define MP3_MODE_MONO_SINGLE 0x1000000
#define MP3_MODE_MONO_DUAL 0x2000000
#define MP3_MODE_JOINT_STEREO 0x3000000
#define MP3_MODE_STEREO 0x0

#define MP3_INT_MS_STEREO_OFF_OFF 0x0
#define MP3_INT_MS_STEREO_ON_OFF 0x4000000
#define MP3_INT_MS_STEREO_OFF_ON 0x8000000
#define MP3_INT_MS_STEREO_ON_ON 0xc000000

#define MP3_COPYRIGHTED 0x10000000
#define MP3_ORIGINAL 0x20000000

#define MP3_NO_EMPHASIS 0x0

#define MP3_PADDING_SET 0x400000

typedef struct ul_audio_frame_mp3_s {
    uint32_t header;
    uint32_t sideInfo[8];
    char* mainData;
    char* helperData;

    size_t helperDataSize, mainDataSize;
} ul_audio_frame_mp3_t;

typedef struct ul_audio_mp3_s {
    ul_audio_frame_mp3_t* audioFrames;
    size_t framesCounter;
} ul_audio_mp3_t;

typedef struct ul_audio_wav_s {
    uint32_t subchunk1Id;
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t channels;
    uint32_t samplerate;
    uint32_t byterate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t subchunk2Id;
    uint32_t subchunk2Size;
    // Samples depends on bitsPerSample and channels
    uint8_t* data;
} ul_audio_wav_t;

/**
 * @brief Calculate framerate for layer 2 & 3
 * 
 * @param pFrame mp3 frame
 * @return uint32_t frame length in bytes
 */
uint32_t ulAudioCalculateMP3FrameLength(ul_audio_frame_mp3_t* pFrame) {
    double samplerate = 0.0, bitrate = 0.0;

    switch(pFrame->header & 0xf0000) {
        case MP3_BITRATE_32: { bitrate = 32.0; break; }
        case MP3_BITRATE_40: { bitrate = 40.0; break; }
        case MP3_BITRATE_48: { bitrate = 48.0; break; }
        case MP3_BITRATE_56: { bitrate = 56.0; break; }
        case MP3_BITRATE_64: { bitrate = 64.0; break; }
        case MP3_BITRATE_80: { bitrate = 80.0; break; }
        case MP3_BITRATE_96: { bitrate = 96.0; break; }
        case MP3_BITRATE_112: { bitrate = 112.0; break; }
        case MP3_BITRATE_128: { bitrate = 128.0; break; }
        case MP3_BITRATE_160: { bitrate = 160.0; break; }
        case MP3_BITRATE_192: { bitrate = 192.0; break; }
        case MP3_BITRATE_224: { bitrate = 224.0; break; }
        case MP3_BITRATE_256: { bitrate = 256.0; break; }
        case MP3_BITRATE_320: { bitrate = 320.0; break; }
    }

    switch(pFrame->header & 0x300000) {
        case MP3_FREQUENCY_44_1: { samplerate = 44.1; break; }
        case MP3_FREQUENCY_48: { samplerate = 48.0; break; }
        case MP3_FREQUENCY_32: { samplerate = 32.0; break; }
    }

    uint32_t length = (uint32_t)(144.0 * (bitrate / samplerate));

    if(pFrame->header & MP3_PADDING_SET) {
        length++;
    }

    return length;
}

// TODO: MP3

/*
1152 samples per frame
*/
int ulAudioLoadFromMP3(ul_audio_mp3_t* pAudio, const char* path) {
    FILE* mp3 = fopen(path, "rb+");

    fseek(mp3, 0, SEEK_END);
    size_t len = ftell(mp3);
    fseek(mp3, 0, SEEK_SET);

    uint8_t* data = (uint8_t*)malloc(len);

    fread(data, 1, len, mp3);

    fclose(mp3);



    free(data);
}

int ulAudioLoadFromWAV(ul_audio_wav_t* pAudio, const char* path) {
    FILE* wav = fopen(path, "rb+");

    fseek(wav, 0, SEEK_END);
    size_t len = ftell(wav);
    fseek(wav, 0, SEEK_SET);

    uint8_t* data = (uint8_t*)malloc(len);

    fread(data, 1, len, wav);

    fclose(wav);

    pAudio->subchunk1Id = (uint32_t)data[12] << 24 | (uint32_t)data[13] << 16 | (uint32_t)data[14] << 8 | (uint32_t)data[15];
    pAudio->subchunk1Size = (uint32_t)data[19] << 24 | (uint32_t)data[18] << 16 | (uint32_t)data[17] << 8 | (uint32_t)data[16];
    pAudio->audioFormat = (uint16_t)data[21] << 8 | (uint16_t)data[20];
    pAudio->channels = (uint16_t)data[23] << 8 | (uint16_t)data[22];
    pAudio->samplerate = (uint32_t)data[27] << 24 | (uint32_t)data[26] << 16 | (uint32_t)data[25] << 8 | (uint32_t)data[24];
    pAudio->byterate = (uint32_t)data[31] << 24 | (uint32_t)data[30] << 16 | (uint32_t)data[29] << 8 | (uint32_t)data[28];
    pAudio->blockAlign = (uint16_t)data[33] << 8 | (uint16_t)data[32];
    pAudio->bitsPerSample = (uint16_t)data[35] << 8 | (uint16_t)data[34];
    pAudio->subchunk2Id = (uint32_t)data[36] << 24 | (uint32_t)data[37] << 16 | (uint32_t)data[38] << 8 | (uint32_t)data[39];
    pAudio->subchunk2Size = (uint32_t)data[43] << 24 | (uint32_t)data[42] << 16 | (uint32_t)data[41] << 8 | (uint32_t)data[40];

    pAudio->data = (uint8_t*)malloc(pAudio->subchunk2Size);

    memcpy(pAudio->data, data + 44, pAudio->subchunk2Size);

    free(data);
}

#define WAV_CHANNELS_MONO    1
#define WAV_CHANNELS_STEREO  2

#define WAV_SAMPLERATE_44_1kHz  44100
#define WAV_SAMPLERATE_48kHz    48000
#define WAV_SAMPLERATE_32kHz    32000

#define WAV_8_BITS_PER_SAMPLE   8
#define WAV_16_BITS_PER_SAMPLE  16

void ulAudioMakeWAV(ul_audio_wav_t* pAudio, uint16_t audioFormat, uint16_t channels, uint32_t samplerate, uint16_t bitsPerSample) {
    pAudio->subchunk1Id = (uint32_t)'f' << 24 | (uint32_t)'m' << 16 | (uint32_t)'t' << 8 | 0x20;
    pAudio->subchunk1Size = 0x10;
    pAudio->subchunk2Id = (uint32_t)'d' << 24 | (uint32_t)'a' << 16 | (uint32_t)'t' << 8 | (uint32_t)'a';
    pAudio->audioFormat = audioFormat;
    pAudio->channels = channels;
    pAudio->samplerate = samplerate;
    pAudio->bitsPerSample = bitsPerSample;
    pAudio->byterate = pAudio->samplerate * pAudio->channels * pAudio->bitsPerSample / 8;
    pAudio->blockAlign = pAudio->channels * pAudio->bitsPerSample / 8;
}

void ulAudioSetWAVData(ul_audio_wav_t* pAudio, uint8_t* data, uint32_t size) {
    pAudio->subchunk2Size = size;
    pAudio->data = (uint8_t*)realloc(pAudio->data, pAudio->subchunk2Size);

    memcpy(pAudio->data, data, pAudio->subchunk2Size);
}

// TODO: Save to wave file
/*void ulAudioSaveWAV(ul_audio_wav_t* pAudio, const char* path) {
    FILE* wav = fopen(path, "wb+");


}*/

void ulAudioFreeWAV(ul_audio_wav_t* pAudio) {
    free(pAudio->data);
    pAudio->data = (uint8_t*)0;
}

#endif