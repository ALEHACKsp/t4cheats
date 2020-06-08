#pragma once

#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../menu/menu.hpp"

#include "../../dependencies/interfaces/i_input_system.hpp"
#include "../../dependencies/interfaces/i_client_entity_list.hpp"
#include "../../dependencies/interfaces/iv_engine_client.hpp"
#include "../../dependencies/interfaces/i_studio_render.h"
#include "../../dependencies/interfaces/i_panel.hpp"
#include "../../dependencies/utilities/renderer/renderer.hpp"
#include "../../dependencies/utilities/virtual_method.h"
#include "../../source-sdk/math/vector2d.hpp"

static std::uintptr_t reset_address;
static std::uintptr_t set_cursor_pos_address;

static HWND window;
static HMODULE module;

LRESULT __stdcall wnd_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	static const auto once = [](HWND window) {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(window);

		try {
			hooks::hook_methods();
		}
		catch (const std::runtime_error& error) {
			FreeConsole();
			FreeLibraryAndExitThread(module, 0);
		}

		visuals::chams::initialize();
		menu::init();

		return true;
	}(window);

	if (GetAsyncKeyState(VK_PRIOR)) // page up to uninject
		hooks::release();

	static float last_data_gather = 0.f;
	if (last_data_gather != interfaces::global_vars->realtime) {
		math::update_view_matrix();

		last_data_gather = interfaces::global_vars->realtime;
	}

	LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam);
	interfaces::inputsystem->enable_input(!variables::menu::opened);

	return CallWindowProc(hooks::wnd_proc_original, window, message, wparam, lparam);
}

HRESULT D3DAPI reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	return hooks::reset_original(device, params);
}

HRESULT D3DAPI present(IDirect3DDevice9* device, const RECT* source, const RECT* dest, HWND window_override, const RGNDATA* dirty_region) {
	static const bool dx9_init = ImGui_ImplDX9_Init(device);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame(); { //all ImGui rendering in here
		menu::render();

		if (ImGui::IsKeyPressed(VK_INSERT, false)) {
			variables::menu::opened = !variables::menu::opened;
			if (!variables::menu::opened)
				interfaces::inputsystem->reset_input_state();
		}
		ImGui::GetIO().MouseDrawCursor = variables::menu::opened;
	} ImGui::EndFrame();

	ImGui::Render();

	if (device->BeginScene() == D3D_OK) {
		IDirect3DVertexDeclaration9* vertexDeclaration;
		device->GetVertexDeclaration(&vertexDeclaration);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		device->SetVertexDeclaration(vertexDeclaration);
		vertexDeclaration->Release();
		device->EndScene();
	}

	return hooks::present_original(device, source, dest, window_override, dirty_region);
}

BOOL WINAPI set_cursor_pos(int x, int y) {
	return variables::menu::opened || hooks::set_cursor_pos_original(x, y);
}

bool __stdcall create_move(int input_sample_frametime, c_usercmd* cmd) {
	const bool result = hooks::original_create_move(interfaces::clientmode, input_sample_frametime, cmd);

	if (!cmd->command_number)
		return result;

	std::uintptr_t* frame_pointer;
	__asm mov frame_pointer, ebp;
	bool& send_packet = *reinterpret_cast<bool*>(*frame_pointer - 0x1C);

	csgo::local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	const float old_forwardmove = cmd->forward_move;
	const float old_sidemove = cmd->side_move;
	const vec3_t old_viewangles = cmd->view_angles;

	const bool is_alive = csgo::local_player->is_alive();

	misc::movement::bunny_hop(cmd);
	if (is_alive)
		anti_aim::desync(cmd, send_packet);

	if (variables::misc::clantag_spammer_enable)
		misc::clantag_spammer();

	if (is_alive) {
		prediction::start(cmd); {
			aimbot::run(cmd);
		} prediction::end();

		if (variables::misc::fake_lag_enable)
			misc::movement::fake_lag(send_packet);
	}

	math::correct_movement(old_viewangles, cmd, old_forwardmove, old_sidemove);

	cmd->forward_move = std::clamp(cmd->forward_move, -450.f, 450.f);
	cmd->side_move = std::clamp(cmd->side_move, -450.f, 450.f);
	cmd->up_move = std::clamp(cmd->up_move, -320.f, 320.f);

	cmd->view_angles.normalize();
	cmd->view_angles.x = std::clamp(cmd->view_angles.x, -89.f, 89.f);
	cmd->view_angles.y = std::clamp(cmd->view_angles.y, -180.f, 180.f);
	cmd->view_angles.z = 0.0f;

	return false;
}

struct renderable_info {
	entity_t* renderable;
	std::byte pad_18[18];
	std::uint16_t flags;
	std::uint16_t flags2;
};

int __fastcall list_leaves_in_box(void* ecx, void*, const vec3_t& mins, const vec3_t& maxs, unsigned short* list, int list_max) {
	static const auto list_leaves = utilities::pattern_scan("client.dll", "56 52 FF 50 18") + 5;

	if (variables::visuals::chams_enable && std::uintptr_t(_ReturnAddress()) == *list_leaves) {
		if (auto info = *reinterpret_cast<renderable_info**>(std::uintptr_t(_AddressOfReturnAddress()) + 0x14); info && info->renderable) {
			if (auto entity = virtual_method_handler::call<entity_t*, 7>(info->renderable - 4); entity && entity->is_player()) {
				info->flags &= ~0x100;
				info->flags2 |= 0x40;

				constexpr float max_coord = 16384.f;
				constexpr vec3_t min{ -max_coord, -max_coord, -max_coord };
				constexpr vec3_t max{ max_coord, max_coord, max_coord };

				return hooks::original_list_leaves_in_box(ecx, std::cref(min), std::cref(max), list, list_max);
			}
		}
	}

	return hooks::original_list_leaves_in_box(ecx, std::cref(mins), std::cref(maxs), list, list_max);
}

