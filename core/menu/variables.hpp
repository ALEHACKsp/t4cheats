#pragma once

namespace variables { //you maybe dont like my style of organizing, but i think it looks alright
	inline bool bunnyhop = false;
	inline float test_float = 0.f;

	namespace visuals {
		inline bool esp_enable = false;
		inline bool esp_show_name = true;
		inline bool esp_show_box = true;
		inline bool esp_show_weapon = true;
		inline bool esp_show_healthbar = true;
		inline bool esp_show_headdot = false;
		namespace esp_colors {
			inline color enemies_visible = color(255, 200, 0, 200);
			inline color enemies_invisible = color(255, 0, 0, 200);
			inline color team_visible = color(0, 255, 0, 200);
			inline color team_invisible = color(0, 0, 255, 200);
		}
		inline bool chams_enable = false;
		inline bool chams_through_walls = true;
		namespace chams_colors {
			inline color enemies_visible = color(255, 200, 0);
			inline color enemies_invisible = color(255, 0, 0);
			inline color team_visible = color(0, 255, 0);
			inline color team_invisible = color(0, 0, 255);
		}
		inline bool crosshair_enable = true;
		inline bool crosshair_outline = true;
		inline bool crosshair_recoil = true;
		inline color crosshair_color = color::white();
	}

	namespace menu {
		inline bool opened = false;
		inline int x = 140, y = 140;
		inline int w = 400, h = 300;
	}
}