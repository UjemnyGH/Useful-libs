/**
 * @file ul_bitmap.hpp
 * @author Piotr "UjemnyGH" Plombon
 * @brief Loads bitmap images (C++ warp)
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyleft (c) 2024
 * 
 */

#pragma once
#ifndef _UL_BITMAP_
#define _UL_BITMAP_

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>

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

std::vector<uint8_t> ulLoadBitmapFromFile(std::string filename, uint32_t* w, uint32_t* h, bool recieveSizes = true) {
    *w = 0;
    *h = 0;

    uint32_t headerSize = 0;
    uint32_t *width = w, *height = h;
    uint16_t bitsPerPixel = 0;

    std::ifstream f(filename, std::ios_base::binary);

    f.seekg(HEADER_SIZE, std::ios_base::beg);

    for(uint32_t i = 0; i < 4; i++) {
        uint32_t cc = f.get();

        headerSize |= cc << (i * 8);
    }

    if(recieveSizes) {
        f.seekg(BM_WIDTH, std::ios_base::beg);

        for(uint32_t i = 0; i < 4; i++) {
            uint32_t cc = f.get();

            *width |= cc << (i * 8);
        }

        f.seekg(BM_HEIGHT, std::ios_base::beg);

        for(uint32_t i = 0; i < 4; i++) {
            uint32_t cc = f.get();

            *height |= cc << (i * 8);
        }
    }

    f.seekg(BM_BITS_PER_PIXEL, std::ios_base::beg);

    for(uint32_t i = 0; i < 2; i++) {
        uint32_t cc = f.get();

        bitsPerPixel |= cc << (i * 8);
    }

    f.seekg(headerSize + DONT_CARE_OFFSET + 1, std::ios_base::beg);

    std::vector<uint8_t> result;
    uint32_t bytesCounter = (bitsPerPixel / 8) - 1;

    uint8_t bytes[4] = {0, 0, 0, 0};

    while(!f.eof()) {
        uint32_t cc = f.get();

        bytes[bytesCounter] = cc;

        if(bytesCounter == 0) {
            bytesCounter = (bitsPerPixel / 8);
            for(uint32_t i = 0; i < (bitsPerPixel / 8); i++) {
                result.push_back(bytes[i]);
            }

            if((bitsPerPixel / 8) == 3) {
                result.push_back(0xff);
            }
        }

        if(bytesCounter > 0) {
            bytesCounter--;
        }

    }

    f.close();

    return result;
}

#endif