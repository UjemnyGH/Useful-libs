/**
 * @file ul_mesh.h
 * @author Piotr "UjemnyGH" Plombon
 * @brief 
 * @version 0.1
 * @date 2024-03-10
 * 
 * @copyright Copyleft (c) 2024
 * 
 * This library is for loading 3D geometry
 * 
 * 
 * Have fun!
 */

#pragma once
#ifndef _UL_MESH_
#define _UL_MESH_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>

enum {
    ULMtype_ply,
    ULMtype_obj,
    ULMtype_stl,

    ULMtype_END_DONT_USE
};

typedef struct ul_mesh_s {
    // C implementation
    //float* vertices, *normals, *textureCoordinates;
    std::vector<float> vertices, normals, textureCoordinates;
} ul_mesh_t;

// Needless
/*
void ulMeshAllocate(ul_mesh_t* pMesh, uint32_t size) {
    pMesh->meshSize = size;

    pMesh->vertices = (float*)realloc(pMesh->vertices, pMesh->meshSize * sizeof(float) * 3);
    pMesh->normals = (float*)realloc(pMesh->normals, pMesh->meshSize * sizeof(float) * 3);
    pMesh->textureCoordinates = (float*)realloc(pMesh->textureCoordinates, pMesh->meshSize * sizeof(float) * 2);
}


void ulMeshFree(ul_mesh_t* pMesh) {
    pMesh->meshSize = 0;

    free(pMesh->vertices);
    free(pMesh->normals);
    free(pMesh->textureCoordinates);

    pMesh->vertices = (float*)0;
    pMesh->normals = (float*)0;
    pMesh->textureCoordinates = (float*)0;
}
*/

size_t ulMeshCopyNumber(char* dst, char* src, size_t size, uint8_t stopVal) {
    size_t dst_count = 0;
    memset(dst, 0, strlen(dst));

    for(size_t i = 0; i < size; i++) {
        if(((uint8_t*)src)[i] == stopVal) {
            return i + 1;
        }

        if((*(((uint8_t*)src) + i) >= '0' && *(((uint8_t*)src) + i) <= '9') || *(((uint8_t*)src) + i) == '.' || *(((uint8_t*)src) + i) == '-') {
            ((uint8_t*)dst)[dst_count++] = *(((uint8_t*)src) + i);
        }
    }

    return size;
}

