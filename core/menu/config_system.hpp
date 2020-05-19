#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../../dependencies/json/json.hpp"
#include "variables.hpp"

namespace config {
	void initialize();
	void load(const std::string& name);
	void save(const std::string& name);
}