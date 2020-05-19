#include "../dependencies/utilities/csgo.hpp"
#include "features/features.hpp"
#include "menu/config_system.hpp"

unsigned long WINAPI initialize(void* instance) {
#ifdef _DEBUG
	console::initialize("console");
#endif

	try {
		interfaces::initialize();
		render::initialize();
		hooks::initialize();
		config::initialize();
	}
	catch (const std::runtime_error & error) {
		MessageBoxA(NULL, error.what(), "whoops, there was an error!", MB_OK | MB_ICONERROR);
		FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
	}

	while (!GetAsyncKeyState(VK_PRIOR)) //key to uninject the cheat 'PAGEUP'
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}

unsigned long WINAPI release() {
	hooks::release();

#ifdef _DEBUG
	console::release();
#endif

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH: {
		    CreateThread(nullptr, NULL, LPTHREAD_START_ROUTINE(initialize), module, NULL, nullptr);
		} break;
	case DLL_PROCESS_DETACH: {
			release();
		} break;
	}

	return TRUE;
}
