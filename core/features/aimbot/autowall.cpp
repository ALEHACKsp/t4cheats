#include "../features.hpp"

#include "../../../source-sdk/classes/entities.hpp"
#include "../../../dependencies/interfaces/i_physics_surface_props.hpp"
#include "../../../dependencies/interfaces/i_trace.hpp"
#include "../../../dependencies/interfaces/i_console.hpp"

#define NOMINMAX

// credit to https://www.unknowncheats.me/forum/counterstrike-global-offensive/255054-autowall-convar-scaling.html for the penetration convar scaling system

enum hitgroup : int {
	generic = 0,
	head,
	chest,
	stomach,
	left_arm,
	right_arm,
	left_leg,
	right_leg,
	gear
};

void get_bullet_type(bool sv_penetration_type, float& penetration_power, float& penetration_distance, const char* bullet_type) {
	if (sv_penetration_type) {
		penetration_power = 35.f;
		penetration_distance = 3000.f;
		return;
	}

	switch (fnv::hash(bullet_type)) {
	case fnv::hash("BULLET_PLAYER_338MAG"):
		penetration_power = 45.f;
		penetration_distance = 8000.f;
		break;
	case fnv::hash("BULLET_PLAYER_762MM"):
		penetration_power = 39.f;
		penetration_distance = 5000.f;
		break;
	case fnv::hash("BULLET_PLAYER_556MM"):
	case fnv::hash("BULLET_PLAYER_556MM_SMALL"):
	case fnv::hash("BULLET_PLAYER_556MM_BOX"):
		penetration_power = 35.f;
		penetration_distance = 4000.f;
		break;
	case fnv::hash("BULLET_PLAYER_57MM"):
		penetration_power = 30.f;
		penetration_distance = 2000.f;
		break;
	case fnv::hash("BULLET_PLAYER_50AE"):
		penetration_power = 30.f;
		penetration_distance = 1000.f;
		break;
	case fnv::hash("BULLET_PLAYER_357SIG"):
	case fnv::hash("BULLET_PLAYER_357SIG_SMALL"):
	case fnv::hash("BULLET_PLAYER_357SIG_P250"):
	case fnv::hash("BULLET_PLAYER_357SIG_MIN"):
		penetration_power = 25.f;
		penetration_distance = 800.f;
		break;
	case fnv::hash("BULLET_PLAYER_9MM"):
		penetration_power = 21.f;
		penetration_distance = 800.f;
		break;
	case fnv::hash("BULLET_PLAYER_45ACP"):
		penetration_power = 15.f;
		penetration_distance = 500.f;
		break;
	case fnv::hash("BULLET_PLAYER_BUCKSHOT"):
		penetration_power = 0.f;
		penetration_distance = 0.f;
		break;
	}
}

void trace_line(const vec3_t& start, const vec3_t& end, unsigned int mask, player_t* ignore, trace_t* ptr) {
	ray_t ray; ray.initialize(start, end);
	trace_filter filter; filter.skip = ignore;
	interfaces::trace_ray->trace_ray(ray, mask, &filter, ptr);
}

void clip_trace_to_player(player_t* player, const vec3_t& start, const vec3_t& end, unsigned int mask, trace_filter* filter, trace_t* tr) {
	if (!filter->ShouldHitEntity(player, mask))
		return;

	ray_t ray;
	ray.initialize(start, end);
	trace_t trace;
	interfaces::trace_ray->trace_ray(ray, mask, filter, &trace);

	if (trace.flFraction < tr->flFraction)
		tr = &trace;
}

