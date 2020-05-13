#pragma once

namespace variables {
	inline bool bunnyhop = false;
	inline float test_float = 0.f;

	namespace visuals {
		inline bool esp_enable = false;
		inline bool esp_show_name = true;
		inline bool esp_show_box = true;
		inline bool esp_show_weapon = true;
		inline bool esp_show_healthbar = true;
	}

	namespace menu {
		inline bool opened = false;
		inline int x = 140, y = 140;
		inline int w = 400, h = 300;
	}
}