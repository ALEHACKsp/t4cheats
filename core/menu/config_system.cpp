#include "config_system.hpp"

std::string directory_path;
#define config_header "t4cheats"

void config::initialize() {
	directory_path = "C:/t4cheats";

	if (!std::filesystem::exists(directory_path))
		std::filesystem::create_directories(directory_path);
}
void config::save(const std::string name) {
	const auto path = directory_path + "/" + name;
	std::ofstream out(path);
	if (!out.is_open())
		return;

	Json::Value save;

	///////////////////

	save[config_header]["bunnyhop"] = variables::bunnyhop;

	save[config_header]["aimbot_enable"] = variables::aimbot::aimbot_enable;
	save[config_header]["aimbot_minimum_damage"] = variables::aimbot::aimbot_minimum_damage;
	save[config_header]["aimbot_minimum_hitchance"] = variables::aimbot::aimbot_minimum_hitchance;
	save[config_header]["aimbot_multipoint_body"] = variables::aimbot::aimbot_multipoint_body;
	save[config_header]["aimbot_multipoint_enable"] = variables::aimbot::aimbot_multipoint_enable;
	save[config_header]["aimbot_multipoint_head"] = variables::aimbot::aimbot_multipoint_head;
	save[config_header]["aimbot_silent"] = variables::aimbot::aimbot_silent;

	save[config_header]["chams_colors_enemies_invisible_r"] = variables::visuals::chams_colors::enemies_invisible.r;
	save[config_header]["chams_colors_enemies_invisible_g"] = variables::visuals::chams_colors::enemies_invisible.g;
	save[config_header]["chams_colors_enemies_invisible_b"] = variables::visuals::chams_colors::enemies_invisible.b;
	save[config_header]["chams_colors_enemies_invisible_a"] = variables::visuals::chams_colors::enemies_invisible.a;
	save[config_header]["chams_colors_enemies_visible_r"] = variables::visuals::chams_colors::enemies_visible.r;
	save[config_header]["chams_colors_enemies_visible_g"] = variables::visuals::chams_colors::enemies_visible.g;
	save[config_header]["chams_colors_enemies_visible_b"] = variables::visuals::chams_colors::enemies_visible.b;
	save[config_header]["chams_colors_enemies_visible_a"] = variables::visuals::chams_colors::enemies_visible.a;
	save[config_header]["chams_colors_team_invisible_r"] = variables::visuals::chams_colors::team_invisible.r;
	save[config_header]["chams_colors_team_invisible_g"] = variables::visuals::chams_colors::team_invisible.g;
	save[config_header]["chams_colors_team_invisible_b"] = variables::visuals::chams_colors::team_invisible.b;
	save[config_header]["chams_colors_team_invisible_a"] = variables::visuals::chams_colors::team_invisible.a;
	save[config_header]["chams_colors_team_visible_r"] = variables::visuals::chams_colors::team_visible.r;
	save[config_header]["chams_colors_team_visible_g"] = variables::visuals::chams_colors::team_visible.g;
	save[config_header]["chams_colors_team_visible_b"] = variables::visuals::chams_colors::team_visible.b;
	save[config_header]["chams_colors_team_visible_a"] = variables::visuals::chams_colors::team_visible.a;
	save[config_header]["chams_enable"] = variables::visuals::chams_enable;
	save[config_header]["chams_through_walls"] = variables::visuals::chams_through_walls;
	save[config_header]["crosshair_color_r"] = variables::visuals::crosshair_color.r;
	save[config_header]["crosshair_color_g"] = variables::visuals::crosshair_color.g;
	save[config_header]["crosshair_color_b"] = variables::visuals::crosshair_color.b;
	save[config_header]["crosshair_color_a"] = variables::visuals::crosshair_color.a;
	save[config_header]["crosshair_enable"] = variables::visuals::crosshair_enable;
	save[config_header]["crosshair_outline"] = variables::visuals::crosshair_outline;
	save[config_header]["crosshair_recoil"] = variables::visuals::crosshair_recoil;
	save[config_header]["esp_enable"] = variables::visuals::esp_enable;
	save[config_header]["esp_show_box"] = variables::visuals::esp_show_box;
	save[config_header]["esp_show_headdot"] = variables::visuals::esp_show_headdot;
	save[config_header]["esp_show_healthbar"] = variables::visuals::esp_show_healthbar;
	save[config_header]["esp_show_name"] = variables::visuals::esp_show_name;
	save[config_header]["esp_show_weapon"] = variables::visuals::esp_show_weapon;
	save[config_header]["fire_timer_enable"] = variables::visuals::fire_timer_enable;
	save[config_header]["spectator_list_enable"] = variables::visuals::spectator_list_enable;
	save[config_header]["visualize_choke_enable"] = variables::visuals::visualize_choke_enable;

	save[config_header]["fake_lag_enable"] = variables::misc::fake_lag_enable;
	save[config_header]["fake_lag_jitter"] = variables::misc::fake_lag_jitter;
	save[config_header]["fake_lag_max"] = variables::misc::fake_lag_max;

	///////////////////

	out << save;
	out.close();
}

