#include <cassert>
#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <Windows.h>

#include "console.hpp"

void console::initialize(const char* title) {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdin), "conin$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "conout$", "w", stdout);
	SetConsoleTitle(title);
}

void console::release() {
	FreeConsole();
}