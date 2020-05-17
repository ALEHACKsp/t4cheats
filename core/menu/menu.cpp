#include "menu.hpp"

namespace fs = std::filesystem;

std::string captions[] = {
	"we are back",
	"hL can't believe it",
	"maybe without crashes now?",
	"crashware 2.0",
	"funny yes",
	"lorem ipsum",
	"now in HD!",
	"dominating legits since 1000 BC"
};

ImFont* font;

void menu::init(HWND wnd) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Tahoma.ttf", 12.0f, NULL, io.Fonts->GetGlyphRangesDefault());
	ImGui_ImplWin32_Init(wnd);
	ImGui_ImplDX9_Init(csgo::device);
	ImGui::StyleColorsDarker();
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().GrabRounding = 0.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;

	ImGui::GetStyle().WindowMinSize = ImVec2(300, 300);

	menu::caption = captions[rand() % 8];
}

void aimbot_window(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(300, 500));

	ImGui::Begin("Aimbot", open, ImGuiWindowFlags_NoResize); {

	}
	ImGui::End();
}

void visuals_window(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(300, 500));

	ImGui::Begin("Visuals", open, ImGuiWindowFlags_NoResize); {
		ImGui::Checkbox("Esp enable", &variables::visuals::esp_enable);
		ImGui::Checkbox("Esp show name", &variables::visuals::esp_show_name);
		ImGui::Checkbox("Esp show box", &variables::visuals::esp_show_box);
		ImGui::Checkbox("Esp show healthbar", &variables::visuals::esp_show_healthbar);
		ImGui::Checkbox("Esp show weapon", &variables::visuals::esp_show_weapon);
		ImGui::Checkbox("Esp show headdot", &variables::visuals::esp_show_headdot);
		
		ImGui::Separator();

		ImGui::Checkbox("Chams enable", &variables::visuals::chams_enable);
		ImGui::ColorEdit4("Chams team visible", &variables::visuals::chams_colors::team_visible, ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit4("Chams enemies visible", &variables::visuals::chams_colors::enemies_visible, ImGuiColorEditFlags_NoInputs);
		ImGui::Checkbox("Chams through walls", &variables::visuals::chams_through_walls);
		ImGui::ColorEdit4("Chams team invisible", &variables::visuals::chams_colors::team_invisible, ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit4("Chams enemies invisible", &variables::visuals::chams_colors::enemies_invisible, ImGuiColorEditFlags_NoInputs);

		ImGui::Separator();

		ImGui::Checkbox("Crosshair enable", &variables::visuals::crosshair_enable);
		ImGui::Checkbox("Crosshair outline", &variables::visuals::crosshair_outline);
		ImGui::Checkbox("Crosshair recoil", &variables::visuals::crosshair_recoil);
		ImGui::ColorEdit4("Crosshair color", &variables::visuals::crosshair_color, ImGuiColorEditFlags_NoInputs);

		ImGui::Separator();

		ImGui::Checkbox("Visualize choke", &variables::visuals::visualize_choke_enable);
		ImGui::Checkbox("Spectator list", &variables::visuals::spectator_list_enable);
		ImGui::Checkbox("Fire timer", &variables::visuals::fire_timer_enable);

	}
	ImGui::End();
}

void misc_window(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(300, 500));

	ImGui::Begin("Misc", open, ImGuiWindowFlags_NoResize); {
		ImGui::Checkbox("Bunnyhop", &variables::bunnyhop);
		ImGui::Checkbox("Fake lag enable", &variables::misc::fake_lag_enable);
		ImGui::SliderInt("Fake lag max choke", &variables::misc::fake_lag_max, 0, 6);
		ImGui::SliderInt("Fake lag jitter", &variables::misc::fake_lag_jitter, 0, 6);
	}
	ImGui::End();
}

void config_window(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(300, 300));
	static auto is_configs_loaded = false;
	ImGui::Begin("Config", open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse); {
		static std::vector<std::string> configs;
		static auto load_configs = []() {
			std::vector<std::string> items = {};
			std::string path("C:/t4cheats");
			if (!fs::is_directory(path))
				fs::create_directories(path);
			for (auto& p : fs::directory_iterator(path))
				items.push_back(p.path().string().substr(path.length() + 1));

			return items;
		};
		static auto is_configs_loaded = false;
		if (!is_configs_loaded) {
			is_configs_loaded = true;
			configs = load_configs();
		}
		static std::string current_config;
		static char config_name[32];
		ImGui::InputText("##config_name", config_name, sizeof(config_name));
		ImGui::SameLine();
		if (ImGui::Button("Create")) {
			current_config = std::string(config_name);

			config::save(current_config);
			is_configs_loaded = false;
			memset(config_name, 0, 32);
		}
		ImGui::ListBoxHeader("##configs"); {
			for (auto& config : configs) {
				if (ImGui::Selectable(config.c_str(), config == current_config)) {
					current_config = config;
				}
			}
		}
		ImGui::ListBoxFooter();
		if (!current_config.empty()) {
			if (ImGui::Button("Load"))
				config::load(current_config);
			ImGui::SameLine();
			if (ImGui::Button("Save"))
				config::save(current_config);
			ImGui::SameLine();
			if (ImGui::Button("Delete") && fs::remove("C:/t4cheats/" + current_config)) {
				current_config.clear();
				is_configs_loaded = false;
			}
			ImGui::SameLine();
		}
		if (ImGui::Button("Refresh"))
			is_configs_loaded = false;
	}
	ImGui::End();
}

void menu::render() {
	if (font)
		ImGui::PushFont(font);
	if (variables::menu::opened) {
		bool aimbot_window_open = true;
		aimbot_window(&aimbot_window_open);
		bool visuals_window_open = true;
		visuals_window(&visuals_window_open);
		bool misc_window_open = true;
		misc_window(&misc_window_open);
		bool config_window_open = true;
		config_window(&config_window_open);
	}
	if (font)
		ImGui::PopFont();
}

void menu::toggle() {
	if (GetAsyncKeyState(VK_INSERT) & 1) //switch opened state when menu key 'INSERT' is pressed
		variables::menu::opened = !variables::menu::opened;
}