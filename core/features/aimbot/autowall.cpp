#include "../features.hpp"

/*

	idk where all this is from, generally every free cheat has this sh#t
	if there is a original owner to this, let me know

*/

#define HITGROUP_GENERIC  0
#define HITGROUP_HEAD     1
#define HITGROUP_CHEST    2
#define HITGROUP_STOMACH  3
#define HITGROUP_LEFTARM  4
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG  6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR     10

bool penetration = false;

struct fire_bullet_data {
	fire_bullet_data(const vec3_t& eye_pos) : src(eye_pos) { }
	vec3_t src;
	trace_t enter_trace;
	vec3_t direction;
	trace_filter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

void bullet_type(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type) { //old pen system, sucks ass
	if (sv_penetration_type) {
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else {
		if (!strcmp(bulletType, ("BULLET_PLAYER_338MAG"))) {
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_762MM"))) {
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_556MM")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_BOX"))) {
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_57MM"))) {
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_50AE"))) {
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_357SIG")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_P250")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_MIN"))) {
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_9MM"))) {
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_45ACP"))) {
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_BUCKSHOT"))) {
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}

void trace_line(vec3_t& start, vec3_t& end, unsigned int mask, player_t* ignore, trace_t* ptr) {
	ray_t ray; ray.initialize(start, end);
	trace_filter filter; filter.skip = ignore;
	interfaces::trace_ray->trace_ray(ray, mask, &filter, ptr);
}

void clip_trace_to_player(const vec3_t& start, const vec3_t end, unsigned int mask, trace_filter* filter, trace_t* tr) {
	static auto address = reinterpret_cast<DWORD>(utilities::pattern_scan("client_panorama.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10"));

	if (!address)
		return;

	_asm {
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, end
		LEA		ECX, start
		CALL	address
		ADD		ESP, 0xC
	}
}

void scale_damage(trace_t& trace, weapon_info_t* weapon_data, float& current_damage)
{
	auto armor_value = trace.entity->armor();
	auto hitGroup = trace.hitGroup;

	auto has_armor = [&trace]()->bool {
		auto target = trace.entity;
		switch (trace.hitGroup) {
			case HITGROUP_HEAD:
				return target->has_helmet();
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				return target->armor() > 0;
			default:
				return false;
		}
	};

	switch (hitGroup) {
		case HITGROUP_HEAD: current_damage *= 4.f; break;
		case HITGROUP_STOMACH: current_damage *= 1.25f; break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG: current_damage *= 0.75f; break;
		default: break;
	}

	if (armor_value > 0 && has_armor()) {
		auto bonus_value = 1.f, armor_bonus = 0.5f, armor_ratio = weapon_data->flArmorRatio * .5f;

		if (trace.entity->has_heavy_armor()) {
			armor_bonus = 0.33f;
			armor_ratio *= 0.5f;
			bonus_value = 0.33f;
		}

		auto new_damage = current_damage * armor_ratio;

		if (((current_damage - (current_damage * armor_ratio)) * (bonus_value * armor_bonus)) > armor_value)
			new_damage = current_damage - (armor_value / armor_bonus);

		current_damage = new_damage;
	}
}

bool is_breakable(player_t* entity) {
	c_client_class* client_class = entity->client_class();
	if (!client_class)
		return false;
	return client_class->class_id == cbreakableprop || client_class->class_id == cbreakablesurface;
}

bool trace_to_exit(trace_t& enter, trace_t& exit, vec3_t start_pos, vec3_t direction) {
	vec3_t start, end;
	auto current_distance = 0.f;
	auto first_contents = 0;

	while (current_distance <= 90.f) {
		current_distance += 4.f;
		start = start_pos + direction * current_distance;
		if (!first_contents)
			first_contents = interfaces::trace_ray->get_point_contents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		auto point_contents = interfaces::trace_ray->get_point_contents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(point_contents & MASK_SHOT_HULL) || point_contents & CONTENTS_HITBOX && point_contents != first_contents) {
			end = start - (direction * 4.f);
			trace_line(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exit);
			if (exit.startSolid && exit.surface.flags & SURF_HITBOX) {
				trace_line(start, start_pos, MASK_SHOT_HULL, exit.entity, &exit);
				if (exit.did_hit() && !exit.startSolid) {
					start = exit.end;
					return true;
				}
				continue;
			}

			if (exit.did_hit() && !exit.startSolid) {
				if (is_breakable(enter.entity) && is_breakable(exit.entity))
					return true;
				if (enter.surface.flags & SURF_NODRAW || !(exit.surface.flags & SURF_NODRAW) && (exit.plane.normal.dot(direction) <= 1.f)) {
					float multAmount = exit.flFraction * 4.f;
					start -= direction * multAmount;
					return true;
				}
				continue;
			}

			if (!exit.did_hit() || exit.startSolid) {
				if (enter.did_hit_non_world_entity() && is_breakable(enter.entity)) {
					exit = enter;
					exit.end = start + direction;
					return true;
				}
				continue;
			}
		}
	}
	return false;
}

