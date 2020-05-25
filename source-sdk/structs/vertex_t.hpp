#pragma once

#include "../math/vector2d.hpp"

struct vertex_t {
	vertex_t() = default;

	vertex_t(const vec2_t& pos, const vec2_t& coord = { 0, 0 }) {
		position = pos;
		tex_coord = coord;
	}

	vec2_t position;
	vec2_t tex_coord;
};