#pragma once

#include "../../dependencies/utilities/virtual_method.h"

enum class cs_weapon_type;

struct weapon_info_t {
	pad(20)
	int max_clip;
	pad(104)
	const char* bullet_type;
	pad(4)
	const char* name;
	pad(60)
	cs_weapon_type type;
	pad(16)
	float cycle_time;
	pad(0x10)
	int damage;
	float armor_ratio;
	int bullets;
	float penetration;
	pad(8)
	float range;
	float range_modifier;
	pad(32)
	float max_speed;
	float max_speed_alt;
	pad(100)
	float recoil_magnitude;
	float recoil_magnitude_alt;
	pad(0x14)
	float recovery_time_stand;
};