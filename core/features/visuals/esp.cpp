#include "../features.hpp"

RECT get_bounds(entity_t* entity) {
	RECT rect{};
	auto collideable = entity->collideable();

	if (!collideable)
		return rect;

	auto min = collideable->mins();
	auto max = collideable->maxs();

	matrix_t& trans = entity->coordinateframe();

	vec3_t points[] = {
		vec3_t(min.x, min.y, min.z),
		vec3_t(min.x, max.y, min.z),
		vec3_t(max.x, max.y, min.z),
		vec3_t(max.x, min.y, min.z),
		vec3_t(max.x, max.y, max.z),
		vec3_t(min.x, max.y, max.z),
		vec3_t(min.x, min.y, max.z),
		vec3_t(max.x, min.y, max.z)
	};

	vec3_t pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		math::transform_vector(points[i], trans, pointsTransformed[i]);
	}

	vec3_t screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!math::world_to_screen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)bottom, (long)right, (long)top };
}

color get_color_from_health(int health, int alpha = 255)
{
	return color(255 - (health * 2.55f), health * 2.55f, 0, alpha);
}

RECT bbox;
/*
	BOX DIMENSIONS

	left = x-coordinate top
	top = y-coordinate top
	right = x-coordinate bottom
	bottom = y-coordinate bottom
*/

void draw_name(player_t* player, color color) {
	static wchar_t name[128]; player_info_t info;
	interfaces::engine->get_player_info(player->index(), &info);
	if (MultiByteToWideChar(CP_UTF8, 0, info.name, -1, name, 128)) {
		render::draw_text_wchar(bbox.left + ((bbox.right - bbox.left) * .5f), bbox.top - 14, render::fonts::main, name, true, color);
	}
}

void draw_box(color color) {
	render::draw_outline(bbox.left - 1, bbox.top - 1, bbox.right - bbox.left + 2, bbox.bottom - bbox.top + 2, color::black(color.a)); //outter outline
	render::draw_outline(bbox.left, bbox.top, bbox.right - bbox.left, bbox.bottom - bbox.top, color); //actual box
	render::draw_outline(bbox.left + 1, bbox.top + 1, bbox.right - bbox.left - 2, bbox.bottom - bbox.top - 2, color::black(color.a)); //inner outline
}

void draw_weapon(player_t* player, color color) {
	if (player->active_weapon()) {
		render::draw_text_string(bbox.left + ((bbox.right - bbox.left) * .5f), bbox.bottom, render::fonts::main, player->active_weapon()->get_weapon_name(), true, color);
	}
}

void draw_healthbar(player_t* player) {
	auto health = player->health();
	auto height = ((bbox.bottom - bbox.top) * health) / 100;
	render::draw_outline(bbox.left - 6, bbox.top - 1, 4, bbox.bottom - bbox.top + 2, color::black(200));
	render::draw_filled_rect(bbox.left - 5, bbox.top, 2, ((bbox.bottom - bbox.top) - height), color::black(200));
	render::draw_filled_rect(bbox.left - 5, bbox.top + ((bbox.bottom - bbox.top) - height), 2, height, get_color_from_health(health));
}

void draw_headdot(player_t* player, color color) {
	vec3_t screen_head_pos;
	if (math::world_to_screen(player->get_hitbox_position(hitbox_head), screen_head_pos))
		render::draw_xhair(screen_head_pos.x, screen_head_pos.y, true, color);
}

void visuals::esp::draw() {
	for (int i = 1; i < interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!entity || !entity->is_player() || entity->dormant() || !entity->is_alive() || entity == csgo::local_player) //dont draw esp for localplayer for now
			continue;

		bbox = get_bounds(entity);
		if (bbox.right == 0 || bbox.bottom == 0) //if box is out of bounds, skip
			continue;

		if (variables::visuals::esp_show_name)
			draw_name(entity, color::white(200));
		if (variables::visuals::esp_show_box) //ghetto visibility check, will change
			draw_box(entity->is_enemy(csgo::local_player) ? csgo::local_player->can_see_player_pos(entity, entity->get_hitbox_position(hitbox_chest)) ? variables::visuals::esp_colors::enemies_visible : variables::visuals::esp_colors::enemies_invisible : csgo::local_player->can_see_player_pos(entity, entity->get_hitbox_position(hitbox_chest)) ? variables::visuals::esp_colors::team_visible : variables::visuals::esp_colors::team_invisible);
		if (variables::visuals::esp_show_weapon)
			draw_weapon(entity, color::white(200));
		if (variables::visuals::esp_show_healthbar)
			draw_healthbar(entity);
		if (variables::visuals::esp_show_headdot)
			draw_headdot(entity, color::white(200));
	}
}