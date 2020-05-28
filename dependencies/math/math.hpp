#pragma once

#include "../interfaces/interfaces.hpp"
#include "../interfaces/c_global_vars_base.hpp"

struct c_usercmd;
class matrix_t;
struct vec2_t;
struct vec3_t;

constexpr double m_pi = 3.14159265358979323846;
constexpr float M_RADPI = 57.295779513082f;

constexpr auto degrees_to_radians = [](float degrees) constexpr { return degrees * static_cast<float>(m_pi) / 180.f; };
constexpr auto radians_to_degrees = [](float radians) constexpr { return radians * 180.f / static_cast<float>(m_pi); };

constexpr auto time_to_ticks = [](float time) constexpr { return static_cast<int>(.5f + time / interfaces::global_vars->interval_per_tick); };
constexpr auto ticks_to_time = [](int ticks) constexpr { return interfaces::global_vars->interval_per_tick * static_cast<float>(ticks); };

namespace math {
	void angle_vectors(const vec3_t&, vec3_t*, vec3_t* = nullptr, vec3_t* = nullptr);
	vec3_t calculate_angle(const vec3_t& src, const vec3_t& dest);
	void correct_movement(vec3_t old_angles, c_usercmd* cmd, float old_forwardmove, float old_sidemove);
	void transform_vector(const vec3_t&, const matrix_t&, vec3_t&);
	void update_view_matrix();
	void vector_angles(const vec3_t& in, vec3_t& out);
	bool world_to_screen(const vec3_t& in, vec2_t& out);
}