int __ulMeshLoadPLY(ul_mesh_t* pMesh, const char* path) {
    FILE* mesh_file = fopen(path, "rb+");

    fseek(mesh_file, 0, SEEK_END);
    uint32_t len = ftell(mesh_file);
    fseek(mesh_file, 0, SEEK_SET);

    uint8_t *source_buffer = new uint8_t[len]; //(uint8_t*)calloc(len, sizeof(uint8_t));

    fread(source_buffer, sizeof(uint8_t), len, mesh_file);

    if(source_buffer[0] != 'p' && source_buffer[1] != 'l' && source_buffer[2] != 'y') {
        fclose(mesh_file);

        printf("Desired model isn`t ply model!");

        delete[] source_buffer;
        //source_buffer = (uint8_t*)0;

        return 0;
    }

    uint8_t* end_header = (uint8_t*)strstr((char*)source_buffer, "end_header");
    uint8_t* vertex_element = (uint8_t*)strstr((char*)source_buffer, "element vertex ");
    uint8_t* face_element = (uint8_t*)strstr((char*)source_buffer, "element face ");
    uint8_t* format = (uint8_t*)strstr((char*)source_buffer, "format ascii 1.0 ");
    uint8_t* prop_vertices = (uint8_t*)strstr((char*)source_buffer, "property float x\n");
    uint8_t* prop_normals = (uint8_t*)strstr((char*)source_buffer, "property float nx\n");
    uint8_t* prop_texpos = (uint8_t*)strstr((char*)source_buffer, "property float s\n");

    char number_buffer[256];

    for(uint8_t* i = vertex_element; i < vertex_element + 256; i++) {
        if(vertex_element[16 + (i - vertex_element)] == '\n') break;

        number_buffer[i - vertex_element] = vertex_element[16 + (i - vertex_element)];
    }

    uint32_t vertex_amount = atoi(number_buffer);

    memset(number_buffer, 0, 256);

    for(uint8_t* i = face_element; i < face_element + 256; i++) {
        if(face_element[16 + (i - face_element)] == '\n') break;

        number_buffer[i - face_element] = face_element[16 + (i - face_element)];
    }

    uint32_t faces_amount = atoi(number_buffer);

    fseek(mesh_file, 0, SEEK_SET);

    char *line = new char[len]; //(char*)calloc(len, sizeof(char));

    uint32_t vertex_counter = 0;
    uint32_t faces_counter = 0;

    ul_mesh_t temp;

    // Needless
    /*
    uint32_t temp_vert_counter = 0;
    uint32_t temp_norm_counter = 0;
    uint32_t temp_texc_counter = 0;
    
    temp.meshSize = 0;
    temp.vertices = (float*)0;
    temp.normals = (float*)0;
    temp.textureCoordinates = (float*)0;

    ulMeshAllocate(&temp, vertex_amount);
    */

    while(!feof(mesh_file)) {
        uint8_t ch = 0;

        if(format != (uint8_t*)0) {
            fgets(line, (size_t)len, mesh_file);
        }
        else {
            ch = (uint8_t)fgetc(mesh_file);
        }

        if(ftell(mesh_file) <= (long)(source_buffer - end_header + 11)) continue;

        if(format != (uint8_t*)0) {
            if(strcmp(line, "end_header\n") == 0) continue;

            if(vertex_counter < vertex_amount) {
                char number[256];
                size_t current_pos = 0;

                if(prop_vertices != (uint8_t*)0) {
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.vertices.push_back(atof(number));
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.vertices.push_back(atof(number));
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.vertices.push_back(atof(number));
                }

                if(prop_normals != (uint8_t*)0) {
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.normals.push_back(atof(number));
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.normals.push_back(atof(number));
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.normals.push_back(atof(number));
                }

                if(prop_texpos != (uint8_t*)0) {
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.textureCoordinates.push_back(atof(number));
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.textureCoordinates.push_back(atof(number));
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    temp.textureCoordinates.push_back(atof(number));
                }

                vertex_counter++;

                continue;
            }

            if(faces_counter < faces_amount) {
                char number[256];
                size_t current_pos = 0;

                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                int faces_value_amount = atoi(number);

                if(faces_value_amount == 4) {
                    uint32_t face[4] = { 0, 0, 0, 0 };

                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[0] = atoi(number);
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[1] = atoi(number);
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[2] = atoi(number);
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[3] = atoi(number);

                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[3] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[3] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[3] * 3 + 2]);

                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[3] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[3] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[3] * 3 + 2]);

                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[3] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[3] * 2 + 1]);
                }
                else {
                    uint32_t face[3] = { 0, 0, 0 };

                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[0] = atoi(number);
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[1] = atoi(number);
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face[2] = atoi(number);

                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 2]);

                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 2]);

                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 1]);
                }

                faces_counter++;

                continue;
            }
        }
        else {
            uint32_t convert = 0;

            if(vertex_counter < vertex_amount) {
                if(prop_vertices != (uint8_t*)0) {
                    convert = (uint32_t)ch | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.vertices.push_back(*(float*)&convert);
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.vertices.push_back(*(float*)&convert);
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.vertices.push_back(*(float*)&convert);
                }

                if(prop_normals != (uint8_t*)0) {
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.normals.push_back(*(float*)&convert);
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.normals.push_back(*(float*)&convert);
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.normals.push_back(*(float*)&convert);
                }

                if(prop_texpos != (uint8_t*)0) {
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.textureCoordinates.push_back(*(float*)&convert);
                    convert = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    temp.textureCoordinates.push_back(*(float*)&convert);
                }

                vertex_counter++;

                continue;
            }

            if(faces_counter < faces_amount) {
                uint8_t faces_value_amount = ch;

                if(faces_value_amount == 4) {
                    uint32_t face[4] = { 0, 0, 0, 0 };

                    face[0] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    face[1] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    face[2] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    face[3] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;

                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[3] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[3] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[3] * 3 + 2]);

                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[3] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[3] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[3] * 3 + 2]);

                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[3] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[3] * 2 + 1]);
                }
                else {
                    uint32_t face[3] = { 0, 0, 0 };

                    face[0] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    face[1] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;
                    face[2] = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24;

                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[0] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[1] * 3 + 2]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 0]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 1]);
                    pMesh->vertices.push_back(temp.vertices[face[2] * 3 + 2]);

                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[0] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[1] * 3 + 2]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 0]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 1]);
                    pMesh->normals.push_back(temp.normals[face[2] * 3 + 2]);

                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[0] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[1] * 2 + 1]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 0]);
                    pMesh->textureCoordinates.push_back(temp.textureCoordinates[face[2] * 2 + 1]);
                }

                faces_counter++;

                continue;
            }
        }
    }

    // free(line);
    // free(source_buffer);
    delete[] line;
    delete[] source_buffer;
    fclose(mesh_file);

    return 1;
}

