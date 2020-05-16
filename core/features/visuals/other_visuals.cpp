#include "../features.hpp"

void draw_crosshair(bool recoil, bool outline, color color) {
	int screen_size[2];
	interfaces::surface->get_screen_size(screen_size[0], screen_size[1]); //get current screen size
	auto x = screen_size[0] / 2; //divide screen size with two so our xhair is in the middle of the screen
	auto y = screen_size[1] / 2;
	if (recoil) { //should the crosshair follow the recoil?
		auto punch = csgo::local_player->aim_punch_angle() / (csgo::local_player->is_scoped() ? .5f : 1.07f); //get the aim punch/recoil amount (fixed when scoped, with aug,sg)
		auto dx = screen_size[0] / 90;
		auto dy = screen_size[1] / 90;
		x -= ((screen_size[0] / 90) * (punch.y)); //subtract the punch from the x-, y-position of the xhair
		y += ((screen_size[1] / 90) * (punch.x));
	}
	render::draw_xhair(x, y, outline, color); // draw that b*tch
}

void visualize_choke() {
	auto max_choked = 6; //maximum number of choked packets (TODO: change according to m_bIsValveDS, maybe something else actually)
	auto current_choked = interfaces::clientstate->net_channel->choked_packets; //get amount of currently choked packets
	auto choked_width = (156 * current_choked) / max_choked; //calculate width
	auto y = 800, x = 10;
	render::draw_filled_rect(x, y, 200, 15, color(50, 50, 50, 150));
	render::draw_outline(x, y, 200, 15, color(25, 25, 25, 150));
	render::draw_text_string(x + 4, y + 1, render::fonts::main, "choke", false, color(240, 240, 240, 200));
	render::draw_outline(x + 37, y + 5, 158, 5, color(15, 15, 15, 150));
	render::draw_filled_rect(x + 38, y + 6, choked_width, 3, color(255, 140, 0, 200));
	if (current_choked > 0)
		render::draw_text_string(x + 38 + choked_width, y + 2, render::fonts::pixel_shadow, std::to_string(current_choked), true, color(240, 240, 240, 200)); //if you choked any packets, draw number of choked packets
}

void draw_spectator_list(int x, int y) {
	//header
	render::draw_filled_rect(x, y, 200, 15, color(50, 50, 50, 150));
	render::draw_outline(x, y, 200, 15, color(25, 25, 25, 150));
	render::draw_text_string(x + 4, y + 1, render::fonts::main, "spectators", false, color(240, 240, 240, 200));
	//actual list
	auto spectator_target = csgo::local_player->is_alive() ? csgo::local_player : csgo::local_player->get_observer_target();
	if (!spectator_target)
		return;
	auto spectator_box_y = y + 16, spectator_box_height = 0, spectator_text_offset = 0;
	for (int i = 1; i <= interfaces::globals->max_clients; i++) { //iterate through every possible entity indexes
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i)); //get entity from index
		if (!entity || entity->dormant() || entity->is_alive() || entity->get_observer_target() != spectator_target || entity == csgo::local_player)
			continue; //skip entity if not correct observer target, is localplayer, etc.
		player_info_t info;
		interfaces::engine->get_player_info(i, &info); //get entities player info
		std::string string_name = info.name;
		if (string_name.length() > 20) {
			string_name.resize(20);
			string_name += "...";
		}
		if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, string_name.c_str(), -1, name, 128)) { //convert the name for pretty UTF8 characters
			int text_dimensions[2];
			interfaces::surface->get_text_size(render::fonts::main, name, text_dimensions[0], text_dimensions[1]); //get text dimensions
			render::draw_text_wchar(x + 4, spectator_box_y + spectator_text_offset + 2, render::fonts::main, name, false, color(255, 255, 255, 255)); //TODO: move this draw_text_wchar after drawing the box
			auto observer_mode = entity->observer_mode();
			std::string obs_mode_text = "";
			switch (observer_mode) {
				case obs_modes::obs_mode_deathcam: obs_mode_text = "death"; break;
				case obs_modes::obs_mode_fixed: obs_mode_text = "fixed"; break;
				case obs_modes::obs_mode_chase: obs_mode_text = "3rd"; break;
				case obs_modes::obs_mode_in_eye: obs_mode_text = "1st"; break;
				case obs_modes::obs_mode_roaming: obs_mode_text = "no clip"; break;
				case obs_modes::obs_mode_freezecam: obs_mode_text = "freeze"; break;
				default: break;
			}
			render::draw_text_string(x + 200 - (render::get_text_size(render::fonts::pixel, obs_mode_text).x + 4), spectator_box_y + spectator_text_offset + 3, render::fonts::pixel, obs_mode_text, false, color(255, 255, 255, 255));
			spectator_text_offset += text_dimensions[1] + 5; spectator_box_height += text_dimensions[1] + 5;
		}
	}
	if (spectator_text_offset > 0) {
		render::draw_filled_rect(x, spectator_box_y, 200, spectator_box_height, color(50, 50, 50, 150));
		render::draw_outline(x, spectator_box_y, 200, spectator_box_height, color(25, 25, 25, 150));
	}
}

void draw_fire_timer(entity_t* entity) {
	auto inferno = reinterpret_cast<inferno_t*>(entity);

	auto screen_origin = vec3_t();

	if (!math::world_to_screen(inferno->origin(), screen_origin))
		return;

	const auto spawn_time = inferno->get_spawn_time();
	const auto factor = ((spawn_time + inferno_t::get_expiry_time()) - interfaces::globals->cur_time) / inferno_t::get_expiry_time();

	static auto size = vec2_t(60, 4);

	render::draw_filled_rect(screen_origin.x - size.x * 0.5, screen_origin.y - size.y * 0.5, size.x, size.y, color(45, 45, 45, factor >= 0.2f ? 255 : 255 * (factor / 0.2f)));
	render::draw_filled_rect(screen_origin.x - size.x * 0.5 + 2, screen_origin.y - size.y * 0.5 + 1, (size.x - 4) * factor, size.y - 2, color(240, 100, 100, factor >= 0.2f ? 255 : 255 * (factor / 0.2f)));

	render::draw_outline(screen_origin.x - size.x * 0.5, screen_origin.y - size.y * 0.5, size.x, size.y, color(25, 25, 25, factor >= 0.2f ? 255 : 255 * (factor / 0.2f)));
}

void visuals::other_visuals::draw() {
	if (csgo::local_player->is_alive()) { //only draw when alive
		if (variables::visuals::crosshair_enable)
			draw_crosshair(variables::visuals::crosshair_recoil, variables::visuals::crosshair_outline, variables::visuals::crosshair_color);
		if (variables::visuals::visualize_choke_enable && variables::misc::fake_lag_enable)
			visualize_choke(); //visualized the amount of packets you choked through fake lag
	}
	if (variables::visuals::spectator_list_enable)
		draw_spectator_list(300, 10);
	for (int i = 1; i <= interfaces::entity_list->get_highest_index(); i++) {
		auto entity = reinterpret_cast<entity_t*>(interfaces::entity_list->get_client_entity(i));
		if (!entity)
			continue;
		switch (entity->client_class()->class_id) {
			case class_ids::cinferno:
				if (variables::visuals::fire_timer_enable)
					draw_fire_timer(entity); //shows time a molotov/incendiary will continue to burn
				break;
			default: break;
		}
	}
}