void __stdcall paint_traverse(unsigned int panel, bool force_repaint, bool allow_force) {
	switch (fnv::hash(interfaces::panel->get_panel_name(panel))) {
	case fnv::hash("MatSystemTopPanel"): {
		if (interfaces::engine->is_in_game() && csgo::local_player) {
			if (variables::visuals::esp_enable)
				visuals::esp::draw();

			visuals::other_visuals::draw();
		}

		const std::string watermark_text = "t4cheats | " + std::string(menu::get_caption()) + " | " + utilities::get_time_as_string();
		const float watermark_width = render::get_text_size(render::fonts::main, watermark_text).x;

		render::draw_filled_rect(11, 11, watermark_width + 8, 18, color(50, 50, 50, 150));
		render::draw_outline(10, 10, watermark_width + 9, 19, color(25, 25, 25, 150));
		render::draw_gradient(color(255, 100, 0, 150), color(255, 200, 0, 150), 10, 10, watermark_width + 9, 1, true);
		render::draw_text_string(15, 13, render::fonts::main, watermark_text, false, color::white(255));;
	}
	break;
	}

	hooks::original_paint_traverse(interfaces::panel, panel, force_repaint, allow_force);
}

void __stdcall draw_model_execute(void* ctx, void* state, const model_render_info_t& info, matrix_t* custom_bone_to_world) {
	if (!csgo::local_player || interfaces::studio_render->is_force_material_override())
		return hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, custom_bone_to_world);

	if (visuals::chams::render(ctx, state, info, custom_bone_to_world))
		hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, custom_bone_to_world);

	interfaces::studio_render->forced_material_override(nullptr);
}

DWORD WINAPI wait_on_unload(HMODULE module) {
	interfaces::inputsystem->enable_input(true);
	// interfaces::input->m_fCameraInThirdPerson = false;

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	FreeConsole();
	FreeLibraryAndExitThread(module, 0);
}

void hooks::initialize(HMODULE module) {
    #ifdef _DEBUG
	    console::initialize("console");
    #endif

	try {
		interfaces::initialize();
		render::initialize();
	}
	catch (const std::runtime_error& error) {
		MessageBoxA(NULL, error.what(), "whoops, there was an error!", MB_OK | MB_ICONERROR);
		FreeConsole();
		FreeLibraryAndExitThread(module, 0);
	}

	config::initialize();

	::module = module;

	window = FindWindow("Valve001", nullptr);
	wnd_proc_original = WNDPROC(SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(wnd_proc)));
}

#define hook_method(target, function, original) \
if (MH_CreateHook(target, &function, reinterpret_cast<void**>(&original)) != MH_OK) \
    throw std::runtime_error("failed to initialize hook");

void hooks::hook_methods() {
	auto get_virtual = [](void* base, std::size_t index) -> void* {
		return reinterpret_cast<void*>(static_cast<unsigned int>((*reinterpret_cast<int**>(base))[index]));
	};

	if (MH_Initialize() != MH_OK)
		throw std::runtime_error("failed to initialize hooks");

	reset_address = *reinterpret_cast<std::uintptr_t*>(utilities::pattern_scan("gameoverlayrenderer.dll", "53 57 C7 45") + 11);
	set_cursor_pos_address = *reinterpret_cast<std::uintptr_t*>(utilities::pattern_scan("gameoverlayrenderer.dll", "C2 08 ? 5D") + 6);

	reset_original = *reinterpret_cast<decltype(reset_original)*>(reset_address);
	*reinterpret_cast<decltype(reset)**>(reset_address) = reset;

	present_original = *reinterpret_cast<decltype(present_original)*>(reset_address + 4);
	*reinterpret_cast<decltype(present)**>(reset_address + 4) = present;

	set_cursor_pos_original = *reinterpret_cast<decltype(set_cursor_pos_original)*>(set_cursor_pos_address);
	*reinterpret_cast<decltype(set_cursor_pos)**>(set_cursor_pos_address) = set_cursor_pos;

	hook_method(get_virtual(interfaces::clientmode, 24), create_move, original_create_move)
	hook_method(get_virtual(interfaces::engine->get_bsp_tree_query(), 6), list_leaves_in_box, original_list_leaves_in_box)
	hook_method(get_virtual(interfaces::panel, 41), paint_traverse, original_paint_traverse)
	hook_method(get_virtual(interfaces::model_render, 21), draw_model_execute, original_draw_model_execute)

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		throw std::runtime_error("failed to enable hooks");

	console::log("[setup] hooks initialized!\n");
}

void hooks::release() {
	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);

	*reinterpret_cast<void**>(reset_address) = reset_original;
	*reinterpret_cast<void**>(reset_address + 4) = present_original;
	*reinterpret_cast<void**>(set_cursor_pos_address) = set_cursor_pos_original;

	SetWindowLongPtrA(window, GWLP_WNDPROC, LONG_PTR(wnd_proc_original));

	if (HANDLE handle = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(wait_on_unload), module, 0, nullptr))
		CloseHandle(handle);
}