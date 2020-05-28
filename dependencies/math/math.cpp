#include "../utilities/csgo.hpp"
#include "../../dependencies/interfaces/iv_engine_client.hpp"
#include "../../source-sdk/math/vector2d.hpp"
#include "../../source-sdk/math/vector3d.hpp"

void math::angle_vectors(const vec3_t& angles, vec3_t* forward, vec3_t* right, vec3_t* up) {
	const float radians_x = degrees_to_radians(angles.x);
	const float radians_y = degrees_to_radians(angles.y);

	const float sp = std::sin(radians_x);
	const float cp = std::cos(radians_x);
	const float sy = std::sin(radians_y);
	const float cy = std::cos(radians_y);

	*forward = { cp * cy,  cp * sy, -sp };

	if (right || up) {
		const float radians_z = degrees_to_radians(angles.z);

		const float sr = std::sin(radians_z);
		const float cr = std::cos(radians_z);

		if (right)
			*right = { -1.f * sr * sp * cy + -1.f * cr * -sy, -1.f * sr * sp * sy + -1.f * cr * cy, -1.f * sr * cp };

		if (up)
			*up = { cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp };
	}
}

vec3_t math::calculate_angle(const vec3_t& src, const vec3_t& dest) {
	const vec3_t delta = src - dest;
	vec3_t angles{ radians_to_degrees(std::atanf(delta.z / delta.length_2d())),
				   radians_to_degrees(std::atanf(delta.y / delta.x)), 0.f };

	if (delta.x >= 0.f)
		angles.y += 180.f;

	return angles;
}

//aimtux
void math::correct_movement(vec3_t old_angles, c_usercmd* cmd, float old_forwardmove, float old_sidemove) {
	float delta_view = 0.f;
	float f1 = old_angles.y;
	float f2 = cmd->view_angles.y;

	if (old_angles.y < 0.f)
		f1 += 360.f;

	if (cmd->view_angles.y < 0.f)
		f2 += 360.f;

	if (f2 < f1)
		delta_view = std::abs(f2 - f1);
	else
		delta_view = 360.f - std::abs(f1 - f2);

	delta_view = 360.f - delta_view;

	cmd->forward_move = std::cos(degrees_to_radians(delta_view)) * old_forwardmove + std::cos(degrees_to_radians(delta_view + 90.f)) * old_sidemove;
	cmd->side_move = std::sin(degrees_to_radians(delta_view)) * old_forwardmove + std::sin(degrees_to_radians(delta_view + 90.f)) * old_sidemove;
}

void math::transform_vector(const vec3_t& a, const matrix_t& b, vec3_t& out) {
	out.x = a.dot(b[0]) + b[0][3];
	out.y = a.dot(b[1]) + b[1][3];
	out.z = a.dot(b[2]) + b[2][3];
}

static D3DMATRIX view_matrix;

void math::update_view_matrix() {
	view_matrix = interfaces::engine->world_to_screen_matrix();
}

void math::vector_angles(const vec3_t& in, vec3_t& angles) {
	if (in.y == 0.f && in.x == 0.f) {
		angles.x = in.z > 0.f ? 270.f : 90.f;
		angles.y = 0.f;
	}
	else {
		angles.x = radians_to_degrees(std::atan2(-in.z, in.length_2d()));
		angles.y = radians_to_degrees(std::atan2(in.y, in.x));

		if (angles.x < 0.f)
			angles.x += 360.f;
		if (angles.y < 0.f)
			angles.y += 360.f;
	}

	angles.x -= std::floor(angles.x / 360.f + .5f) * 360.f;
	angles.y -= std::floor(angles.y / 360.f + .5f) * 360.f;

	if (angles.x > 89.f)
		angles.x = 89.f;
	else if (angles.x < -89.f)
		angles.x = -89.f;
}

bool math::world_to_screen(const vec3_t& in, vec2_t& out) {
	const D3DMATRIX& matrix = view_matrix;

	float w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;

	if (w > .001f) {
		int width, height;
		interfaces::engine->get_screen_size(width, height);

		out.x = width / 2 * (1 + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w);
		out.y = height / 2 * (1 - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w);

		return true;
	}

	return false;
}