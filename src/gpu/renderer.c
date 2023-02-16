#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "lib/stb/stb_truetype.h"
#include <kinc/color.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/system.h>
#include <krink/color.h>
#include <krink/system.h>
#include <krink/memory.h>
#include <krink/graphics2/graphics.h>
#include <krink/graphics2/ttf.h>
#include "renderer.h"

#define MAX_GLYPHSET 256

struct RenImage {
  RenColor *pixels;
  int width, height;
};

typedef struct {
  RenImage *image;
  stbtt_bakedchar glyphs[256];
} GlyphSet;

struct RenFont {
  kr_ttf_font_t* data;
  float size;
  int height;
};

static kinc_image_t surf;
static struct { int left, top, right, bottom; } clip;

static inline uint32_t color_to_uint(RenColor color) {
	uint32_t c = 0;
	c = kr_color_set_channel(c, 'A', color.a);
	c = kr_color_set_channel(c, 'R', color.r);
	c = kr_color_set_channel(c, 'G', color.g);
	c = kr_color_set_channel(c, 'B', color.b);
	return c;
}

static void* check_alloc(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "Fatal error: memory allocation failed\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}


void ren_init(void) {
  kr_g2_init();
  int w = kinc_width();
  int h = kinc_height();
}

void ren_update_rects(RenRect *rects, int count) {}

void ren_set_clip_rect(RenRect rect) {
  kr_g2_scissor(rect.x,rect.y,rect.width,rect.height);
}

void ren_pop_clip_rect(void) {
  kr_g2_disable_scissor();
}


void ren_get_size(int *x, int *y) {
  *x = kinc_width();
  *y = kinc_height();
}


// RenImage* ren_new_image(int width, int height) {
//   assert(width > 0 && height > 0);
//   RenImage *image = kr_malloc(sizeof(RenImage) + width * height * sizeof(RenColor));
//   check_alloc(image);
//   image->pixels = (void*) (image + 1);
//   image->width = width;
//   image->height = height;
//   return image;
// }


// void ren_free_image(RenImage *image) {
//   kr_free(image);
// }

struct Font{
  char fname[260];
  kr_ttf_font_t font;
};

static struct Font fonts[64];
static size_t num_fonts = 0;
RenFont* ren_load_font(const char *filename, float size) {

  RenFont *font = NULL;
  FILE *fp = NULL;

  /* init font */
  font = check_alloc(kr_malloc(sizeof(RenFont)));
  bool found = false;
  for(int i =0; i < num_fonts;++i){
    if(strcmp(fonts[i].fname,filename) == 0){
      font->data = &fonts[i].font;
      found = true;
    }
  }
  if(!found){
    font->data = &fonts[num_fonts].font;
    strcpy(fonts[num_fonts].fname,filename);
    kr_ttf_font_init(font->data,filename,0);
    num_fonts++;
  }

  float scale = kr_ttf_load(font->data,size);
  font->size = size;
  for(int i = 0; i < font->data->m_images_len;++i){
    if((int)font->data->images[i].m_size  == (int)font->size){
      font->height = (font->data->images[i].baseline/ scale - font->data->images[i].descent / scale + font->data->images[i].line_gap / scale) * scale + 0.5;
    }
  }

  return font;

fail:
  if (font) { kr_free(font->data); }
  kr_free(font);
  return NULL;
}


void ren_free_font(RenFont *font) {
  kr_free(font);
}


void ren_set_font_tab_width(RenFont *font, int n) {
  for(int i = 0; i < font->data->m_images_len;++i){
    if((int)font->data->images[i].m_size  == (int)font->size ){
      font->data->images[i].chars['\t'].xadvance = n;
      break;
    }
  }
}


int ren_get_font_tab_width(RenFont *font) {
  for(int i = 0; i < font->data->m_images_len;++i){
    if((int)font->data->images[i].m_size  == (int)font->size){
      return font->data->images[i].chars['\t'].xadvance;
    }
  }
}


int ren_get_font_width(RenFont *font, const char *text) {
  return kr_ttf_width(font->data,font->size,text);
}


int ren_get_font_height(RenFont *font) {
  return font->height;
}

void ren_begin_frame(void) {
  kinc_g4_begin(0);
  kr_g2_begin(0);
  kr_g2_clear(KINC_COLOR_BLACK);
  kr_g2_set_transform(kr_matrix3x3_identity());
}

void ren_end_frame(void) {
  kr_g2_end();
  kinc_g4_end(0);
}

void ren_draw_rect(RenRect rect, RenColor color) {
  kr_g2_set_color(color_to_uint(color));
  kr_g2_fill_rect(rect.x,rect.y,rect.width,rect.height);
}


void ren_draw_image(RenImage *image, RenRect *sub, int x, int y, RenColor color) {}

int ren_draw_text(RenFont *font, const char *text, int x, int y, RenColor color) {
  ren_set_font_tab_width(font, ren_get_font_tab_width(font));
  kr_g2_set_color(color_to_uint(color));
  kr_g2_set_font(font->data, font->size);
  return kr_g2_draw_string(text,x,y);
}
