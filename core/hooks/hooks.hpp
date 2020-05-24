#pragma once

#include <cassert>
#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <Windows.h>

struct c_usercmd;
class i_panel;
class iv_model_render;
class matrix_t;
struct model_render_info_t;
class vec3_t;

namespace hooks {
	void initialize(HMODULE);
	void hook_methods();
	void release();

	inline std::add_pointer_t<HRESULT D3DAPI(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> reset_original;
	inline std::add_pointer_t<HRESULT D3DAPI(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> present_original;
	inline std::add_pointer_t<BOOL WINAPI(int, int)> set_cursor_pos_original;
	inline WNDPROC wnd_proc_original;

	inline bool(__thiscall* original_create_move)(void*, float, c_usercmd*);
	inline int(__thiscall* original_list_leaves_in_box)(void*, const vec3_t&, const vec3_t&, unsigned short*, int);
	inline void(__thiscall* original_paint_traverse)(i_panel*, unsigned int, bool, bool);
	inline void(__thiscall* original_draw_model_execute)(iv_model_render*, void*, void*, const model_render_info_t&, matrix_t*);
	inline void(__stdcall* original_lock_cursor)();
}