#include "../features.hpp"

#include "../../../source-sdk/math/vector3d.hpp"

RECT get_bounds(entity_t* entity) {
	const auto collideable = entity->get_collideable();
	if (!collideable)
		return { };

	const vec3_t min = collideable->mins();
	const vec3_t max = collideable->maxs();

	vec3_t points[] = {
		{ min.x, min.y, min.z },
		{ min.x, max.y, min.z },
		{ max.x, max.y, min.z },
		{ max.x, min.y, min.z },
		{ max.x, max.y, max.z },
		{ min.x, max.y, max.z },
		{ min.x, min.y, max.z },
		{ max.x, min.y, max.z }
	};

	vec3_t points_transformed[8];
	vec3_t screen_points[8];

	matrix_t coordinate_frame = entity->coordinate_frame();

	for (int i = 0; i < 8; ++i)
		math::transform_vector(points[i], coordinate_frame, points_transformed[i]);

	for (int i = 0; i < 8; ++i) {
		if (!math::world_to_screen(points_transformed[i], screen_points[i]))
			return { };
	}

	float left = screen_points[0].x;
	float top = screen_points[0].y;
	float right = screen_points[0].x;
	float bottom = screen_points[0].y;

	for (int i = 1; i < 8; ++i) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;

		if (top < screen_points[i].y)
			top = screen_points[i].y;

		if (right < screen_points[i].x)
			right = screen_points[i].x;

		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}

	// to-do: make our own class so we can stop casting these to long
	return { static_cast<LONG>(left), static_cast<LONG>(bottom), static_cast<LONG>(right), static_cast<LONG>(top) };
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

void draw_name(player_t* player) {
	static wchar_t name[128]; player_info_t info;
	interfaces::engine->get_player_info(player->index(), &info);
	std::string string_name = info.name;

	if (string_name.length() > 16) {
		string_name.resize(16);
		string_name += "...";
	}

	int wname_dimensions[2];
	int width = 10;

	if (MultiByteToWideChar(CP_UTF8, 0, string_name.c_str(), -1, name, 128)) {
		interfaces::surface->get_text_size(render::fonts::main, name, wname_dimensions[0], wname_dimensions[1]);
		render::draw_text_wchar(bbox.left + ((bbox.right - bbox.left) * .5f) - (info.fake_player ? (render::get_text_size(render::fonts::pixel, "[BOT]").x / 2) + 2 : 0), bbox.top - 16, render::fonts::main, name, true, color(255, 255, 255, 200));
		width += wname_dimensions[0];
	}

	if (info.fake_player) {
		width += render::get_text_size(render::fonts::pixel, "[BOT]").x;
		render::draw_text_string(bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f) + wname_dimensions[0] + 6, bbox.top - 15, render::fonts::pixel, "[BOT]", false, color(200, 200, 200, 200));
	}

	render::draw_filled_rect(bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f), bbox.top - 18, width, wname_dimensions[1] + 4, color(50, 50, 50, 150));
	render::draw_outline(bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f), bbox.top - 18, width, wname_dimensions[1] + 4, color(25, 25, 25, 150));
	render::draw_line(bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f), bbox.top - 18, bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f) + width, bbox.top - 18, player->team() == 2 ? color(190, 160, 60, 200) : color(60, 120, 190, 200));
}

void draw_box() {
	render::draw_outline(bbox.left, bbox.top, bbox.right - bbox.left, bbox.bottom - bbox.top, color::black(150));
}

void draw_healthbar(player_t* player) {
	auto health = player->health();
	auto health_width = (56 * health) / 100;
	auto y = bbox.bottom + 1; //haha, i love doing stuff like this
	render::draw_filled_rect(bbox.left + ((bbox.right - bbox.left) * .5f) - 39, y, 78, 10, color(50, 50, 50, 150));
	render::draw_outline(bbox.left + ((bbox.right - bbox.left) * .5f) - 39, y, 78, 10, color(25, 25, 25, 150));
	render::draw_text_string(bbox.left + ((bbox.right - bbox.left) * .5f) - 36, y - 1, render::fonts::pixel, "HP:", false, color(240, 240, 240, 200));
	render::draw_outline(bbox.left + ((bbox.right - bbox.left) * .5f) - 22, y + 3, 58, 4, color(15, 15, 15, 150));
	render::draw_filled_rect(bbox.left + ((bbox.right - bbox.left) * .5f) - 21, y + 4, health_width, 2, get_color_from_health(health, 200));
	if (health < 100)
		render::draw_text_string(bbox.left + ((bbox.right - bbox.left) * .5f) - 21 + health_width, y - 1, render::fonts::pixel_shadow, std::to_string(health), true, color(240, 240, 240, 200));
}

void draw_weapon(player_t* player) {
	const auto weapon = player->get_active_weapon();
	if (!weapon)
		return;

	auto y = bbox.bottom + 1 + (variables::visuals::esp_show_healthbar * 11); //haha, i love doing stuff like this
	std::string weapon_text = weapon->get_weapon_name();
	if (weapon->ammo() >= 0)
		weapon_text += " [" + std::to_string(weapon->ammo()) + "/" + std::to_string(weapon->primary_reserve_ammo_acount()) + "]";
	auto width = render::get_text_size(render::fonts::pixel, weapon_text).x + 5;
	render::draw_filled_rect(bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f), y, width, 9, color(50, 50, 50, 150));
	render::draw_outline(bbox.left + ((bbox.right - bbox.left) * .5f) - (width * .5f), y, width, 9, color(25, 25, 25, 150));
	render::draw_text_string(bbox.left + ((bbox.right - bbox.left) * .5f), y - 1, render::fonts::pixel, weapon_text, true, color(240, 240, 240, 200));
}

void draw_headdot(player_t* player, color color) {
	if (vec3_t screen_head_pos; math::world_to_screen(player->get_hitbox_position(hitbox_head), screen_head_pos))
		render::draw_xhair(screen_head_pos.x, screen_head_pos.y, true, color);
}

void visuals::esp::draw() {
	for (int i = 1; i < interfaces::global_vars->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!entity || !entity->is_player() || entity->is_dormant() || !entity->is_alive() || entity == csgo::local_player) //dont draw esp for localplayer for now
			continue;

		if (variables::visuals::esp_enemies_only && !entity->is_enemy(csgo::local_player))
			continue;

		bbox = get_bounds(entity);
		if (bbox.right == 0 || bbox.bottom == 0) //if box is out of bounds, skip
			continue;

		if (variables::visuals::esp_show_name)
			draw_name(entity);
		if (variables::visuals::esp_show_box)
			draw_box();
		if (variables::visuals::esp_show_healthbar)
			draw_healthbar(entity);
		if (variables::visuals::esp_show_weapon)
			draw_weapon(entity);
		if (variables::visuals::esp_show_headdot)
			draw_headdot(entity, color::white(200));
	}
}