void config::load(const std::string name) {
	const auto path = directory_path + "/" + name;
	std::ifstream in(path);

	if (!in.good())
		save(name);

	if (!in.is_open())
		return;

	Json::Value load;

	in >> load;

	///////////////////

	variables::bunnyhop = load[config_header]["bunnyhop"].asBool();

	variables::aimbot::aimbot_enable = load[config_header]["aimbot_enable"].asBool();
	variables::aimbot::aimbot_minimum_damage = load[config_header]["aimbot_minimum_damage"].asFloat();
	variables::aimbot::aimbot_minimum_hitchance = load[config_header]["aimbot_minimum_hitchance"].asFloat();
	variables::aimbot::aimbot_multipoint_body = load[config_header]["aimbot_multipoint_body"].asFloat();
	variables::aimbot::aimbot_multipoint_enable = load[config_header]["aimbot_multipoint_enable"].asBool();
	variables::aimbot::aimbot_multipoint_head = load[config_header]["aimbot_multipoint_head"].asFloat();
	variables::aimbot::aimbot_silent = load[config_header]["aimbot_silent"].asBool();

	variables::visuals::chams_colors::enemies_invisible.r = load[config_header]["chams_colors_enemies_invisible_r"].asInt();
	variables::visuals::chams_colors::enemies_invisible.g = load[config_header]["chams_colors_enemies_invisible_g"].asInt();
	variables::visuals::chams_colors::enemies_invisible.b = load[config_header]["chams_colors_enemies_invisible_b"].asInt();
	variables::visuals::chams_colors::enemies_invisible.a = load[config_header]["chams_colors_enemies_invisible_a"].asInt();
	variables::visuals::chams_colors::enemies_visible.r = load[config_header]["chams_colors_enemies_visible_r"].asInt();
	variables::visuals::chams_colors::enemies_visible.g = load[config_header]["chams_colors_enemies_visible_g"].asInt();
	variables::visuals::chams_colors::enemies_visible.b = load[config_header]["chams_colors_enemies_visible_b"].asInt();
	variables::visuals::chams_colors::enemies_visible.a = load[config_header]["chams_colors_enemies_visible_a"].asInt();
	variables::visuals::chams_colors::team_invisible.r = load[config_header]["chams_colors_team_invisible_r"].asInt();
	variables::visuals::chams_colors::team_invisible.g = load[config_header]["chams_colors_team_invisible_g"].asInt();
	variables::visuals::chams_colors::team_invisible.b = load[config_header]["chams_colors_team_invisible_b"].asInt();
	variables::visuals::chams_colors::team_invisible.a = load[config_header]["chams_colors_team_invisible_a"].asInt();
	variables::visuals::chams_colors::team_visible.r = load[config_header]["chams_colors_team_visible_r"].asInt();
	variables::visuals::chams_colors::team_visible.g = load[config_header]["chams_colors_team_visible_g"].asInt();
	variables::visuals::chams_colors::team_visible.b = load[config_header]["chams_colors_team_visible_b"].asInt();
	variables::visuals::chams_colors::team_visible.a = load[config_header]["chams_colors_team_visible_a"].asInt();
	variables::visuals::chams_enable = load[config_header]["chams_enable"].asBool();
	variables::visuals::chams_through_walls = load[config_header]["chams_through_walls"].asBool();
	variables::visuals::crosshair_color.r = load[config_header]["crosshair_color_r"].asInt();
	variables::visuals::crosshair_color.g = load[config_header]["crosshair_color_g"].asInt();
	variables::visuals::crosshair_color.b = load[config_header]["crosshair_color_b"].asInt();
	variables::visuals::crosshair_color.a = load[config_header]["crosshair_color_a"].asInt();
	variables::visuals::crosshair_enable = load[config_header]["crosshair_enable"].asBool();
	variables::visuals::crosshair_outline = load[config_header]["crosshair_outline"].asBool();
	variables::visuals::crosshair_recoil = load[config_header]["crosshair_recoil"].asBool();
	variables::visuals::esp_enable = load[config_header]["esp_enable"].asBool();
	variables::visuals::esp_show_box = load[config_header]["esp_show_box"].asBool();
	variables::visuals::esp_show_headdot = load[config_header]["esp_show_headdot"].asBool();
	variables::visuals::esp_show_healthbar = load[config_header]["esp_show_healthbar"].asBool();
	variables::visuals::esp_show_name = load[config_header]["esp_show_name"].asBool();
	variables::visuals::esp_show_weapon = load[config_header]["esp_show_weapon"].asBool();
	variables::visuals::fire_timer_enable = load[config_header]["fire_timer_enable"].asBool();
	variables::visuals::spectator_list_enable = load[config_header]["spectator_list_enable"].asBool();
	variables::visuals::visualize_choke_enable = load[config_header]["visualize_choke_enable"].asBool();

	variables::misc::fake_lag_enable = load[config_header]["fake_lag_enable"].asBool();
	variables::misc::fake_lag_jitter = load[config_header]["fake_lag_jitter"].asInt();
	variables::misc::fake_lag_max = load[config_header]["fake_lag_max"].asInt();


	///////////////////
	
	in.close();
}