int __ulMeshLoadOBJ(ul_mesh_t* pMesh, const char* path) {
    FILE* mesh_file = fopen(path, "rb+");

    fseek(mesh_file, 0, SEEK_END);
    uint32_t len = ftell(mesh_file);
    fseek(mesh_file, 0, SEEK_SET);

    char *line = new char[len]; // (char*)calloc(len, sizeof(char));

    float* temp_vert = (float*)0;
    float* temp_norm = (float*)0;
    float* temp_texc = (float*)0;
    uint32_t tv_size = 0;
    uint32_t tn_size = 0;
    uint32_t tt_size = 0;

    while(!feof(mesh_file)) {
        char number[256];
        size_t current_pos = 0;

        fgets(line, (size_t)len, mesh_file);

        if(strstr(line, "v ") != (char*)0) {
            temp_vert = (float*)realloc(temp_vert, sizeof(float) * (++tv_size) * 3);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_vert[tv_size * 3 - 3] = atof(number);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_vert[tv_size * 3 - 2] = atof(number);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_vert[tv_size * 3 - 1] = atof(number);
        }
        else if(strstr(line, "vn ") != (char*)0) {
            temp_norm = (float*)realloc(temp_norm, sizeof(float) * (++tn_size) * 3);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_norm[tn_size * 3 - 3] = atof(number);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_norm[tn_size * 3 - 2] = atof(number);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_norm[tn_size * 3 - 1] = atof(number);
        }
        else if(strstr(line, "vt ") != (char*)0) {
            temp_texc = (float*)realloc(temp_texc, sizeof(float) * (++tt_size) * 2);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_texc[tt_size * 2 - 2] = atof(number);
            current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
            temp_texc[tt_size * 2 - 1] = atof(number);
        }
        else if(strstr(line, "f ") != (char*)0) {
            int slashes = 0;
            int no_textures = 0;

            for(int i = 0; i < len; i++) {
                if(line[i] == 0) break;

                if(line[i] == '/') slashes++;
                if(line[i] == '/' && line[i + 1] == '/') no_textures = 1;
            }

            if(slashes >= 7) {
                uint32_t face_v[4];
                uint32_t face_t[4];
                uint32_t face_n[4];

                for(int i = 0; i < 4; i++) {
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, '/');
                    face_v[i] = (uint32_t)atoi(number);
                    
                    if(!no_textures) {
                        current_pos += ulMeshCopyNumber(number, line + current_pos, 256, '/');
                        face_t[i] = (uint32_t)atoi(number);
                    }
                    else {
                        current_pos++;
                    }

                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face_n[i] = (uint32_t)atoi(number);
                }

                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[2] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[2] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[2] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[3] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[3] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[3] * 3 + 2]);

                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[2] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[2] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[2] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[3] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[3] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[3] * 3 + 2]);

                if(no_textures) continue;

                pMesh->textureCoordinates.push_back(temp_texc[face_t[0] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[0] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[1] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[1] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[2] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[2] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[0] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[0] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[1] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[1] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[3] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[3] * 2 + 1]);
            }
            else {
                uint32_t face_v[3];
                uint32_t face_t[3];
                uint32_t face_n[3];

                for(int i = 0; i < 3; i++) {
                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, '/');
                    face_v[i] = (uint32_t)atoi(number);
                    
                    if(!no_textures) {
                        current_pos += ulMeshCopyNumber(number, line + current_pos, 256, '/');
                        face_t[i] = (uint32_t)atoi(number);
                    }
                    else {
                        current_pos++;
                    }

                    current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                    face_n[i] = (uint32_t)atoi(number);
                }

                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[0] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[1] * 3 + 2]);
                pMesh->vertices.push_back(temp_vert[face_v[2] * 3 + 0]);
                pMesh->vertices.push_back(temp_vert[face_v[2] * 3 + 1]);
                pMesh->vertices.push_back(temp_vert[face_v[2] * 3 + 2]);

                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[0] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[1] * 3 + 2]);
                pMesh->normals.push_back(temp_norm[face_n[2] * 3 + 0]);
                pMesh->normals.push_back(temp_norm[face_n[2] * 3 + 1]);
                pMesh->normals.push_back(temp_norm[face_n[2] * 3 + 2]);

                if(no_textures) continue;

                pMesh->textureCoordinates.push_back(temp_texc[face_t[0] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[0] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[1] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[1] * 2 + 1]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[2] * 2 + 0]);
                pMesh->textureCoordinates.push_back(temp_texc[face_t[2] * 2 + 1]);
            }
        }
    }

    free(temp_vert);
    temp_vert = (float*)0;
    tv_size = 0;

    free(temp_norm);
    temp_norm = (float*)0;
    tn_size = 0;

    free(temp_texc);
    temp_texc = (float*)0;
    tt_size = 0;

    free(line);
    fclose(mesh_file);

    return 1;
}

