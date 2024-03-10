/**
 * @file ul_quick_gui.h
 * @author Piotr "UjemnyGH" Plombon
 * @brief 
 * @version 0.1
 * @date 2024-03-10
 * 
 * @copyright Copyleft (c) 2024
 * 
 * To use this library just add #include "ul_quick_qui.h" in your file, but for now library does nothing
 * 
 * Have fun!
 */

#ifndef _UL_QUICK_GUI_
#define _UL_QUICK_GUI_

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#define DRAW_TYPE_TRIANGLE  2
#define DRAW_TYPE_LINE      1
#define DRAW_TYPE_POINT     0

typedef void (*PFN_DrawPrototype)(int type);

typedef struct ul_qgui_s {
    float* vertices;
    float* colors;
    float* textureCoorinates;
    uint64_t size;

    double mouseX, mouseY;
    int windowWidth, windowHeight;
} ul_qgui_t;

void ulQGuiAllocate(ul_qgui_t* pGui, size_t size) {
    pGui->size = size;

    pGui->vertices = (float*)realloc(pGui->vertices, pGui->size * sizeof(float) * 2);
    pGui->colors = (float*)realloc(pGui->colors, pGui->size * sizeof(float) * 4);
    pGui->textureCoorinates = (float*)realloc(pGui->textureCoorinates, pGui->size * sizeof(float) * 2);
}

/**
 * @brief Remember you add point size like for example entire color (4 float values = 16 bytes), not 1 value (like float)
 * 
 * @param pGui 
 * @param addedSize 
 */
void ulQGuiAddSize(ul_qgui_t *pGui, size_t addedSize) {
    ulQGuiAllocate(pGui, pGui->size + addedSize);
}

void ulQGuiFree(ul_qgui_t* pGui) {
    pGui->size = 0;

    free(pGui->vertices);
    free(pGui->colors);
    free(pGui->textureCoorinates);

    pGui->vertices = (float*)0;
    pGui->colors = (float*)0;
    pGui->textureCoorinates = (float*)0;
}

void ulQGuiDrawPoint(ul_qgui_t* pGui, float x, float y, float r, float g, float b, float a) {
    ulQGuiAddSize(pGui, 1);

    pGui->vertices[pGui->size * 2 - 2] = x;
    pGui->vertices[pGui->size * 2 - 1] = y;

    pGui->colors[pGui->size * 4 - 4] = r;
    pGui->colors[pGui->size * 4 - 3] = g;
    pGui->colors[pGui->size * 4 - 2] = b;
    pGui->colors[pGui->size * 4 - 1] = a;

    pGui->textureCoorinates[pGui->size * 2 - 2] = 0.5f;
    pGui->textureCoorinates[pGui->size * 2 - 1] = 0.5f;
}

void ulQGuiDrawLine(ul_qgui_t* pGui, float x0, float y0, float x1, float y1, float r, float g, float b, float a) {
    ulQGuiAddSize(pGui, 2);

    pGui->vertices[pGui->size * 2 - 4] = x0;
    pGui->vertices[pGui->size * 2 - 3] = y0;
    pGui->vertices[pGui->size * 2 - 2] = x1;
    pGui->vertices[pGui->size * 2 - 1] = y1;

    pGui->colors[pGui->size * 4 - 8] = r;
    pGui->colors[pGui->size * 4 - 7] = g;
    pGui->colors[pGui->size * 4 - 6] = b;
    pGui->colors[pGui->size * 4 - 5] = a;

    pGui->colors[pGui->size * 4 - 4] = r;
    pGui->colors[pGui->size * 4 - 3] = g;
    pGui->colors[pGui->size * 4 - 2] = b;
    pGui->colors[pGui->size * 4 - 1] = a;

    pGui->textureCoorinates[pGui->size * 2 - 4] = 1.0f;
    pGui->textureCoorinates[pGui->size * 2 - 3] = 1.0f;

    pGui->textureCoorinates[pGui->size * 2 - 2] = 0.0f;
    pGui->textureCoorinates[pGui->size * 2 - 1] = 0.0f;
}

void ulQGuiDrawTriangle(ul_qgui_t* pGui, float x0, float y0, float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    ulQGuiAddSize(pGui, 3);

    pGui->vertices[pGui->size * 2 - 6] = x0;
    pGui->vertices[pGui->size * 2 - 5] = y0;
    pGui->vertices[pGui->size * 2 - 4] = x1;
    pGui->vertices[pGui->size * 2 - 3] = y1;
    pGui->vertices[pGui->size * 2 - 2] = x2;
    pGui->vertices[pGui->size * 2 - 1] = y2;

    pGui->colors[pGui->size * 4 - 12] = r;
    pGui->colors[pGui->size * 4 - 11] = g;
    pGui->colors[pGui->size * 4 - 10] = b;
    pGui->colors[pGui->size * 4 - 9] = a;

    pGui->colors[pGui->size * 4 - 8] = r;
    pGui->colors[pGui->size * 4 - 7] = g;
    pGui->colors[pGui->size * 4 - 6] = b;
    pGui->colors[pGui->size * 4 - 5] = a;

    pGui->colors[pGui->size * 4 - 4] = r;
    pGui->colors[pGui->size * 4 - 3] = g;
    pGui->colors[pGui->size * 4 - 2] = b;
    pGui->colors[pGui->size * 4 - 1] = a;

    pGui->textureCoorinates[pGui->size * 2 - 6] = 0.0f;
    pGui->textureCoorinates[pGui->size * 2 - 5] = 0.0f;

    pGui->textureCoorinates[pGui->size * 2 - 4] = 1.0f;
    pGui->textureCoorinates[pGui->size * 2 - 3] = 0.0f;

    pGui->textureCoorinates[pGui->size * 2 - 2] = 0.0f;
    pGui->textureCoorinates[pGui->size * 2 - 1] = 1.0f;
}

void ulQGuiDrawQuad(ul_qgui_t* pGui, float x, float y, float sizeX, float sizeY, float r, float g, float b, float a) {
    ulQGuiDrawTriangle(pGui, x + sizeX, y + sizeY, x + sizeX, y - sizeY, x - sizeX, y + sizeY, r, g, b, a);
    ulQGuiDrawTriangle(pGui, x - sizeX, y - sizeY, x + sizeX, y - sizeY, x - sizeX, y + sizeY, r, g, b, a);

    pGui->textureCoorinates[pGui->size * 2 - 6] = 1.0f;
    pGui->textureCoorinates[pGui->size * 2 - 5] = 0.0f;

    pGui->textureCoorinates[pGui->size * 2 - 4] = 0.0f;
    pGui->textureCoorinates[pGui->size * 2 - 3] = 1.0f;

    pGui->textureCoorinates[pGui->size * 2 - 2] = 1.0f;
    pGui->textureCoorinates[pGui->size * 2 - 1] = 1.0f;
}

void ulQGuiDraw() {

}

#endif