void scale_damage(trace_t& trace, float weapon_armor_ratio, float& current_damage)
{
	int armor_value = trace.entity->armor();
	bool has_heavy_armor = trace.entity->has_heavy_armor();

	current_damage *= [&]() -> float {
		switch (trace.hitGroup) {
		case hitgroup::head:
			return has_heavy_armor ? 2.f : 4.f;
		case hitgroup::stomach:
			return 1.25f;
		case hitgroup::left_leg:
		case hitgroup::right_leg:
			return .75f;
		default:
			return 1.f;
		}
	}();

	if ((armor_value > 0 && trace.hitGroup < hitgroup::left_leg) || (trace.hitGroup == hitgroup::head && trace.entity->has_helmet())) {
		float armor_bonus = .5f, armor_ratio = weapon_armor_ratio * .5f;

		if (has_heavy_armor) {
			armor_bonus = .33f;
			armor_ratio *= .5f;
		}

		float new_damage = current_damage * armor_ratio;

		if (((current_damage - (current_damage * armor_ratio)) * (has_heavy_armor ? .33 : 1.f * armor_bonus)) > armor_value)
			new_damage = current_damage - (armor_value / armor_bonus);

		current_damage = new_damage;
	}
}

bool is_breakable(player_t* entity) {
	if (const auto client_class = entity->get_client_class())
		return client_class->class_id == cbreakableprop || client_class->class_id == cbreakablesurface;

	return false;
}

