#pragma once

#include "../../dependencies/utilities/csgo.hpp"
#include "variables.hpp"
#include "config_system.hpp"

namespace menu {
	void init();
	void render();

	const char* get_caption();
};