#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "variables.hpp"
#include <filesystem>
#include "config_system.hpp"

namespace menu {
	inline std::string caption = "";

	void init(HWND wnd);
	void render();
	void toggle();
};