bool trace_to_exit(trace_t& enter, trace_t& exit, vec3_t start_pos, vec3_t direction) {
	int first_contents = 0;
	float current_distance = 0.f;

	vec3_t start, end;

	while (current_distance <= 90.f) {
		current_distance += 4.f;
		start = start_pos + direction * current_distance;

		if (!first_contents)
			first_contents = interfaces::trace_ray->get_point_contents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		const int point_contents = interfaces::trace_ray->get_point_contents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

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

bool handle_bullet_penetration(player_t* player, weapon_info_t* weapon_data, trace_t& trace, vec3_t start, vec3_t direction, int& possible_hits, float& current_damage, float penetration_power, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration) {
	const bool is_solid_surf = ((trace.contents >> 3) & CONTENTS_SOLID);
	const bool is_light_surf = ((trace.surface.flags >> 7) & SURF_LIGHT);
	float thickness = 0.f, final_damage_modifier = 0.f, combined_penetration_modifier = 0.f;

	const auto enter_surface_data = interfaces::physics_surface->get_surface_data(trace.surface.surface_props);
	const int enter_material = enter_surface_data->game_props.material;

	trace_t exit;

	if (possible_hits <= 0
		|| (!possible_hits && !is_light_surf && !is_solid_surf && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
		|| weapon_data->flPenetration <= 0.f
		|| !trace_to_exit(trace, exit, trace.end, direction)
		&& !(interfaces::trace_ray->get_point_contents(trace.end, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;

	const auto exit_surface_data = interfaces::physics_surface->get_surface_data(exit.surface.surface_props);
	const int exit_material = exit_surface_data->game_props.material;

	if (sv_penetration_type) {
		if (enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS) {
			combined_penetration_modifier = 3.f;
			final_damage_modifier = .05f;
		}
		else if (is_solid_surf || is_light_surf) {
			combined_penetration_modifier = 1.f;
			final_damage_modifier = .16f;
		}
		else if (enter_material == CHAR_TEX_FLESH && (!trace.entity->is_enemy(player) && ff_damage_reduction_bullets == 0.f)) {
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			combined_penetration_modifier = ff_damage_bullet_penetration;
			final_damage_modifier = .16f;
		}
		else {
			combined_penetration_modifier = (enter_surface_data->game_props.penetration_modifier + exit_surface_data->game_props.penetration_modifier) / 2.f;
			final_damage_modifier = .16f;
		}

		if (enter_material == exit_material) {
			if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
				combined_penetration_modifier = 3.f;
			else if (exit_material == CHAR_TEX_PLASTIC)
				combined_penetration_modifier = 2.f;
		}

		thickness = (exit.end - trace.end).length_sqr();

		const float modifier = std::max<float>(1.f / combined_penetration_modifier, 0.f);
		const float lost_damage = std::max<float>(((modifier * thickness) / 24.f) + ((current_damage * final_damage_modifier) + (std::fmax(3.75f / penetration_power, 0.f) * 3.f * modifier)), 0.f);

		if (lost_damage > current_damage)
			return false;

		if (lost_damage > 0.f)
			current_damage -= lost_damage;

		if (current_damage < 1.f)
			return false;

		start = exit.end;
		--possible_hits;

		return true;
	}
	else {
		combined_penetration_modifier = 1.f;

		if (is_solid_surf || is_light_surf)
			final_damage_modifier = .99f;
		else {
			final_damage_modifier = std::fmin(enter_surface_data->game_props.damage_modifier, exit_surface_data->game_props.damage_modifier);
			combined_penetration_modifier = std::fmin(enter_surface_data->game_props.penetration_modifier, exit_surface_data->game_props.penetration_modifier);
		}

		if (enter_material == exit_material && (exit_material == CHAR_TEX_METAL || exit_material == CHAR_TEX_WOOD))
			combined_penetration_modifier += combined_penetration_modifier;

		thickness = (exit.end - trace.end).length_sqr();

		if (std::sqrt(thickness) <= combined_penetration_modifier * penetration_power) {
			current_damage *= final_damage_modifier;
			start = exit.end;
			--possible_hits;
			return true;
		}

		return false;
	}
}

bool fire_bullet(player_t* player, const vec3_t direction, float& current_damage) {
	const auto weapon = player->get_active_weapon();
	if (!weapon)
		return false;

	const auto weapon_data = weapon->get_weapon_data();
	if (!weapon_data)
		return false;

	const bool sv_penetration_type = static_cast<bool>(interfaces::cvar->get_convar("sv_penetration_type")->get_int());
	const float ff_damage_reduction_bullets = interfaces::cvar->get_convar("ff_damage_reduction_bullets")->get_float();
	const float ff_damage_bullet_penetration = interfaces::cvar->get_convar("ff_damage_bullet_penetration")->get_float();
	const vec3_t eye_position = player->get_eye_pos();

	float penetration_power = 0.f, penetration_distance = 0.f;
	get_bullet_type(sv_penetration_type, penetration_power, penetration_distance, weapon_data->szBulletType);

	if (sv_penetration_type)
		penetration_power = weapon_data->flPenetration;

	int possible_hits = 4;
	float current_distance = 0.f;

	trace_t trace;
	trace_filter filter;
	filter.skip = player;

	current_damage = weapon_data->iDamage;

	while (possible_hits > 0 && current_damage >= 1.f) {
		const float max_range = weapon_data->flRange - current_distance;
		const vec3_t end = eye_position + direction * max_range;

		trace_line(eye_position, end, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &trace);
		clip_trace_to_player(player, eye_position, end + direction * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

		if (trace.flFraction == 1.f)
			break;

		current_distance += trace.flFraction * max_range;
		current_damage *= std::pow(weapon_data->flRangeModifier, (current_distance / 500.f));

		if (current_distance > penetration_distance && weapon_data->flPenetration > 0.f || interfaces::physics_surface->get_surface_data(trace.surface.surface_props)->game_props.penetration_modifier < .1f)
			break;

		if ((trace.hitGroup != hitgroup::chest && trace.hitGroup != hitgroup::generic) && trace.entity->is_enemy(csgo::local_player)) {
			scale_damage(trace, weapon_data->flArmorRatio, current_damage);
			return true;
		}

		if (!handle_bullet_penetration(player, weapon_data, trace, eye_position, direction, possible_hits, current_damage, penetration_power, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}

	return false;
}

float aimbot::autowall::can_hit(player_t* player, vec3_t point) {
	if (!player || !player->is_alive())
		return -1.f;

	vec3_t angles, direction;

	direction = point - player->get_eye_pos();
	direction.normalize();

	float current_damage = -1.f;

	if (!fire_bullet(player, direction, current_damage))
		return -1.f;

	return current_damage;
}