bool handle_bullet_penetration(player_t* player, weapon_info_t* weapon_data, trace_t& trace, vec3_t& start, vec3_t direction, int& possible_hits, float& current_damage, float penetration_power, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration) {
	if (&current_damage == nullptr) {
		penetration = false;
		return false;
	}

	fire_bullet_data data(start);
	data.filter = trace_filter();
	data.filter.skip = player;
	trace_t exit_trace;
	player_t* target = trace.entity;
	surface_data_t* enter_surface_data = interfaces::physics_surface->get_surface_data(trace.surface.surface_props);
	int enter_material = enter_surface_data->game_props.material;

	float thickness, modifier, lost_damage, final_damage_modifier, combined_penetration_modifier;
	bool is_solid_surf = ((trace.contents >> 3) & CONTENTS_SOLID);
	bool is_light_surf = ((trace.surface.flags >> 7) & SURF_LIGHT);

	if (possible_hits <= 0
		|| (trace.surface.name == (const char*)0x2227c261 && exit_trace.surface.name == (const char*)0x2227c868)
		|| (!possible_hits && !is_light_surf && !is_solid_surf && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
		|| weapon_data->flPenetration <= 0.f
		|| !trace_to_exit(trace, exit_trace, trace.end, direction)
		&& !(interfaces::trace_ray->get_point_contents(trace.end, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL)) {
		penetration = false;
		return false;
	}

	surface_data_t* exit_surface_data = interfaces::physics_surface->get_surface_data(exit_trace.surface.surface_props);
	int exit_material = exit_surface_data->game_props.material;
	float exitDamageModifier = exit_surface_data->game_props.damage_modifier;

	if (sv_penetration_type) {
		if (enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS) {
			combined_penetration_modifier = 3.f;
			final_damage_modifier = 0.05f;
		}
		else if (is_solid_surf || is_light_surf) {
			combined_penetration_modifier = 1.f;
			final_damage_modifier = 0.16f;
		}
		else if (enter_material == CHAR_TEX_FLESH && (!target->is_enemy(player) && ff_damage_reduction_bullets == 0.f)) {
			if (ff_damage_bullet_penetration == 0.f) {
				penetration = false;
				return false;
			}
			combined_penetration_modifier = ff_damage_bullet_penetration;
			final_damage_modifier = 0.16f;
		}
		else {
			combined_penetration_modifier = (enter_surface_data->game_props.penetration_modifier + exit_surface_data->game_props.penetration_modifier) / 2.f;
			final_damage_modifier = 0.16f;
		}

		if (enter_material == exit_material) {
			if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
				combined_penetration_modifier = 3.f;
			else if (exit_material == CHAR_TEX_PLASTIC)
				combined_penetration_modifier = 2.f;
		}

		thickness = (exit_trace.end - trace.end).length_sqr();
		modifier = fmaxf(1.f / combined_penetration_modifier, 0.f);

		lost_damage = fmaxf( ((modifier * thickness) / 24.f) + ((current_damage * final_damage_modifier) + (fmaxf(3.75f / penetration_power, 0.f) * 3.f * modifier)), 0.f);

		if (lost_damage > current_damage) {
			penetration = false;
			return false;
		}

		if (lost_damage > 0.f)
			current_damage -= lost_damage;

		if (current_damage < 1.f) {
			penetration = false;
			return false;
		}

		start = exit_trace.end;
		--possible_hits;

		penetration = true;
		return true;
	}
	else {
		combined_penetration_modifier = 1.f;

		if (is_solid_surf || is_light_surf)
			final_damage_modifier = 0.99f;
		else {
			final_damage_modifier = fminf(enter_surface_data->game_props.damage_modifier, exit_surface_data->game_props.damage_modifier);
			combined_penetration_modifier = fminf(enter_surface_data->game_props.penetration_modifier, exit_surface_data->game_props.penetration_modifier);
		}

		if (enter_material == exit_material && (exit_material == CHAR_TEX_METAL || exit_material == CHAR_TEX_WOOD))
			combined_penetration_modifier += combined_penetration_modifier;

		thickness = (exit_trace.end - trace.end).length_sqr();

		if (sqrt(thickness) <= combined_penetration_modifier * penetration_power) {
			current_damage *= final_damage_modifier;
			start = exit_trace.end;
			--possible_hits;
			penetration = true;
			return true;
		}
		penetration = false;
		return false;
	}
}

bool fire_bullet(player_t* player, vec3_t& direction, float& current_damage) {
	auto weapon = player->active_weapon();
	if (!weapon)
		return false;

	vec3_t eye_position = player->get_eye_pos();

	bool sv_penetration_type;
	float currentDistance = 0.f, penetration_power, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration;

	sv_penetration_type = interfaces::console->get_convar(("sv_penetration_type"))->numerical_value;
	ff_damage_reduction_bullets = interfaces::console->get_convar(("ff_damage_reduction_bullets"))->float_value;
	ff_damage_bullet_penetration = interfaces::console->get_convar(("ff_damage_bullet_penetration"))->float_value;
	weapon_info_t* weapon_data = weapon->get_weapon_data();
	trace_t trace;
	trace_filter filter;

	filter.skip = player;

	if (!weapon_data)
		return false;

	maxRange = weapon_data->flRange;

	bullet_type(penetration_power, penetrationDistance, *(char**)((uintptr_t)weapon_data + 0x0080), sv_penetration_type);

	if (sv_penetration_type)
		penetration_power = weapon_data->flPenetration;

	int possible_hits = 4;

	current_damage = weapon_data->iDamage;
	while (possible_hits > 0 && current_damage >= 1.f) {
		maxRange -= currentDistance;
		vec3_t end = eye_position + direction * maxRange;
		trace_line(eye_position, end, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &trace);
		clip_trace_to_player(eye_position, end + direction * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);
		surface_data_t* surface_data = interfaces::physics_surface->get_surface_data(trace.surface.surface_props);
		if (trace.flFraction == 1.f)
			break;
		currentDistance += trace.flFraction * maxRange;
		current_damage *= pow(weapon_data->flRangeModifier, (currentDistance / 500.f));
		if (currentDistance > penetrationDistance && weapon_data->flPenetration > 0.f || surface_data->game_props.penetration_modifier < 0.1f)
			break;
		bool able_to_deal_damage = (trace.hitGroup != HITGROUP_CHEST && trace.hitGroup != HITGROUP_GENERIC);
		if (able_to_deal_damage && static_cast<player_t*>(trace.entity)->is_enemy(csgo::local_player)) {
			scale_damage(trace, weapon_data, current_damage);
			return true;
		}
		if (!handle_bullet_penetration(player, weapon_data, trace, eye_position, direction, possible_hits, current_damage, penetration_power, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}
	return false;
}

float aimbot::autowall::can_hit(player_t* player, vec3_t point) { //basically what you need
	if (!player || !player->is_alive())
		return -1.f;
	fire_bullet_data data(player->get_eye_pos());
	data.filter = trace_filter();
	data.filter.skip = player;
	vec3_t angles, direction;
	vec3_t tmp = point - player->get_eye_pos();
	float currentDamage = 0;
	direction = tmp;
	direction.normalize();
	if (fire_bullet(player, direction, currentDamage))
		return currentDamage;
	return -1.f;
}