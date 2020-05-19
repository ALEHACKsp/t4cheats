#include "config_system.hpp"

static std::filesystem::path path;
static std::string folder_name;

void config::initialize() {
	folder_name = "t4cheats";

	path = "C:/";
	path /= folder_name;

	std::error_code ec;

	if (!std::filesystem::is_directory(path, ec)) {
		std::filesystem::remove(path, ec);
		std::filesystem::create_directory(path, ec);
	}
}

void config::save(const std::string& name) {
	Json::Value j;
	Json::Value& save = j[folder_name];

	save["bunnyhop"] = variables::bunnyhop;

	save["aimbot_enable"] = variables::aimbot::aimbot_enable;
	save["aimbot_minimum_damage"] = variables::aimbot::aimbot_minimum_damage;
	save["aimbot_minimum_hitchance"] = variables::aimbot::aimbot_minimum_hitchance;
	save["aimbot_multipoint_body"] = variables::aimbot::aimbot_multipoint_body;
	save["aimbot_multipoint_enable"] = variables::aimbot::aimbot_multipoint_enable;
	save["aimbot_multipoint_head"] = variables::aimbot::aimbot_multipoint_head;
	save["aimbot_silent"] = variables::aimbot::aimbot_silent;

	save["chams_colors_enemies_invisible_r"] = variables::visuals::chams_colors::enemies_invisible.r;
	save["chams_colors_enemies_invisible_g"] = variables::visuals::chams_colors::enemies_invisible.g;
	save["chams_colors_enemies_invisible_b"] = variables::visuals::chams_colors::enemies_invisible.b;
	save["chams_colors_enemies_invisible_a"] = variables::visuals::chams_colors::enemies_invisible.a;
	save["chams_colors_enemies_visible_r"] = variables::visuals::chams_colors::enemies_visible.r;
	save["chams_colors_enemies_visible_g"] = variables::visuals::chams_colors::enemies_visible.g;
	save["chams_colors_enemies_visible_b"] = variables::visuals::chams_colors::enemies_visible.b;
	save["chams_colors_enemies_visible_a"] = variables::visuals::chams_colors::enemies_visible.a;
	save["chams_colors_team_invisible_r"] = variables::visuals::chams_colors::team_invisible.r;
	save["chams_colors_team_invisible_g"] = variables::visuals::chams_colors::team_invisible.g;
	save["chams_colors_team_invisible_b"] = variables::visuals::chams_colors::team_invisible.b;
	save["chams_colors_team_invisible_a"] = variables::visuals::chams_colors::team_invisible.a;
	save["chams_colors_team_visible_r"] = variables::visuals::chams_colors::team_visible.r;
	save["chams_colors_team_visible_g"] = variables::visuals::chams_colors::team_visible.g;
	save["chams_colors_team_visible_b"] = variables::visuals::chams_colors::team_visible.b;
	save["chams_colors_team_visible_a"] = variables::visuals::chams_colors::team_visible.a;
	save["chams_enable"] = variables::visuals::chams_enable;
	save["chams_through_walls"] = variables::visuals::chams_through_walls;
	save["crosshair_color_r"] = variables::visuals::crosshair_color.r;
	save["crosshair_color_g"] = variables::visuals::crosshair_color.g;
	save["crosshair_color_b"] = variables::visuals::crosshair_color.b;
	save["crosshair_color_a"] = variables::visuals::crosshair_color.a;
	save["crosshair_enable"] = variables::visuals::crosshair_enable;
	save["crosshair_outline"] = variables::visuals::crosshair_outline;
	save["crosshair_recoil"] = variables::visuals::crosshair_recoil;
	save["esp_enable"] = variables::visuals::esp_enable;
	save["esp_enemies_only"] = variables::visuals::esp_enemies_only;
	save["esp_show_box"] = variables::visuals::esp_show_box;
	save["esp_show_headdot"] = variables::visuals::esp_show_headdot;
	save["esp_show_healthbar"] = variables::visuals::esp_show_healthbar;
	save["esp_show_name"] = variables::visuals::esp_show_name;
	save["esp_show_weapon"] = variables::visuals::esp_show_weapon;
	save["fire_timer_enable"] = variables::visuals::fire_timer_enable;
	save["spectator_list_enable"] = variables::visuals::spectator_list_enable;
	save["visualize_choke_enable"] = variables::visuals::visualize_choke_enable;

	save["fake_lag_enable"] = variables::misc::fake_lag_enable;
	save["fake_lag_jitter"] = variables::misc::fake_lag_jitter;
	save["fake_lag_max"] = variables::misc::fake_lag_max;

	std::error_code ec;

	if (!std::filesystem::is_directory(path, ec)) {
		std::filesystem::remove(path, ec);
		std::filesystem::create_directory(path, ec);
	}

	if (std::ofstream out{ path / name.c_str() }; out.good())
		out << j;
}

