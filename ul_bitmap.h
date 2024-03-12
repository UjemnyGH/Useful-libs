/**
 * @file ul_bitmap.hpp
 * @author Piotr "UjemnyGH" Plombon
 * @brief Loads bitmap images
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyleft (c) 2024
 * 
 */

#ifndef _UL_BITMAP_
#define _UL_BITMAP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Offsets
#define DONT_CARE_OFFSET        0xd
#define HEADER_SIZE             0xe
#define BM_WIDTH                0x12
#define BM_HEIGHT               0x16
#define BM_BITS_PER_PIXEL       0x1c

// Values
#define BM_BPP_1                0x1
#define BM_BPP_4                0x4
#define BM_BPP_8                0x8
#define BM_BPP_16               0x10
#define BM_BPP_24               0x18
#define BM_BPP_32               0x20

uint8_t* LoadBitmapFromFile(const char* filename, uint32_t* w, uint32_t* h) {
    *w = 0;
    *h = 0;

    uint32_t headerSize = 0;
    uint32_t *width = w, *height = h;
    uint16_t bitsPerPixel = 0;

    FILE* f = fopen(filename, "rb+");

    fseek(f, HEADER_SIZE, SEEK_SET);

    for(uint32_t i = 0; i < 4; i++) {
        uint32_t cc = fgetc(f);

        headerSize |= cc << (i * 8);
    }

    fseek(f, BM_WIDTH, SEEK_SET);

    for(uint32_t i = 0; i < 4; i++) {
        uint32_t cc = fgetc(f);

        *width |= cc << (i * 8);
    }

    fseek(f, BM_HEIGHT, SEEK_SET);

    for(uint32_t i = 0; i < 4; i++) {
        uint32_t cc = fgetc(f);

        *height |= cc << (i * 8);
    }

    fseek(f, BM_BITS_PER_PIXEL, SEEK_SET);

    for(uint32_t i = 0; i < 2; i++) {
        uint32_t cc = fgetc(f);

        bitsPerPixel |= cc << (i * 8);
    }

    fseek(f, headerSize + DONT_CARE_OFFSET + 1, SEEK_SET);

    uint32_t result_counter = 0;
    uint8_t* result = (uint8_t*)realloc(result, *width * *height * 4);
    uint32_t bytesCounter = (bitsPerPixel / 8) - 1;

    uint8_t bytes[4] = {0, 0, 0, 0};

    while(!feof(f)) {
        uint32_t cc = fgetc(f);

        bytes[bytesCounter] = cc;

        if(bytesCounter == 0) {
            bytesCounter = (bitsPerPixel / 8);
            for(uint32_t i = 0; i < (bitsPerPixel / 8); i++) {
                result[result_counter++] = bytes[i];
            }

            if((bitsPerPixel / 8) == 3) {
                result[result_counter++] = 0xff;
            }
        }

        if(bytesCounter > 0) {
            bytesCounter--;
        }

    }

    fclose(f);

    return result;
}

#endif