#pragma once

#include <cmath>
#include <algorithm>

constexpr double M_PI = 3.14159265358979323846;
constexpr float M_RADPI = 57.295779513082f;
constexpr float M_PI_F = static_cast<float>(M_PI);

class matrix_t {
	float mat_val[3][4];
public:
	float* operator[](int i) { return mat_val[i]; }
	const float* operator[](int i) const { return mat_val[i]; }
};

struct vec3_t {
	vec3_t& operator+=(const vec3_t& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3_t& operator-=(const vec3_t& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3_t& operator*=(float v) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3_t& operator/=(float fl) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}

	vec3_t operator+(const vec3_t& v) const {
		return { x + v.x, y + v.y, z + v.z };
	}

	vec3_t operator-(const vec3_t& v) const {
		return { x - v.x, y - v.y, z - v.z };
	}

	vec3_t operator*(const vec3_t& v) const {
		return { x * v.x, y * v.y, z * v.z };
	}

	vec3_t operator*(float fl) const {
		return { x * fl, y * fl, z * fl };
	}

	vec3_t operator/(float fl) const {
		return { x / fl, y / fl, z / fl };
	}

	float& operator[](int i) {
		return ((float*)this)[i];
	}

	vec3_t& clamp() {
		if (x < -89.f)
			x = -89.f;
		if (x > 89.f)
			x = 89.f;

		if (y < -180.f)
			y += 360.f;
		if (y > 180.f)
			y -= 360.f;

		z = 0.f;

		return *this;
	}

	vec3_t& normalize() {
		x = std::isfinite(x) ? std::remainderf(x, 360.f) : 0.f;
		y = std::isfinite(y) ? std::remainderf(y, 360.f) : 0.f;
		z = 0.f;

		return *this;
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float length_sqr() const {
		return x * x + y * y + z * z;
	}

	float length_2d() const {
		return std::sqrt(x * x + y * y);
	}

	float length_2d_sqr() const {
		return x * x + y * y;
	}

	float dot(const vec3_t& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float dot(const float* fl) const {
		return x * fl[0] + y * fl[1] + z * fl[2];
	}

	vec3_t transform(const matrix_t& mat) const {
		return { dot({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
				 dot({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
				 dot({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3] };
	}

	float x, y, z;
};