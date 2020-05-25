#pragma once

#include <cmath>

struct vec2_t {
	vec2_t operator*(const float n) const {
		return { x * n, y * n };
	}

	vec2_t operator+(const vec2_t& v) const {
		return { x + v.x, y + v.y };
	}

	vec2_t operator-(const vec2_t& v) const {
		return { x - v.x, y - v.y };
	}

	vec2_t& operator+=(const vec2_t& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	vec2_t& operator-=(const vec2_t& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	bool operator==(const vec2_t & v) const {
		return v.x == x && v.y == y;
	}

	float length() {
		return std::sqrt(x * x + y * y);
	}

	float x, y;
};