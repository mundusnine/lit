#pragma once
#include "sdfpainter.h"
#include "ttf.h"
#include <kinc/global.h>
#include <krink/image.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>

#ifndef KR_G2_MAX_WIN
#define KR_G2_MAX_WIN 8
#endif

/*! \file graphics.h
    \brief Provides a 2D drawing API.

    INFO: The entire `graphics2` API is more or less a direct port of the same API in Kha
   https://github.com/Kode/Kha using the code available in Haxe and translating it to the best of my
   abilities to C. IANAL, but I suppose copyright of the original software goes to the Kha
   Development Team that wrote the original code upon which this is based and I hereby state that
   this is altered source based upon source from the afforementioned Kha Development Team.

   \note Calling `kinc_g4_begin` and `kinc_g4_end` of the windows to be rendered to
   is the users responsibility.

   \brief If KR_FULL_RGBA_FONTS is defined, krink will generate all ttf
   fonts as full RGBA textures and use the same pipeline for text that is also used for images.

   \note This is meant to be used in cases where all assets, including fonts are stored in a single
   texture atlas and thus further optimizes batching of draw calls by not having to switch pipeline,
   at the cost of more VRAM being used.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Initialize graphics2.
/// </summary>
void kr_g2_init(void);

/// <summary>
/// Destroy kr_g2. Currently does nothing!
/// </summary>
void kr_g2_destroy(void);

/// <summary>
/// Needs to be called before rendering to a window. Typically called at the start of each frame.
/// </summary>
/// <param name="window"></param>
void kr_g2_begin(int window);

/// <summary>
/// Needs to be called after `kr_g2_begin` and before rendering to a render-target that was
/// activated in kinc using `kinc_g4_set_render_targets`.
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
void kr_g2_set_render_target_dim(int width, int height);

/// <summary>
/// Needs to be called to restore rendering to a framebuffer.
/// </summary>
void kr_g2_reset_render_target_dim(void);

/// <summary>
/// Needs to be called after rendering to a window. Typically called at the end of each frame.
/// </summary>
void kr_g2_end(void);

/// <summary>
/// Clears the color, depth and/or stencil-components of the current framebuffer or render-target.
/// </summary>
/// <param name="color">The color-value to clear to</param>
void kr_g2_clear(uint32_t color);

/// <summary>
/// Set vertex color for following draw calls.
/// </summary>
/// <param name="color">The color-value to draw with</param>
void kr_g2_set_color(uint32_t color);

/// <summary>
/// Set opacity for following draw calls.
/// </summary>
/// <param name="opacity">0..1</param>
void kr_g2_set_opacity(float opacity);

/// <summary>
/// Draw a rectangle.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="strength"></param>
void kr_g2_draw_rect(float x, float y, float width, float height, float strength);

/// <summary>
/// Fill a rectangle.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void kr_g2_fill_rect(float x, float y, float width, float height);

/// <summary>
/// Fill a triangle.
/// </summary>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="x2"></param>
/// <param name="y2"></param>
/// <param name="x3"></param>
/// <param name="y3"></param>
void kr_g2_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3);

/// <summary>
/// Draw a triangle.
/// </summary>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="x2"></param>
/// <param name="y2"></param>
/// <param name="x3"></param>
/// <param name="y3"></param>
/// <param name="strength"></param>
void kr_g2_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3,
                         float strength);

/// <summary>
/// Draw a string.
/// </summary>
/// <param name="text"></param>
/// <param name="x"></param>
/// <param name="y"></param>
int kr_g2_draw_string(const char *text, float x, float y);

/// <summary>
/// Draw an array of characters.
/// </summary>
/// <param name="text">Array of character codes</param>
/// <param name="start">First index</param>
/// <param name="length">Number of characters</param>
/// <param name="x"></param>
/// <param name="y"></param>
int kr_g2_draw_characters(int *text, int start, int length, float x, float y);

/// <summary>
/// Draw a line.
/// </summary>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="x2"></param>
/// <param name="y2"></param>
/// <param name="strength"></param>
void kr_g2_draw_line(float x1, float y1, float x2, float y2, float strength);

/// <summary>
/// Draw an image.
/// </summary>
/// <param name="img">Image to draw</param>
/// <param name="sx">Source x</param>
/// <param name="sy">Source y</param>
/// <param name="sw">Source width</param>
/// <param name="sh">Source height</param>
/// <param name="dx">Destination x</param>
/// <param name="dy">Destination y</param>
/// <param name="dw">Destination width</param>
/// <param name="dh">Destination height</param>
void kr_g2_draw_scaled_sub_image(kr_image_t *img, float sx, float sy, float sw, float sh, float dx,
                                 float dy, float dw, float dh);

/// <summary>
/// Set transformation matrix for the next drawing operations.
/// </summary>
/// <param name="m"></param>
void kr_g2_set_transform(kr_matrix3x3_t m);

/// <summary>
/// Get current transformation matrix.
/// </summary>
/// <returns>The current transformation matrix</returns>
kr_matrix3x3_t kr_g2_get_transform(void);

/// <summary>
/// Get the font ptr of the current font.
/// </summary>
/// <returns>The current font</returns>
kr_ttf_font_t *kr_g2_get_font(void);

/// <summary>
/// Get the current font size.
/// </summary>
/// <returns>The current font size</returns>
int kr_g2_get_font_size(void);

/// <summary>
/// Set the current font and font size.
/// </summary>
/// <param name="font">Ptr to `kr_ttf_font_t`</param>
/// <param name="size">Font size in pixel</param>
void kr_g2_set_font(kr_ttf_font_t *font, int size);

/// <summary>
/// Draw SDF rect.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="corner"></param>
/// <param name="border"></param>
/// <param name="border_color"></param>
/// <param name="smooth"></param>
void kr_g2_draw_sdf_rect(float x, float y, float width, float height, kr_sdf_corner_radius_t corner,
                         float border, uint32_t border_color, float smooth);

/// <summary>
/// Draw SDF rect with same corner radius on all edges.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="corner"></param>
/// <param name="border"></param>
/// <param name="border_color"></param>
/// <param name="smooth"></param>
void kr_g2_draw_sdf_rect_symm(float x, float y, float width, float height, float corner,
                              float border, uint32_t border_color, float smooth);

/// <summary>
/// Draw SDF circle.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="r"></param>
/// <param name="border"></param>
/// <param name="border_color"></param>
/// <param name="smooth"></param>
void kr_g2_draw_sdf_circle(float x, float y, float r, float border, uint32_t border_color,
                           float smooth);

/// <summary>
/// Draw SDF line.
/// </summary>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="x2"></param>
/// <param name="y2"></param>
/// <param name="strength"></param>
/// <param name="smooth"></param>
void kr_g2_draw_sdf_line(float x1, float y1, float x2, float y2, float strength, float smooth);

/// <summary>
/// Enable scissor.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="w"></param>
/// <param name="h"></param>
void kr_g2_scissor(float x, float y, float w, float h);

/// <summary>
/// Disable scissor.
/// </summary>
void kr_g2_disable_scissor(void);

/// <summary>
/// Toggle the bilinear filter on/off.
/// </summary>
/// <param name="bilinear"></param>
void kr_g2_set_bilinear_filter(bool bilinear);

#ifdef __cplusplus
}
#endif
