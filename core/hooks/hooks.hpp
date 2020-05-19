#pragma once

namespace hooks {
	bool initialize();
	void release();

	LRESULT __stdcall wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept;
	using present_fn = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);

	long __stdcall present(IDirect3DDevice9* device, RECT* source_rect, RECT* dest_rect, HWND dest_window_override, RGNDATA* dirty_region) noexcept;

	inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*reinterpret_cast<int**>(_class))[index]); }

	namespace create_move {
		using fn = bool(__stdcall*)(float, c_usercmd*);
		static bool __fastcall hook(void* ecx, void* edx, int input_sample_frametime, c_usercmd* cmd);
	};

	namespace paint_traverse {
		using fn = void(__thiscall*)(i_panel*, unsigned int, bool, bool);
		static void __stdcall hook(unsigned int panel, bool force_repaint, bool allow_force);
	}

	namespace draw_model_execute {
		using fn = void(__thiscall*)(void*, void*, void*, const model_render_info_t&, matrix_t*);
		static void __fastcall hook(void* ecx, void*, void* ctx, void* state, const model_render_info_t& info, matrix_t* custom_bone_to_world);
	}
}