void config::load(const std::string& name) {
	Json::Value j;

	if (std::ifstream in{ path / name }; in.good())
		in >> j;
	else
		return;

	Json::Value& load = j[folder_name];

	variables::bunnyhop = load["bunnyhop"].asBool();

	variables::aimbot::aimbot_enable = load["aimbot_enable"].asBool();
	variables::aimbot::aimbot_minimum_damage = load["aimbot_minimum_damage"].asFloat();
	variables::aimbot::aimbot_minimum_hitchance = load["aimbot_minimum_hitchance"].asFloat();
	variables::aimbot::aimbot_multipoint_body = load["aimbot_multipoint_body"].asFloat();
	variables::aimbot::aimbot_multipoint_enable = load["aimbot_multipoint_enable"].asBool();
	variables::aimbot::aimbot_multipoint_head = load["aimbot_multipoint_head"].asFloat();
	variables::aimbot::aimbot_silent = load["aimbot_silent"].asBool();

	variables::visuals::chams_colors::enemies_invisible.r = load["chams_colors_enemies_invisible_r"].asInt();
	variables::visuals::chams_colors::enemies_invisible.g = load["chams_colors_enemies_invisible_g"].asInt();
	variables::visuals::chams_colors::enemies_invisible.b = load["chams_colors_enemies_invisible_b"].asInt();
	variables::visuals::chams_colors::enemies_invisible.a = load["chams_colors_enemies_invisible_a"].asInt();
	variables::visuals::chams_colors::enemies_visible.r = load["chams_colors_enemies_visible_r"].asInt();
	variables::visuals::chams_colors::enemies_visible.g = load["chams_colors_enemies_visible_g"].asInt();
	variables::visuals::chams_colors::enemies_visible.b = load["chams_colors_enemies_visible_b"].asInt();
	variables::visuals::chams_colors::enemies_visible.a = load["chams_colors_enemies_visible_a"].asInt();
	variables::visuals::chams_colors::team_invisible.r = load["chams_colors_team_invisible_r"].asInt();
	variables::visuals::chams_colors::team_invisible.g = load["chams_colors_team_invisible_g"].asInt();
	variables::visuals::chams_colors::team_invisible.b = load["chams_colors_team_invisible_b"].asInt();
	variables::visuals::chams_colors::team_invisible.a = load["chams_colors_team_invisible_a"].asInt();
	variables::visuals::chams_colors::team_visible.r = load["chams_colors_team_visible_r"].asInt();
	variables::visuals::chams_colors::team_visible.g = load["chams_colors_team_visible_g"].asInt();
	variables::visuals::chams_colors::team_visible.b = load["chams_colors_team_visible_b"].asInt();
	variables::visuals::chams_colors::team_visible.a = load["chams_colors_team_visible_a"].asInt();
	variables::visuals::chams_enable = load["chams_enable"].asBool();
	variables::visuals::chams_through_walls = load["chams_through_walls"].asBool();
	variables::visuals::crosshair_color.r = load["crosshair_color_r"].asInt();
	variables::visuals::crosshair_color.g = load["crosshair_color_g"].asInt();
	variables::visuals::crosshair_color.b = load["crosshair_color_b"].asInt();
	variables::visuals::crosshair_color.a = load["crosshair_color_a"].asInt();
	variables::visuals::crosshair_enable = load["crosshair_enable"].asBool();
	variables::visuals::crosshair_outline = load["crosshair_outline"].asBool();
	variables::visuals::crosshair_recoil = load["crosshair_recoil"].asBool();
	variables::visuals::esp_enable = load["esp_enable"].asBool();
	variables::visuals::esp_enemies_only = load["esp_enemies_only"].asBool();
	variables::visuals::esp_show_box = load["esp_show_box"].asBool();
	variables::visuals::esp_show_headdot = load["esp_show_headdot"].asBool();
	variables::visuals::esp_show_healthbar = load["esp_show_healthbar"].asBool();
	variables::visuals::esp_show_name = load["esp_show_name"].asBool();
	variables::visuals::esp_show_weapon = load["esp_show_weapon"].asBool();
	variables::visuals::fire_timer_enable = load["fire_timer_enable"].asBool();
	variables::visuals::spectator_list_enable = load["spectator_list_enable"].asBool();
	variables::visuals::visualize_choke_enable = load["visualize_choke_enable"].asBool();

	variables::misc::fake_lag_enable = load["fake_lag_enable"].asBool();
	variables::misc::fake_lag_jitter = load["fake_lag_jitter"].asInt();
	variables::misc::fake_lag_max = load["fake_lag_max"].asInt();
}