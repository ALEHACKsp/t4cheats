#include "../utilities/csgo.hpp"

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

vec3_t math::calculate_angle(vec3_t& a, vec3_t& b) {
	vec3_t angles;
	vec3_t delta;
	delta.x = (a.x - b.x);
	delta.y = (a.y - b.y);
	delta.z = (a.z - b.z);

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
	angles.x = (float)(atanf(delta.z / hyp) * 57.295779513082f);
	angles.y = (float)(atanf(delta.y / delta.x) * 57.295779513082f);

	angles.z = 0.0f;
	if (delta.x >= 0.0) { angles.y += 180.0f; }
	return angles;
}

void math::sin_cos(float r, float* s, float* c) {
	*s = std::sin(r);
	*c = std::cos(r);
}

vec3_t math::angle_vector(vec3_t angle) {
	auto sy = std::sin(angle.y / 180.f * static_cast<float>(M_PI));
	auto cy = std::cos(angle.y / 180.f * static_cast<float>(M_PI));

	auto sp = std::sin(angle.x / 180.f * static_cast<float>(M_PI));
	auto cp = std::cos(angle.x / 180.f * static_cast<float>(M_PI));

	return { cp * cy, cp * sy, -sp };
}

void math::transform_vector(const vec3_t& a, const matrix_t& b, vec3_t& out) {
	out.x = a.dot(b[0]) + b[0][3];
	out.y = a.dot(b[1]) + b[1][3];
	out.z = a.dot(b[2]) + b[2][3];
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

	angles.x -= std::floorf(angles.x / 360.f + .5f) * 360.f;
	angles.y -= std::floorf(angles.y / 360.f + .5f) * 360.f;

	if (angles.x > 89.f)
		angles.x = 89.f;
	else if (angles.x < -89.f)
		angles.x = -89.f;
}

void math::angle_vectors(const vec3_t& angles, vec3_t* forward, vec3_t * right, vec3_t * up) {
	const float radians_x = degrees_to_radians(angles.x);
	const float radians_y = degrees_to_radians(angles.y);

	const float sp = std::sin(radians_x);
	const float cp = std::cos(radians_x);
	const float sy = std::sin(radians_y);
	const float cy = std::cos(radians_y);

	*forward = vec3_t{ cp * cy,  cp * sy, -sp };

	if (right || up) {
		const float radians_z = degrees_to_radians(angles.z);

		const float sr = std::sin(radians_z);
		const float cr = std::cos(radians_z);

		if (right)
			*right = vec3_t{ -1.f * sr * sp * cy + -1.f * cr * -sy, -1.f * sr * sp * sy + -1.f * cr * cy, -1.f * sr * cp };

		if (up)
			*up = vec3_t{ cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp };
	}
}

static D3DMATRIX view_matrix;

void math::update_view_matrix() {
	view_matrix = interfaces::engine->world_to_screen_matrix();
}

bool math::world_to_screen(const vec3_t& point, vec3_t& screen) {
	const D3DMATRIX& matrix = view_matrix;

	float w = matrix._41 * point.x + matrix._42 * point.y + matrix._43 * point.z + matrix._44;

	if (w > .001f) {
		int width, height;
		interfaces::engine->get_screen_size(width, height);

		screen.x = width / 2 * (1 + (matrix._11 * point.x + matrix._12 * point.y + matrix._13 * point.z + matrix._14) / w);
		screen.y = height / 2 * (1 - (matrix._21 * point.x + matrix._22 * point.y + matrix._23 * point.z + matrix._24) / w);

		return true;
	}

	return false;
}