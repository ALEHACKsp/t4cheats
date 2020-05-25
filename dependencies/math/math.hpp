#pragma once

class matrix_t;
struct vec3_t;

constexpr auto degrees_to_radians = [](float degrees) constexpr noexcept { return degrees * static_cast<float>(M_PI) / 180.f; };
constexpr auto radians_to_degrees = [](float radians) constexpr noexcept { return radians * 180.f / static_cast<float>(M_PI); };

constexpr auto time_to_ticks = [](float time) constexpr noexcept { return static_cast<int>(.5f + time / interfaces::global_vars->interval_per_tick); };
constexpr auto ticks_to_time = [](int ticks) constexpr noexcept { return interfaces::global_vars->interval_per_tick * static_cast<float>(ticks); };

namespace math {
	void correct_movement(vec3_t old_angles, c_usercmd* cmd, float old_forwardmove, float old_sidemove);
	vec3_t calculate_angle(vec3_t& a, vec3_t& b);
	void sin_cos(float r, float* s, float* c);
	vec3_t angle_vector(vec3_t angle);
	void transform_vector(const vec3_t&, const matrix_t&, vec3_t&);
	void vector_angles(const vec3_t& in, vec3_t& out);
	void angle_vectors(const vec3_t&, vec3_t*, vec3_t* = nullptr, vec3_t* = nullptr);

	void update_view_matrix();
	bool world_to_screen(const vec3_t& origin, vec3_t& screen);
};