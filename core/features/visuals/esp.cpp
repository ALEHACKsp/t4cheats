#include <limits>

#define NOMINMAX

#include "../features.hpp"
#include "../../../dependencies/interfaces/c_global_vars_base.hpp"
#include "../../../dependencies/interfaces/interfaces.hpp"
#include "../../../dependencies/interfaces/i_surface.hpp"
#include "../../../dependencies/interfaces/i_client_entity_list.hpp"
#include "../../../dependencies/interfaces/iv_engine_client.hpp"
#include "../../../source-sdk/math/vector3d.hpp"

struct bounding_box {
private:
	bool valid;
public:
	vec3_t min, max;
	vec3_t vertices[8];

	bounding_box(entity_t* entity) {
		min.y = min.x = std::numeric_limits<float>::max();
		max.y = max.x = -std::numeric_limits<float>::max();

		const auto collideable = entity->get_collideable();
		const vec3_t obb_mins = collideable->mins();
		const vec3_t obb_maxs = collideable->maxs();

		const matrix_t coordinate_frame = entity->coordinate_frame();

		for (int i = 0; i < 8; ++i) {
			const vec3_t point{ i & 1 ? obb_maxs.x : obb_mins.x,
								i & 2 ? obb_maxs.y : obb_mins.y,
								i & 4 ? obb_maxs.z : obb_mins.z };

			if (!math::world_to_screen(point.transform(coordinate_frame), vertices[i])) {
				valid = false;
				return;
			}

			min.x = std::min<float>(min.x, vertices[i].x);
			min.y = std::min<float>(min.y, vertices[i].y);
			max.x = std::max<float>(max.x, vertices[i].x);
			max.y = std::max<float>(max.y, vertices[i].y);
		}
		valid = true;
	}

	operator bool() const {
		return valid;
	}
};

static void draw_box(const bounding_box& box) {
	render::draw_outline(box.min.x, box.min.y, box.max.x - box.min.x, box.max.y - box.min.y, color::white(150));
}

static void draw_name(const bounding_box& box, player_t* player) {
	player_info_t info;
	if (!interfaces::engine->get_player_info(player->index(), &info))
		return;

	std::string string_name = info.name;

	if (string_name.length() > 20) {
		string_name.resize(20);
		string_name += "...";
	}

	int wname_dimensions[2];
	int width = 10;

	if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, string_name.c_str(), -1, name, 128)) {
		interfaces::surface->get_text_size(render::fonts::main, name, wname_dimensions[0], wname_dimensions[1]);
		render::draw_text_wchar(box.min.x + ((box.max.x - box.min.x) * .5f) - (info.fake_player ? (render::get_text_size(render::fonts::pixel, "[BOT]").x / 2) + 2 : 0), box.min.y - 16, render::fonts::main, name, true, color(255, 255, 255, 200));
		width += wname_dimensions[0];
	}

	if (info.fake_player) {
		width += render::get_text_size(render::fonts::pixel, "[BOT]").x;
		render::draw_text_string(box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f) + wname_dimensions[0] + 6, box.min.y - 15, render::fonts::pixel, "[BOT]", false, color(200, 200, 200, 200));
	}

	render::draw_filled_rect(box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f), box.min.y - 18, width, wname_dimensions[1] + 4, color(50, 50, 50, 150));
	render::draw_outline(box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f), box.min.y - 18, width, wname_dimensions[1] + 4, color(25, 25, 25, 150));
	render::draw_line(box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f), box.min.y - 18, box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f) + width, box.min.y - 18, player->team() == 2 ? color(190, 160, 60, 200) : color(60, 120, 190, 200));
}

static color get_color_from_health(int health, int alpha = 255) {
	return color(255 - (health * 2.55f), health * 2.55f, 0, alpha);
}

static void draw_healthbar(const bounding_box& box, player_t* player) {
	const int health = player->health();
	const int health_width = (56 * health) / 100;

	render::draw_filled_rect(box.min.x + ((box.max.x - box.min.x) * .5f) - 39, box.max.y + 1, 78, 10, color(50, 50, 50, 150));
	render::draw_outline(box.min.x + ((box.max.x - box.min.x) * .5f) - 39, box.max.y + 1, 78, 10, color(25, 25, 25, 150));
	render::draw_filled_rect(box.min.x + ((box.max.x - box.min.x) * .5f) - 21, box.max.y + 5, health_width, 2, get_color_from_health(health, 200));
	render::draw_text_string(box.min.x + ((box.max.x - box.min.x) * .5f) - 36, box.max.y, render::fonts::pixel, "HP:", false, color(240, 240, 240, 200));

	if (health < 100)
		render::draw_text_string(box.min.x + ((box.max.x - box.min.x) * .5f) - 21 + health_width, box.max.y, render::fonts::pixel_shadow, std::to_string(health), true, color(240, 240, 240, 200));
}

static void draw_weapon(const bounding_box& box, player_t* player) {
	const auto weapon = player->get_active_weapon();
	if (!weapon)
		return;

	const auto weapon_data = weapon->get_weapon_data();
	if (!weapon_data)
		return;

	const int ammo = weapon->ammo();

	std::string weapon_text = weapon->get_weapon_name();
	if (weapon_data->WeaponType != cs_weapon_type::WEAPONTYPE_KNIFE)
		weapon_text.append(" [").append(std::to_string(ammo)).append("/").append(std::to_string(weapon->primary_reserve_ammo_acount())).append("]");

	const int y = box.max.y + 1 + (variables::visuals::esp_show_healthbar * 11);
	const float width = render::get_text_size(render::fonts::pixel, weapon_text).x + 5;

	render::draw_filled_rect(box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f), y, width, 9, color(50, 50, 50, 150));
	render::draw_outline(box.min.x + ((box.max.x - box.min.x) * .5f) - (width * .5f), y, width, 9, color(25, 25, 25, 150));
	render::draw_text_string(box.min.x + ((box.max.x - box.min.x) * .5f), y - 1, render::fonts::pixel, weapon_text, true, color(240, 240, 240, 200));
}

static void draw_headdot(player_t* player) {
	if (vec3_t screen_head_pos; math::world_to_screen(player->get_hitbox_position(hitbox_head), screen_head_pos))
		render::draw_xhair(screen_head_pos.x, screen_head_pos.y, true, color::white(200));
}

void visuals::esp::draw() {
	for (int i = 1; i < interfaces::global_vars->max_clients; ++i) {
		const auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!player || player == csgo::local_player || player->is_dormant() || !player->is_alive() || (variables::visuals::esp_enemies_only && !player->is_enemy(csgo::local_player)))
			continue;

		const bounding_box box{ player };
		if (!box)
			continue;

		if (variables::visuals::esp_show_box)
			draw_box(box);
		if (variables::visuals::esp_show_name)
			draw_name(box, player);
		if (variables::visuals::esp_show_healthbar)
			draw_healthbar(box, player);
		if (variables::visuals::esp_show_weapon)
			draw_weapon(box, player);
		if (variables::visuals::esp_show_headdot)
			draw_headdot(player);
	}
}