#include "../features.hpp"

void draw_crosshair() {
	std::pair<int, int> screen_size;

	interfaces::surface->get_screen_size(screen_size.first, screen_size.second);
	int x = screen_size.first / 2;
	int y = screen_size.second / 2;

	if (variables::visuals::crosshair_recoil) {
		vec3_t punch = csgo::local_player->aim_punch_angle();
		if (csgo::local_player->is_scoped())
			punch /= .5f;

		// subtract the punch from the position
		x -= (screen_size.first / 90) * punch.y;
		y += (screen_size.second / 90) * punch.x;
	}

	render::draw_xhair(x, y, variables::visuals::crosshair_recoil, variables::visuals::crosshair_color);
}

void visualize_choke() {
	const int choked_packets = interfaces::clientstate->net_channel->choked_packets;
	const int max_choke = 16; // todo: change according to m_bIsValveDS, maybe something else actually
	const int choked_width = (156 * choked_packets) / max_choke;

	const int y = 800, x = 10;

	render::draw_filled_rect(x, y, 200, 15, color(50, 50, 50, 150));
	render::draw_outline(x, y, 200, 15, color(25, 25, 25, 150));
	render::draw_text_string(x + 4, y + 1, render::fonts::main, "choke", false, color(240, 240, 240, 200));
	render::draw_outline(x + 37, y + 5, 158, 5, color(15, 15, 15, 150));
	render::draw_filled_rect(x + 38, y + 6, choked_width, 3, color(255, 140, 0, 200));

	if (choked_packets > 0)
		render::draw_text_string(x + 38 + choked_width, y + 2, render::fonts::pixel_shadow, std::to_string(choked_packets), true, color(240, 240, 240, 200)); //if you choked any packets, draw number of choked packets
}

void draw_spectator_list(int x, int y) {
	render::draw_filled_rect(x, y, 200, 15, color(50, 50, 50, 150));
	render::draw_outline(x, y, 200, 15, color(25, 25, 25, 150));
	render::draw_text_string(x + 4, y + 1, render::fonts::main, "spectators", false, color(240, 240, 240, 200));

	const auto observer_target = csgo::local_player->get_observer_target();
	if (!observer_target)
		return;

	const int spectator_box_y = y + 16;
	int spectator_box_height = 0, spectator_text_offset = 0;

	for (int i = 1; i <= interfaces::globals->max_clients; ++i) {
		const auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!player || player == csgo::local_player || player->dormant() || player->is_alive() || player->get_observer_target() != observer_target)
			continue;

		player_info_t info;
		if (!interfaces::engine->get_player_info(i, &info))
			continue;

		std::string string_name = info.name;
		if (string_name.length() > 20) {
			string_name.resize(20);
			string_name += "...";
		}

		if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, string_name.c_str(), -1, name, 128)) {
			int text_dimensions[2];
			interfaces::surface->get_text_size(render::fonts::main, name, text_dimensions[0], text_dimensions[1]);
			render::draw_text_wchar(x + 4, spectator_box_y + spectator_text_offset + 2, render::fonts::main, name, false, color(255, 255, 255, 255)); //TODO: move this draw_text_wchar after drawing the box

			std::string obs_mode_text = [](int observer_mode) -> std::string {
				switch (observer_mode) {
				case obs_modes::obs_mode_deathcam:
					return "death";
				case obs_modes::obs_mode_fixed:
					return "fixed";
				case obs_modes::obs_mode_chase:
					return "thirdperson";
				case obs_modes::obs_mode_in_eye:
					return "firstperson";
				case obs_modes::obs_mode_roaming:
					return "freeroam";
				case obs_modes::obs_mode_freezecam:
					return "freeze";
				default:
					return "";
				}
			}(player->observer_mode());

			render::draw_text_string(x + 200 - (render::get_text_size(render::fonts::pixel, obs_mode_text).x + 4), spectator_box_y + spectator_text_offset + 3, render::fonts::pixel, obs_mode_text, false, color(255, 255, 255, 255));
			spectator_text_offset += text_dimensions[1] + 5;
			spectator_box_height += text_dimensions[1] + 5;
		}
	}

	if (spectator_text_offset > 0) {
		render::draw_filled_rect(x, spectator_box_y, 200, spectator_box_height, color(50, 50, 50, 150));
		render::draw_outline(x, spectator_box_y, 200, spectator_box_height, color(25, 25, 25, 150));
	}
}

void draw_fire_timer(inferno_t* entity) {
	vec3_t screen_pos;

	if (!math::world_to_screen(entity->origin(), screen_pos))
		return;

	constexpr float inferno_life_time = 7.03125f;
	static const vec2_t size{ 60.f, 4.f };

	const float life = ((entity->spawn_time() + inferno_life_time) - interfaces::globals->cur_time) / inferno_life_time;

	render::draw_filled_rect(screen_pos.x - size.x * .5f, screen_pos.y - size.y * .5f, size.x, size.y, color(45, 45, 45, life >= .1f ? 255 : 255 * (life / .1f)));
	render::draw_filled_rect(screen_pos.x - size.x * .5f + 2, screen_pos.y - size.y * .5f + 1, (size.x - 4) * life, size.y - 2, color(240, 100, 100, life >= .1f ? 255 : 255 * (life / .1f)));

	render::draw_outline(screen_pos.x - size.x * .5f, screen_pos.y - size.y * .5f, size.x, size.y, color(25, 25, 25, life >= .2f ? 255 : 255 * (life / .1f)));
}

void visuals::other_visuals::draw() {
	for (int i = 1; i <= interfaces::entity_list->get_highest_index(); ++i) {
		const auto entity = interfaces::entity_list->get_client_entity(i);
		if (!entity)
			continue;

		switch (entity->client_class()->class_id) {
			case class_ids::cinferno:
				if (variables::visuals::fire_timer_enable)
					draw_fire_timer(reinterpret_cast<inferno_t*>(entity));

				break;
			default:
				break;
		}
	}

	if (variables::visuals::spectator_list_enable)
		draw_spectator_list(300, 10);

	if (!csgo::local_player->is_alive())
		return;

	if (variables::visuals::crosshair_enable)
		draw_crosshair();

	if (variables::visuals::visualize_choke_enable && variables::misc::fake_lag_enable)
		visualize_choke();
}