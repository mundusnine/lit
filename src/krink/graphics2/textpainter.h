#pragma once

#include "ttf.h"

#include <krink/math/matrix.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef KR_G2_TSP_BUFFER_SIZE
#define KR_G2_TSP_BUFFER_SIZE 1000
#endif

void kr_tsp_init(void);
void kr_tsp_draw_buffer(bool end);
void kr_tsp_set_bilinear_filter(bool bilinear);
void kr_tsp_set_projection_matrix(kinc_matrix4x4_t mat);
void kr_tsp_set_font(kr_ttf_font_t *font);
void kr_tsp_set_font_size(int size);
int kr_tsp_draw_string(const char *text, float opacity, uint32_t color, float x, float y,
                        kr_matrix3x3_t transformation);
int kr_tsp_draw_characters(const int *text, int start, int length, float opacity, uint32_t color,
                            float x, float y, kr_matrix3x3_t transformation);
void kr_tsp_end(void);
