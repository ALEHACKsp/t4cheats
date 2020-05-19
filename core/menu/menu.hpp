#pragma once

#include <filesystem>

#include "../../dependencies/utilities/csgo.hpp"
#include "variables.hpp"
#include "config_system.hpp"

namespace menu {
	inline std::string caption = "";

	void init(HWND wnd);
	void render();
	void toggle();
};