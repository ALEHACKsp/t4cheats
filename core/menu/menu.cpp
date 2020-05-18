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
	ImGui::GetStyle().ScrollbarSize = 8.f;
	ImGui::GetStyle().WindowPadding = ImVec2(0, 8);

	ImGui::GetStyle().WindowMinSize = ImVec2(400, 300);

	menu::caption = captions[rand() % 8];
}

void aimbot_window() {
	ImGui::BeginChild("Aimbot Child"); {

	}
	ImGui::EndChild();
}

void visuals_window() {
	ImGui::BeginChild("Visuals Child"); {
		ImGui::Checkbox("Esp enable", &variables::visuals::esp_enable);
		ImGui::Checkbox("Esp enemies only", &variables::visuals::esp_enemies_only);
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
	ImGui::EndChild();
}

void misc_window() {
	ImGui::BeginChild("Misc Child"); {
		ImGui::Checkbox("Bunnyhop", &variables::bunnyhop);
		ImGui::Separator();
		ImGui::Checkbox("Fake lag enable", &variables::misc::fake_lag_enable);
		ImGui::SliderInt("Fake lag max choke", &variables::misc::fake_lag_max, 0, 6);
		ImGui::SliderInt("Fake lag jitter", &variables::misc::fake_lag_jitter, 0, 6);
		ImGui::Separator();
		ImGui::Checkbox("Clantag spammer", &variables::misc::clantag_spammer_enable);
		static char clantag_text[15];
		ImGui::InputText("Clantag", clantag_text, sizeof(clantag_text));
		if (ImGui::Button("Apply clantag"))
			utilities::set_clantag(clantag_text);
	}
	ImGui::EndChild();
}

void config_window() {
	static auto is_configs_loaded = false;
	ImGui::BeginChild("Config Child"); {
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
	ImGui::EndChild();
}

void menu::render() {
	if (font)
		ImGui::PushFont(font);
	if (variables::menu::opened) {
		std::string text = "t4cheats - " + caption;
		static int tab = 0;
		ImGui::Begin(text.c_str(), nullptr, ImGuiWindowFlags_NoCollapse); {
			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, 65);
			if (ImGui::Button("Aimbot", ImVec2(50, 25)))  tab = 0;
			if (ImGui::Button("Visuals", ImVec2(50, 25))) tab = 1;
			if (ImGui::Button("Misc", ImVec2(50, 25)))    tab = 2;
			if (ImGui::Button("Config", ImVec2(50, 25)))  tab = 3;
			ImGui::NextColumn();
			switch (tab) {
				case 0: aimbot_window(); break;
				case 1: visuals_window(); break;
				case 2: misc_window(); break;
				case 3: config_window(); break;
				default: break;
			}
		}
		ImGui::End();
	}
	if (font)
		ImGui::PopFont();
}

void menu::toggle() {
	if (GetAsyncKeyState(VK_INSERT) & 1) //switch opened state when menu key 'INSERT' is pressed
		variables::menu::opened = !variables::menu::opened;
}