int __ulMeshLoadSTL(ul_mesh_t* pMesh, const char* path) {
    FILE* mesh_file = fopen(path, "rb+");

    fseek(mesh_file, 0, SEEK_END);
    uint32_t len = ftell(mesh_file);
    fseek(mesh_file, 0, SEEK_SET);

    uint8_t *source_buffer = new uint8_t[len]; // (uint8_t*)calloc(len, sizeof(uint8_t));

    fread(source_buffer, sizeof(uint8_t), len, mesh_file);

    int is_ascii = strstr((char*)source_buffer, "solid") != (char*)0;

    if(is_ascii){
        fseek(mesh_file, 0, SEEK_SET);
    }
    else {
        fseek(mesh_file, 0x50, SEEK_SET);
    }

    char *line = new char[len]; // (char*)calloc(len, sizeof(char));

    // It ain`t used
    uint32_t number_of_triangles = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 

    while(!feof(mesh_file)) {
        if(is_ascii) {
            char number[256];
            size_t current_pos = 0;

            fgets(line, len, mesh_file);

            if(strstr(line, "facet normal ") != (char*)0) {
                float normal[3];
                float vertex[3][3];

                current_pos += 13;

                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                normal[0] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                normal[1] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                normal[2] = atof(number);

                fgets(line, len, mesh_file);
                fgets(line, len, mesh_file);
                
                current_pos = (line - strstr(line, "vertex ")) + 7;
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[0][0] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[1][0] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[2][0] = atof(number);

                fgets(line, len, mesh_file);
                
                current_pos = (line - strstr(line, "vertex ")) + 7;
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[0][1] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[1][1] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[2][1] = atof(number);

                fgets(line, len, mesh_file);
                
                current_pos = (line - strstr(line, "vertex ")) + 7;
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[0][2] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[1][2] = atof(number);
                current_pos += ulMeshCopyNumber(number, line + current_pos, 256, ' ');
                vertex[2][2] = atof(number);

                fgets(line, len, mesh_file);
                fgets(line, len, mesh_file);

                pMesh->vertices.push_back(vertex[0][0]);
                pMesh->vertices.push_back(vertex[1][0]);
                pMesh->vertices.push_back(vertex[2][0]);
                pMesh->vertices.push_back(vertex[0][1]);
                pMesh->vertices.push_back(vertex[1][1]);
                pMesh->vertices.push_back(vertex[2][1]);
                pMesh->vertices.push_back(vertex[0][2]);
                pMesh->vertices.push_back(vertex[1][2]);
                pMesh->vertices.push_back(vertex[2][2]);

                pMesh->normals.push_back(normal[0]);
                pMesh->normals.push_back(normal[1]);
                pMesh->normals.push_back(normal[2]);
                pMesh->normals.push_back(normal[0]);
                pMesh->normals.push_back(normal[1]);
                pMesh->normals.push_back(normal[2]);
                pMesh->normals.push_back(normal[0]);
                pMesh->normals.push_back(normal[1]);
                pMesh->normals.push_back(normal[2]);

                // TODO: STL texture coordinates generating. Currently not generated!
                pMesh->textureCoordinates.push_back(0.0f);
                pMesh->textureCoordinates.push_back(0.0f);
                pMesh->textureCoordinates.push_back(1.0f);
                pMesh->textureCoordinates.push_back(0.0f);
                pMesh->textureCoordinates.push_back(0.0f);
                pMesh->textureCoordinates.push_back(1.0f);
            }
        }
        else {
            uint32_t normal_x = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t normal_y = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t normal_z = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 

            uint32_t vertex_x0 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t vertex_y0 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t vertex_z0 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 

            uint32_t vertex_x1 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t vertex_y1 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t vertex_z1 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 

            uint32_t vertex_x2 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t vertex_y2 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            uint32_t vertex_z2 = (uint32_t)fgetc(mesh_file) | (uint32_t)fgetc(mesh_file) << 8 | (uint32_t)fgetc(mesh_file) << 16 | (uint32_t)fgetc(mesh_file) << 24; 
            // Basicly thrash bytes, always set to 0
            uint16_t attrib_count = (uint16_t)fgetc(mesh_file) | (uint16_t)fgetc(mesh_file) << 8;

            pMesh->vertices.push_back(*(float*)&vertex_x0);
            pMesh->vertices.push_back(*(float*)&vertex_y0);
            pMesh->vertices.push_back(*(float*)&vertex_z0);
            pMesh->vertices.push_back(*(float*)&vertex_x1);
            pMesh->vertices.push_back(*(float*)&vertex_y1);
            pMesh->vertices.push_back(*(float*)&vertex_z1);
            pMesh->vertices.push_back(*(float*)&vertex_x2);
            pMesh->vertices.push_back(*(float*)&vertex_y2);
            pMesh->vertices.push_back(*(float*)&vertex_z2);

            pMesh->normals.push_back(*(float*)&normal_x);
            pMesh->normals.push_back(*(float*)&normal_y);
            pMesh->normals.push_back(*(float*)&normal_z);
            pMesh->normals.push_back(*(float*)&normal_x);
            pMesh->normals.push_back(*(float*)&normal_y);
            pMesh->normals.push_back(*(float*)&normal_z);
            pMesh->normals.push_back(*(float*)&normal_x);
            pMesh->normals.push_back(*(float*)&normal_y);
            pMesh->normals.push_back(*(float*)&normal_z);

            // TODO: STL texture coordinates generating. Currently not generated!
            pMesh->textureCoordinates.push_back(0.0f);
            pMesh->textureCoordinates.push_back(0.0f);
            pMesh->textureCoordinates.push_back(1.0f);
            pMesh->textureCoordinates.push_back(0.0f);
            pMesh->textureCoordinates.push_back(0.0f);
            pMesh->textureCoordinates.push_back(1.0f);
        }
    }

    //free(line);
    //free(source_buffer);
    delete[] line;
    delete[] source_buffer;
    fclose(mesh_file);
}

/**
 * @brief Load mesh to ul_mesh_t struct, REMEMBER THAT STL ONLY HAVE VERTICES AND NORMALS!
 * 
 * @param pMesh ul_mesh_t struct
 * @param path path to mesh
 * @param type ULMtype_
 */
void ulMeshLoad(ul_mesh_t* pMesh, const char* path, uint32_t type) {
    if(type == ULMtype_ply) {
        __ulMeshLoadPLY(pMesh, path);
    }
    else if(type == ULMtype_obj) {
        __ulMeshLoadOBJ(pMesh, path);
    }
    else if(type == ULMtype_stl) {
        __ulMeshLoadSTL(pMesh, path);
    }
}

// TODO: Save model to file format

#endif