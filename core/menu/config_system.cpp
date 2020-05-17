#include "config_system.hpp"

std::string directory_path;
#define config_header "t4cheats"

void config::initialize() {
	directory_path = "C:/t4cheats";

	if (!std::filesystem::exists(directory_path))
		std::filesystem::create_directories(directory_path);
}
void config::save(const std::string name) {
	const auto path = directory_path + "/" + name;
	std::ofstream out(path);
	if (!out.is_open())
		return;

	Json::Value save;

	///////////////////

	save[config_header]["bunnyhop"] = variables::bunnyhop;

	///////////////////

	out << save;
	out.close();
}

void config::load(const std::string name) {
	const auto path = directory_path + "/" + name;
	std::ifstream in(path);

	if (!in.good())
		save(name);

	if (!in.is_open())
		return;

	Json::Value load;

	in >> load;

	///////////////////

	variables::bunnyhop = load[config_header]["bunnyhop"].asBool();

	///////////////////
	
	in.close();
}