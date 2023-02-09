#include "matrix.h"
#include <kinc/simd/float32x4.h>
#include <math.h>

// kr_matrix3x3 impl

kr_matrix3x3_t kr_matrix3x3_translation(float x, float y) {
	return (kr_matrix3x3_t){1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f};
}

kr_matrix3x3_t kr_matrix3x3_empty(void) {
	return (kr_matrix3x3_t){0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
}

kr_matrix3x3_t kr_matrix3x3_identity(void) {
	return (kr_matrix3x3_t){1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix3x3_t kr_matrix3x3_scale(float x, float y) {
	return (kr_matrix3x3_t){x, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix3x3_t kr_matrix3x3_rotation(float alpha) {
	return (kr_matrix3x3_t){cos(alpha),
	                        -sin(alpha),
	                        0.0f,
	                        sin(alpha),
	                        cos(alpha),
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        1.0f};
}

kr_matrix3x3_t kr_matrix3x3_add(kr_matrix3x3_t *mlh, kr_matrix3x3_t *mrh) {
	return (kr_matrix3x3_t){mlh->m00 + mrh->m00, mlh->m10 + mrh->m10, mlh->m20 + mrh->m20,
	                        mlh->m01 + mrh->m01, mlh->m11 + mrh->m11, mlh->m21 + mrh->m21,
	                        mlh->m02 + mrh->m02, mlh->m12 + mrh->m12, mlh->m22 + mrh->m22};
}

kr_matrix3x3_t kr_matrix3x3_sub(kr_matrix3x3_t *mlh, kr_matrix3x3_t *mrh) {
	return (kr_matrix3x3_t){mlh->m00 - mrh->m00, mlh->m10 - mrh->m10, mlh->m20 - mrh->m20,
	                        mlh->m01 - mrh->m01, mlh->m11 - mrh->m11, mlh->m21 - mrh->m21,
	                        mlh->m02 - mrh->m02, mlh->m12 - mrh->m12, mlh->m22 - mrh->m22};
}

kr_matrix3x3_t kr_matrix3x3_mult(kr_matrix3x3_t *m, float value) {
	return (kr_matrix3x3_t){m->m00 * value, m->m10 * value, m->m20 * value,
	                        m->m01 * value, m->m11 * value, m->m21 * value,
	                        m->m02 * value, m->m12 * value, m->m22 * value};
}

kr_matrix3x3_t kr_matrix3x3_transpose(kr_matrix3x3_t *m) {
	return (kr_matrix3x3_t){m->m00, m->m01, m->m02, m->m10, m->m11, m->m12, m->m20, m->m21, m->m22};
}

float kr_matrix3x3_trace(kr_matrix3x3_t *m) {
	return m->m00 + m->m11 + m->m22;
}

kr_matrix3x3_t kr_matrix3x3_multmat(kr_matrix3x3_t *mlh, kr_matrix3x3_t *mrh) {
	return (kr_matrix3x3_t){mlh->m00 * mrh->m00 + mlh->m10 * mrh->m01 + mlh->m20 * mrh->m02,
	                        mlh->m00 * mrh->m10 + mlh->m10 * mrh->m11 + mlh->m20 * mrh->m12,
	                        mlh->m00 * mrh->m20 + mlh->m10 * mrh->m21 + mlh->m20 * mrh->m22,
	                        mlh->m01 * mrh->m00 + mlh->m11 * mrh->m01 + mlh->m21 * mrh->m02,
	                        mlh->m01 * mrh->m10 + mlh->m11 * mrh->m11 + mlh->m21 * mrh->m12,
	                        mlh->m01 * mrh->m20 + mlh->m11 * mrh->m21 + mlh->m21 * mrh->m22,
	                        mlh->m02 * mrh->m00 + mlh->m12 * mrh->m01 + mlh->m22 * mrh->m02,
	                        mlh->m02 * mrh->m10 + mlh->m12 * mrh->m11 + mlh->m22 * mrh->m12,
	                        mlh->m02 * mrh->m20 + mlh->m12 * mrh->m21 + mlh->m22 * mrh->m22};
}

kr_vec2_t kr_matrix3x3_multvec(kr_matrix3x3_t *m, kr_vec2_t v) {
	float w = m->m02 * v.x + m->m12 * v.y + m->m22 * 1.0f;
	float x = (m->m00 * v.x + m->m10 * v.y + m->m20 * 1.0f) / w;
	float y = (m->m01 * v.x + m->m11 * v.y + m->m21 * 1.0f) / w;
	return (kr_vec2_t){x, y};
}

void kr_matrix3x3_multquad(kr_matrix3x3_t *m, kr_quad_t q, kr_vec2_t *out) {
	kinc_float32x4_t xx = kinc_float32x4_load(q.x, q.x, q.x + q.w, q.x + q.w);
	kinc_float32x4_t yy = kinc_float32x4_load(q.y + q.h, q.y, q.y, q.y + q.h);

	kinc_float32x4_t m00 = kinc_float32x4_load_all(m->m00);
	kinc_float32x4_t m01 = kinc_float32x4_load_all(m->m01);
	kinc_float32x4_t m02 = kinc_float32x4_load_all(m->m02);
	kinc_float32x4_t m10 = kinc_float32x4_load_all(m->m10);
	kinc_float32x4_t m11 = kinc_float32x4_load_all(m->m11);
	kinc_float32x4_t m12 = kinc_float32x4_load_all(m->m12);
	kinc_float32x4_t m20 = kinc_float32x4_load_all(m->m20);
	kinc_float32x4_t m21 = kinc_float32x4_load_all(m->m21);
	kinc_float32x4_t m22 = kinc_float32x4_load_all(m->m22);

	kinc_float32x4_t w = kinc_float32x4_add(
	    kinc_float32x4_add(kinc_float32x4_mul(m02, xx), kinc_float32x4_mul(m12, yy)), m22);
	kinc_float32x4_t px = kinc_float32x4_div(
	    kinc_float32x4_add(
	        kinc_float32x4_add(kinc_float32x4_mul(m00, xx), kinc_float32x4_mul(m10, yy)), m20),
	    w);
	kinc_float32x4_t py = kinc_float32x4_div(
	    kinc_float32x4_add(
	        kinc_float32x4_add(kinc_float32x4_mul(m01, xx), kinc_float32x4_mul(m11, yy)), m21),
	    w);

	out[0] = (kr_vec2_t){kinc_float32x4_get(px, 0), kinc_float32x4_get(py, 0)};
	out[1] = (kr_vec2_t){kinc_float32x4_get(px, 1), kinc_float32x4_get(py, 1)};
	out[2] = (kr_vec2_t){kinc_float32x4_get(px, 2), kinc_float32x4_get(py, 2)};
	out[3] = (kr_vec2_t){kinc_float32x4_get(px, 3), kinc_float32x4_get(py, 3)};
}

float kr_matrix3x3_cofactor(float m0, float m1, float m2, float m3) {
	return m0 * m3 - m1 * m2;
}

float kr_matrix3x3_determinant(kr_matrix3x3_t *m) {
	float c00 = kr_matrix3x3_cofactor(m->m11, m->m21, m->m12, m->m22);
	float c01 = kr_matrix3x3_cofactor(m->m10, m->m20, m->m12, m->m22);
	float c02 = kr_matrix3x3_cofactor(m->m10, m->m20, m->m11, m->m21);
	return m->m00 * c00 - m->m01 * c01 + m->m02 * c02;
}

kr_matrix3x3_t kr_matrix3x3_inverse(kr_matrix3x3_t *m) {
	float c00 = kr_matrix3x3_cofactor(m->m11, m->m21, m->m12, m->m22);
	float c01 = kr_matrix3x3_cofactor(m->m10, m->m20, m->m12, m->m22);
	float c02 = kr_matrix3x3_cofactor(m->m10, m->m20, m->m11, m->m21);

	float det = m->m00 * c00 - m->m01 * c01 + m->m02 * c02;
	if (kinc_abs(det) < 0.000001f) return kr_matrix3x3_empty();

	float c10 = kr_matrix3x3_cofactor(m->m01, m->m21, m->m02, m->m22);
	float c11 = kr_matrix3x3_cofactor(m->m00, m->m20, m->m02, m->m22);
	float c12 = kr_matrix3x3_cofactor(m->m00, m->m20, m->m01, m->m21);

	float c20 = kr_matrix3x3_cofactor(m->m01, m->m11, m->m02, m->m12);
	float c21 = kr_matrix3x3_cofactor(m->m00, m->m10, m->m02, m->m12);
	float c22 = kr_matrix3x3_cofactor(m->m00, m->m10, m->m01, m->m11);

	float invdet = 1.0f / det;
	return (kr_matrix3x3_t){c00 * invdet,  -c01 * invdet, c02 * invdet,
	                        -c10 * invdet, c11 * invdet,  -c12 * invdet,
	                        c20 * invdet,  -c21 * invdet, c22 * invdet};
}

kinc_matrix3x3_t kr_matrix3x3_to_kinc(kr_matrix3x3_t *m) {
	kinc_matrix3x3_t value;
	kinc_matrix3x3_set(&value, 0, 0, m->m00);
	kinc_matrix3x3_set(&value, 1, 0, m->m10);
	kinc_matrix3x3_set(&value, 2, 0, m->m20);
	kinc_matrix3x3_set(&value, 0, 1, m->m01);
	kinc_matrix3x3_set(&value, 1, 1, m->m11);
	kinc_matrix3x3_set(&value, 2, 1, m->m21);
	kinc_matrix3x3_set(&value, 0, 2, m->m02);
	kinc_matrix3x3_set(&value, 1, 2, m->m12);
	kinc_matrix3x3_set(&value, 2, 2, m->m22);
	return value;
}

// kr_matrix4x4 impl

kr_matrix4x4_t kr_matrix4x4_translation(float x, float y, float z) {
	return (kr_matrix4x4_t){1.0f, 0.0f, 0.0f, x, 0.0f, 1.0f, 0.0f, y,
	                        0.0f, 0.0f, 1.0f, z, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix4x4_t kr_matrix4x4_empty(void) {
	return (kr_matrix4x4_t){0.0f};
}

kr_matrix4x4_t kr_matrix4x4_identity(void) {
	return (kr_matrix4x4_t){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	                        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix4x4_t kr_matrix4x4_scale(float x, float y, float z) {
	return (kr_matrix4x4_t){x,    0.0f, 0.0f, 0.0f, 0.0f, y,    0.0f, 0.0f,
	                        0.0f, 0.0f, z,    0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix4x4_t kr_matrix4x4_rotation_x(float alpha) {
	float ca = cos(alpha);
	float sa = sin(alpha);
	return (kr_matrix4x4_t){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, ca,   -sa,  0.0f,
	                        0.0f, sa,   ca,   0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix4x4_t kr_matrix4x4_rotation_y(float alpha) {
	float ca = cos(alpha);
	float sa = sin(alpha);
	return (kr_matrix4x4_t){ca,  0.0f, sa, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	                        -sa, 0.0f, ca, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix4x4_t kr_matrix4x4_rotation_z(float alpha) {
	float ca = cos(alpha);
	float sa = sin(alpha);
	return (kr_matrix4x4_t){ca,   -sa,  0.0f, 0.0f, sa,   ca,   0.0f, 0.0f,
	                        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

kr_matrix4x4_t kr_matrix4x4_rotation(float yaw, float pitch, float roll) {
	float sy = sin(yaw);
	float cy = cos(yaw);
	float sx = sin(pitch);
	float cx = cos(pitch);
	float sz = sin(roll);
	float cz = cos(roll);
	return (kr_matrix4x4_t){cx * cy,
	                        cx * sy * sz - sx * cz,
	                        cx * sy * cz + sx * sz,
	                        0,
	                        sx * cy,
	                        sx * sy * sz + cx * cz,
	                        sx * sy * cz - cx * sz,
	                        0,
	                        -sy,
	                        cy * sz,
	                        cy * cz,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        1.0f};
}

kr_matrix4x4_t kr_matrix4x4_orthogonal_projection(float left, float right, float bottom, float top,
                                                  float zn, float zf) {
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(zf + zn) / (zf - zn);
	return (kr_matrix4x4_t){2.0f / (right - left),
	                        0.0f,
	                        0.0f,
	                        tx,
	                        0.0f,
	                        2.0f / (top - bottom),
	                        0.0f,
	                        ty,
	                        0.0f,
	                        0.0f,
	                        -2.0f / (zf - zn),
	                        tz,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        1.0f};
}

kr_matrix4x4_t kr_matrix4x4_perspective_projection(float fov_y, float aspect, float zn, float zf) {
	float uh = 1.0 / tan(fov_y / 2.0f);
	float uw = uh / aspect;
	return (kr_matrix4x4_t){uw,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        uh,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        0.0f,
	                        (zf + zn) / (zn - zf),
	                        2.0f * zf * zn / (zn - zf),
	                        0.0f,
	                        0.0f,
	                        -1.0f,
	                        0.0f};
}

kr_matrix4x4_t kr_matrix4x4_look_at(kr_vec3_t eye, kr_vec3_t at, kr_vec3_t up) {
	kr_vec3_t zaxis = kr_vec3_normalized(kr_vec3_subv(at, eye));
	kr_vec3_t xaxis = kr_vec3_normalized(kr_vec3_cross(zaxis, up));
	kr_vec3_t yaxis = kr_vec3_cross(xaxis, zaxis);

	return (kr_matrix4x4_t){xaxis.x,  xaxis.y,  xaxis.z,  -kr_vec3_dot(xaxis, eye),
	                        yaxis.x,  yaxis.y,  yaxis.z,  -kr_vec3_dot(yaxis, eye),
	                        -zaxis.x, -zaxis.y, -zaxis.z, kr_vec3_dot(zaxis, eye),
	                        0.0f,     0.0f,     0.0f,     1.0f};
}

kr_matrix4x4_t kr_matrix4x4_add(kr_matrix4x4_t *mlh, kr_matrix4x4_t *mrh) {
	return (kr_matrix4x4_t){
	    mlh->m00 + mrh->m00, mlh->m10 + mrh->m10, mlh->m20 + mrh->m20, mlh->m30 + mrh->m30,
	    mlh->m01 + mrh->m01, mlh->m11 + mrh->m11, mlh->m21 + mrh->m21, mlh->m31 + mrh->m31,
	    mlh->m02 + mrh->m02, mlh->m12 + mrh->m12, mlh->m22 + mrh->m22, mlh->m32 + mrh->m32,
	    mlh->m03 + mrh->m03, mlh->m13 + mrh->m13, mlh->m23 + mrh->m23, mlh->m33 + mrh->m33};
}

kr_matrix4x4_t kr_matrix4x4_sub(kr_matrix4x4_t *mlh, kr_matrix4x4_t *mrh) {
	return (kr_matrix4x4_t){
	    mlh->m00 - mrh->m00, mlh->m10 - mrh->m10, mlh->m20 - mrh->m20, mlh->m30 - mrh->m30,
	    mlh->m01 - mrh->m01, mlh->m11 - mrh->m11, mlh->m21 - mrh->m21, mlh->m31 - mrh->m31,
	    mlh->m02 - mrh->m02, mlh->m12 - mrh->m12, mlh->m22 - mrh->m22, mlh->m32 - mrh->m32,
	    mlh->m03 - mrh->m03, mlh->m13 - mrh->m13, mlh->m23 - mrh->m23, mlh->m33 - mrh->m33};
}

kr_matrix4x4_t kr_matrix4x4_mult(kr_matrix4x4_t *m, float value) {
	return (kr_matrix4x4_t){m->m00 * value, m->m10 * value, m->m20 * value, m->m30 * value,
	                        m->m01 * value, m->m11 * value, m->m21 * value, m->m31 * value,
	                        m->m02 * value, m->m12 * value, m->m22 * value, m->m32 * value,
	                        m->m03 * value, m->m13 * value, m->m23 * value, m->m33 * value};
}

kr_matrix4x4_t kr_matrix4x4_transpose(kr_matrix4x4_t *m) {
	return (kr_matrix4x4_t){m->m00, m->m01, m->m02, m->m03, m->m10, m->m11, m->m12, m->m13,
	                        m->m20, m->m21, m->m22, m->m23, m->m30, m->m31, m->m32, m->m33};
}

kr_matrix4x4_t kr_matrix4x4_transpose3x3(kr_matrix4x4_t *m) {
	return (kr_matrix4x4_t){m->m00, m->m01, m->m02, m->m30, m->m10, m->m11, m->m12, m->m31,
	                        m->m20, m->m21, m->m22, m->m32, m->m03, m->m13, m->m23, m->m33};
}

float kr_matrix4x4_trace(kr_matrix4x4_t *m) {
	return m->m00 + m->m11 + m->m22 + m->m33;
}

kr_matrix4x4_t kr_matrix4x4_multmat(kr_matrix4x4_t *mlh, kr_matrix4x4_t *mrh) {
	return (kr_matrix4x4_t){
	    mlh->m00 * mrh->m00 + mlh->m10 * mrh->m01 + mlh->m20 * mrh->m02 + mlh->m30 * mrh->m03,
	    mlh->m00 * mrh->m10 + mlh->m10 * mrh->m11 + mlh->m20 * mrh->m12 + mlh->m30 * mrh->m13,
	    mlh->m00 * mrh->m20 + mlh->m10 * mrh->m21 + mlh->m20 * mrh->m22 + mlh->m30 * mrh->m23,
	    mlh->m00 * mrh->m30 + mlh->m10 * mrh->m31 + mlh->m20 * mrh->m32 + mlh->m30 * mrh->m33,
	    mlh->m01 * mrh->m00 + mlh->m11 * mrh->m01 + mlh->m21 * mrh->m02 + mlh->m31 * mrh->m03,
	    mlh->m01 * mrh->m10 + mlh->m11 * mrh->m11 + mlh->m21 * mrh->m12 + mlh->m31 * mrh->m13,
	    mlh->m01 * mrh->m20 + mlh->m11 * mrh->m21 + mlh->m21 * mrh->m22 + mlh->m31 * mrh->m23,
	    mlh->m01 * mrh->m30 + mlh->m11 * mrh->m31 + mlh->m21 * mrh->m32 + mlh->m31 * mrh->m33,
	    mlh->m02 * mrh->m00 + mlh->m12 * mrh->m01 + mlh->m22 * mrh->m02 + mlh->m32 * mrh->m03,
	    mlh->m02 * mrh->m10 + mlh->m12 * mrh->m11 + mlh->m22 * mrh->m12 + mlh->m32 * mrh->m13,
	    mlh->m02 * mrh->m20 + mlh->m12 * mrh->m21 + mlh->m22 * mrh->m22 + mlh->m32 * mrh->m23,
	    mlh->m02 * mrh->m30 + mlh->m12 * mrh->m31 + mlh->m22 * mrh->m32 + mlh->m32 * mrh->m33,
	    mlh->m03 * mrh->m00 + mlh->m13 * mrh->m01 + mlh->m23 * mrh->m02 + mlh->m33 * mrh->m03,
	    mlh->m03 * mrh->m10 + mlh->m13 * mrh->m11 + mlh->m23 * mrh->m12 + mlh->m33 * mrh->m13,
	    mlh->m03 * mrh->m20 + mlh->m13 * mrh->m21 + mlh->m23 * mrh->m22 + mlh->m33 * mrh->m23,
	    mlh->m03 * mrh->m30 + mlh->m13 * mrh->m31 + mlh->m23 * mrh->m32 + mlh->m33 * mrh->m33};
}

kr_vec4_t kr_matrix4x4_multvec(kr_matrix4x4_t *m, kr_vec4_t v) {
	kr_vec4_t product;
	product.x = m->m00 * v.x + m->m10 * v.y + m->m20 * v.z + m->m30 * v.w;
	product.y = m->m01 * v.x + m->m11 * v.y + m->m21 * v.z + m->m31 * v.w;
	product.z = m->m02 * v.x + m->m12 * v.y + m->m22 * v.z + m->m32 * v.w;
	product.w = m->m03 * v.x + m->m13 * v.y + m->m23 * v.z + m->m33 * v.w;
	return product;
}

float kr_matrix4x4_cofactor(float m0, float m1, float m2, float m3, float m4, float m5, float m6,
                            float m7, float m8) {
	return m0 * (m4 * m8 - m5 * m7) - m1 * (m3 * m8 - m5 * m6) + m2 * (m3 * m7 - m4 * m6);
}

float kr_matrix4x4_determinant(kr_matrix4x4_t *m) {
	float c00 = kr_matrix4x4_cofactor(m->m11, m->m21, m->m31, m->m12, m->m22, m->m32, m->m13,
	                                  m->m23, m->m33);
	float c01 = kr_matrix4x4_cofactor(m->m10, m->m20, m->m30, m->m12, m->m22, m->m32, m->m13,
	                                  m->m23, m->m33);
	float c02 = kr_matrix4x4_cofactor(m->m10, m->m20, m->m30, m->m11, m->m21, m->m31, m->m13,
	                                  m->m23, m->m33);
	float c03 = kr_matrix4x4_cofactor(m->m10, m->m20, m->m30, m->m11, m->m21, m->m31, m->m12,
	                                  m->m22, m->m32);
	return m->m00 * c00 - m->m01 * c01 + m->m02 * c02 - m->m03 * c03;
}

kr_matrix4x4_t kr_matrix4x4_inverse(kr_matrix4x4_t *m) {
	float c00 = kr_matrix4x4_cofactor(m->m11, m->m21, m->m31, m->m12, m->m22, m->m32, m->m13,
	                                  m->m23, m->m33);
	float c01 = kr_matrix4x4_cofactor(m->m10, m->m20, m->m30, m->m12, m->m22, m->m32, m->m13,
	                                  m->m23, m->m33);
	float c02 = kr_matrix4x4_cofactor(m->m10, m->m20, m->m30, m->m11, m->m21, m->m31, m->m13,
	                                  m->m23, m->m33);
	float c03 = kr_matrix4x4_cofactor(m->m10, m->m20, m->m30, m->m11, m->m21, m->m31, m->m12,
	                                  m->m22, m->m32);

	float det = m->m00 * c00 - m->m01 * c01 + m->m02 * c02 - m->m03 * c03;
	if (kinc_abs(det) < 0.000001f) {
		return kr_matrix4x4_empty();
	}

	float c10 = kr_matrix4x4_cofactor(m->m01, m->m21, m->m31, m->m02, m->m22, m->m32, m->m03,
	                                  m->m23, m->m33);
	float c11 = kr_matrix4x4_cofactor(m->m00, m->m20, m->m30, m->m02, m->m22, m->m32, m->m03,
	                                  m->m23, m->m33);
	float c12 = kr_matrix4x4_cofactor(m->m00, m->m20, m->m30, m->m01, m->m21, m->m31, m->m03,
	                                  m->m23, m->m33);
	float c13 = kr_matrix4x4_cofactor(m->m00, m->m20, m->m30, m->m01, m->m21, m->m31, m->m02,
	                                  m->m22, m->m32);

	float c20 = kr_matrix4x4_cofactor(m->m01, m->m11, m->m31, m->m02, m->m12, m->m32, m->m03,
	                                  m->m13, m->m33);
	float c21 = kr_matrix4x4_cofactor(m->m00, m->m10, m->m30, m->m02, m->m12, m->m32, m->m03,
	                                  m->m13, m->m33);
	float c22 = kr_matrix4x4_cofactor(m->m00, m->m10, m->m30, m->m01, m->m11, m->m31, m->m03,
	                                  m->m13, m->m33);
	float c23 = kr_matrix4x4_cofactor(m->m00, m->m10, m->m30, m->m01, m->m11, m->m31, m->m02,
	                                  m->m12, m->m32);

	float c30 = kr_matrix4x4_cofactor(m->m01, m->m11, m->m21, m->m02, m->m12, m->m22, m->m03,
	                                  m->m13, m->m23);
	float c31 = kr_matrix4x4_cofactor(m->m00, m->m10, m->m20, m->m02, m->m12, m->m22, m->m03,
	                                  m->m13, m->m23);
	float c32 = kr_matrix4x4_cofactor(m->m00, m->m10, m->m20, m->m01, m->m11, m->m21, m->m03,
	                                  m->m13, m->m23);
	float c33 = kr_matrix4x4_cofactor(m->m00, m->m10, m->m20, m->m01, m->m11, m->m21, m->m02,
	                                  m->m12, m->m22);

	float invdet = 1.0f / det;
	return (kr_matrix4x4_t){c00 * invdet,  -c01 * invdet, c02 * invdet,  -c03 * invdet,
	                        -c10 * invdet, c11 * invdet,  -c12 * invdet, c13 * invdet,
	                        c20 * invdet,  -c21 * invdet, c22 * invdet,  -c23 * invdet,
	                        -c30 * invdet, c31 * invdet,  -c32 * invdet, c33 * invdet};
}

kinc_matrix4x4_t kr_matrix4x4_to_kinc(kr_matrix4x4_t *m) {
	kinc_matrix4x4_t value;
	kinc_matrix4x4_set(&value, 0, 0, m->m00);
	kinc_matrix4x4_set(&value, 1, 0, m->m10);
	kinc_matrix4x4_set(&value, 2, 0, m->m20);
	kinc_matrix4x4_set(&value, 3, 0, m->m30);
	kinc_matrix4x4_set(&value, 0, 1, m->m01);
	kinc_matrix4x4_set(&value, 1, 1, m->m11);
	kinc_matrix4x4_set(&value, 2, 1, m->m21);
	kinc_matrix4x4_set(&value, 3, 1, m->m31);
	kinc_matrix4x4_set(&value, 0, 2, m->m02);
	kinc_matrix4x4_set(&value, 1, 2, m->m12);
	kinc_matrix4x4_set(&value, 2, 2, m->m22);
	kinc_matrix4x4_set(&value, 3, 2, m->m32);
	kinc_matrix4x4_set(&value, 0, 3, m->m03);
	kinc_matrix4x4_set(&value, 1, 3, m->m13);
	kinc_matrix4x4_set(&value, 2, 3, m->m23);
	kinc_matrix4x4_set(&value, 3, 3, m->m33);
	return value;
}
