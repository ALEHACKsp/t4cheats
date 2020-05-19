#include "menu.hpp"

constexpr std::array captions{
	"we are back",
	"hL can't believe it",
	"maybe without crashes now?",
	"crashware 2.0",
	"funny yes",
	"lorem ipsum",
	"now in HD!",
	"dominating legits since 1000 BC"
};

static ImFont* font;

void menu::init(HWND wnd) {
	ImGui::CreateContext();

	const ImGuiIO& io = ImGui::GetIO();
	font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Tahoma.ttf", 12.0f, NULL, io.Fonts->GetGlyphRangesDefault());

	ImGui_ImplWin32_Init(wnd);
	ImGui_ImplDX9_Init(csgo::device);

	ImGui::StyleColorsDarker();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.f;
	style.ChildRounding = 0.f;
	style.FrameRounding = 0.f;
	style.GrabRounding = 0.f;
	style.PopupRounding = 0.f;
	style.ScrollbarRounding = 0.f;
	style.ScrollbarSize = 8.f;
	style.WindowPadding = ImVec2(0.f, 8.f);
	style.WindowMinSize = ImVec2(400.f, 300.f);

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
	ImGui::BeginChild("Config Child");

	const std::filesystem::path path = "C:/t4cheats";

	static int current_config = -1;
	static std::vector<std::string> configs;

	auto reload_configs = [&]() {
		configs.clear();

		std::error_code ec;

		if (!std::filesystem::is_directory(path, ec)) {
			std::filesystem::remove(path, ec);
			std::filesystem::create_directory(path, ec);
		}

		std::transform(std::filesystem::directory_iterator{ path, ec },
					   std::filesystem::directory_iterator{ },
					   std::back_inserter(configs),
					   [](const auto& entry) { return std::string{ (const char*)entry.path().filename().u8string().c_str() }; });
	};

	static const auto once = [&]() {
		reload_configs();
		return true;
	}();

	if (std::size_t(current_config) >= configs.size())
		current_config = -1;

	static std::string buffer;

	ImGui::InputText("##config_name", &buffer, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();

	if (ImGui::Button("Create")) {
		config::save(configs[current_config]);
		reload_configs();
	}

	if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(data);
		*out_text = vector[idx].c_str();
		return true;
	}, &configs, configs.size(), 5) && current_config != -1)
		buffer = configs[current_config];

	if (const std::string config = configs[current_config]; !config.empty()) {
		if (ImGui::Button("Load"))
			config::load(config);

		ImGui::SameLine();

		if (ImGui::Button("Save"))
			config::save(config);

		ImGui::SameLine();

		if (ImGui::Button("Delete")) {
			std::filesystem::remove(path / config);
			reload_configs();
		}

		ImGui::SameLine();
	}

	if (ImGui::Button("Refresh"))
		reload_configs();

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