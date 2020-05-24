#include "../dependencies/utilities/csgo.hpp"
#include "menu/config_system.hpp"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH)
		hooks::initialize(module);

	return TRUE;
}