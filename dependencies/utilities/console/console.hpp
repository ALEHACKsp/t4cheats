#pragma once

namespace console {
	void initialize(const char* title);
	void release();

	template <typename ...args>
	void log(const char* format, args... _args) {
    #ifdef _DEBUG
		std::printf(format, _args...);
    #endif
	}
}