#pragma once

struct vec3_t;

struct c_usercmd {
	enum buttons : int {
		in_jump = 1 << 1
	};

	int pad;
	int command_number;
	int tick_count;
	vec3_t view_angles;
	vec3_t aim_direction;
	float forward_move;
	float side_move;
	float up_move;
	int buttons;
	char impulse;
	int weapon_select;
	int weapon_subtype;
	int random_seed;
	short mouse_dx;
	short mouse_dy;
	bool has_been_predicted;
};