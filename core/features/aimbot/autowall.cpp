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

	float current_damage = 0.f;
	float trace_length = 0.f;
	float trace_length_remaining = 0.f;
	vec3_t direction;
	vec3_t src;
	trace_t enter_trace;
	trace_filter filter;
};

void bullet_type(float& max_range, float& max_distance, char* bulletType, bool sv_penetration_type) {
	if (sv_penetration_type) {
		max_range = 35.f;
		max_distance = 3000.f;
		return;
	}

	switch (fnv::hash(bulletType)) {
	case fnv::hash("BULLET_PLAYER_338MAG"):
		max_range = 45.f;
		max_distance = 8000.f;
		break;
	case fnv::hash("BULLET_PLAYER_762MM"):
		max_range = 39.f;
		max_distance = 5000.f;
		break;
	case fnv::hash("BULLET_PLAYER_556MM"):
	case fnv::hash("BULLET_PLAYER_556MM_SMALL"):
	case fnv::hash("BULLET_PLAYER_556MM_BOX"):
		max_range = 35.f;
		max_distance = 4000.f;
		break;
	case fnv::hash("BULLET_PLAYER_57MM"):
		max_range = 30.f;
		max_distance = 2000.f;
		break;
	case fnv::hash("BULLET_PLAYER_50AE"):
		max_range = 30.f;
		max_distance = 1000.f;
		break;
	case fnv::hash("BULLET_PLAYER_357SIG"):
	case fnv::hash("BULLET_PLAYER_357SIG_SMALL"):
	case fnv::hash("BULLET_PLAYER_357SIG_P250"):
	case fnv::hash("BULLET_PLAYER_357SIG_MIN"):
		max_range = 25.f;
		max_distance = 800.f;
		break;
	case fnv::hash("BULLET_PLAYER_9MM"):
		max_range = 21.f;
		max_distance = 800.f;
		break;
	case fnv::hash("BULLET_PLAYER_45ACP"):
		max_range = 15.f;
		max_distance = 500.f;
		break;
	case fnv::hash("BULLET_PLAYER_BUCKSHOT"):
		max_range = 0.f;
		max_distance = 0.f;
		break;
	}
}

void trace_line(vec3_t& start, vec3_t& end, unsigned int mask, player_t* ignore, trace_t* ptr) {
	ray_t ray; ray.initialize(start, end);
	trace_filter filter; filter.skip = ignore;
	interfaces::trace_ray->trace_ray(ray, mask, &filter, ptr);
}

void clip_trace_to_player(player_t* player, const vec3_t& start, const vec3_t end, unsigned int mask, trace_filter* filter, trace_t* tr) {
	if (!filter->ShouldHitEntity(player, mask))
		return;

	ray_t ray; ray.initialize(start, end);
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
		case HITGROUP_HEAD:
			return has_heavy_armor ? 2.f : 4.f;
		case HITGROUP_STOMACH:
			return 1.25f;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			return .75f;
		default:
			return 1.f;
		}
	}();

	if ((armor_value > 0 && trace.hitGroup < HITGROUP_LEFTLEG) || (trace.hitGroup == HITGROUP_HEAD && trace.entity->has_helmet())) {
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
	if (const auto client_class = entity->client_class())
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

		int point_contents = interfaces::trace_ray->get_point_contents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

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
	const auto weapon = player->active_weapon();
	if (!weapon)
		return false;

	const auto weapon_data = weapon->get_weapon_data();
	if (!weapon_data)
		return false;

	float current_distance = 0.f, penetration_power, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration;
	vec3_t eye_position = player->get_eye_pos();

	bool sv_penetration_type = static_cast<bool>(interfaces::console->get_convar("sv_penetration_type")->numerical_value);
	ff_damage_reduction_bullets = interfaces::console->get_convar("ff_damage_reduction_bullets")->float_value;
	ff_damage_bullet_penetration = interfaces::console->get_convar("ff_damage_bullet_penetration")->float_value;
	trace_t trace;
	trace_filter filter;

	filter.skip = player;

	maxRange = weapon_data->flRange;

	bullet_type(penetration_power, penetrationDistance, *(char**)((uintptr_t)weapon_data + 0x0080), sv_penetration_type);

	if (sv_penetration_type)
		penetration_power = weapon_data->flPenetration;

	int possible_hits = 4;

	current_damage = weapon_data->iDamage;

	while (possible_hits > 0 && current_damage >= 1.f) {
		maxRange -= current_distance;
		vec3_t end = eye_position + direction * maxRange;

		trace_line(eye_position, end, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &trace);
		clip_trace_to_player(player, eye_position, end + direction * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

		if (trace.flFraction == 1.f)
			break;

		current_distance += trace.flFraction * maxRange;
		current_damage *= std::pow(weapon_data->flRangeModifier, (current_distance / 500.f));

		if (current_distance > penetrationDistance && weapon_data->flPenetration > 0.f || interfaces::physics_surface->get_surface_data(trace.surface.surface_props)->game_props.penetration_modifier < .1f)
			break;

		bool able_to_deal_damage = (trace.hitGroup != HITGROUP_CHEST && trace.hitGroup != HITGROUP_GENERIC);

		if (able_to_deal_damage && trace.entity->is_enemy(csgo::local_player)) {
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

	fire_bullet_data data(player->get_eye_pos());
	data.filter = trace_filter();
	data.filter.skip = player;

	vec3_t angles, direction;
	vec3_t tmp = point - player->get_eye_pos();

	direction = tmp;
	direction.normalize();

	float current_damage = -1.f;
	fire_bullet(player, direction, current_damage);
	return current_damage;
}