#pragma once

#include "../../dependencies/utilities/csgo.hpp"
#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../menu/menu.hpp"
#include "../../dependencies/utilities/virtual_method.h"

static std::uintptr_t reset_address;
static HWND wnd;

HRESULT D3DAPI reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto result = hooks::reset_original(device, params);
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

HRESULT D3DAPI present(IDirect3DDevice9* device, const RECT* source, const RECT* dest, HWND window_override, const RGNDATA* dirty_region) {
	static const auto once = [&]() {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(wnd);
		ImGui_ImplDX9_Init(device);

		menu::init();

		return true;
	}();

	IDirect3DVertexDeclaration9* vertexDeclaration;
	device->GetVertexDeclaration(&vertexDeclaration);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame(); { //all ImGui rendering in here
		menu::render();
	} ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	device->SetVertexDeclaration(vertexDeclaration);
	vertexDeclaration->Release();

	return hooks::present_original(device, source, dest, window_override, dirty_region);
}

LRESULT __stdcall wnd_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	if (GetAsyncKeyState(VK_INSERT) & 1)
		variables::menu::opened = !variables::menu::opened;

	LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam);
	interfaces::inputsystem->enable_input(!variables::menu::opened);

	return CallWindowProc(hooks::wnd_proc_original, window, message, wparam, lparam);
}

bool __fastcall create_move(void* ecx, void* edx, int input_sample_frametime, c_usercmd* cmd) {
	bool result = hooks::original_create_move(ecx, input_sample_frametime, cmd);

	if (!cmd || !cmd->command_number)
		return result;

	csgo::local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	std::uintptr_t* frame_pointer;
	__asm mov frame_pointer, ebp;
	bool& send_packet = *reinterpret_cast<bool*>(*frame_pointer - 0x1C);

	float old_forwardmove = cmd->forwardmove;
	float old_sidemove = cmd->sidemove;
	vec3_t old_viewangles = cmd->viewangles;

	const bool is_alive = csgo::local_player->is_alive();

	misc::movement::bunny_hop(cmd);
	if (csgo::local_player && is_alive)
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

	cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
	cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);
	cmd->upmove = std::clamp(cmd->sidemove, -320.0f, 320.0f);

	cmd->viewangles.normalize();
	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;

	return false;
}

struct renderable_info {
	entity_t* renderable;
	std::byte pad_18[18];
	std::uint16_t flags;
	std::uint16_t flags2;
};

int __fastcall list_leaves_in_box(void* ecx, void*, const vec3_t& mins, const vec3_t& maxs, unsigned short* list, int list_max) {
	static const auto list_leaves = utilities::pattern_scan("client_panorama.dll", "56 52 FF 50 18") + 5;

	if (variables::visuals::chams_enable && std::uintptr_t(_ReturnAddress()) == *list_leaves) {
		if (auto info = *reinterpret_cast<renderable_info**>(std::uintptr_t(_AddressOfReturnAddress()) + 0x14); info && info->renderable) {
			if (auto entity = virtual_method_handler::call<entity_t*, 7>(info->renderable - 4); entity && entity->is_player()) {
				info->flags &= ~0x100;
				info->flags2 |= 0x40;

				constexpr float maxCoord = 16384.f;
				constexpr vec3_t min{ -maxCoord, -maxCoord, -maxCoord };
				constexpr vec3_t max{ maxCoord, maxCoord, maxCoord };

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

		const std::string watermark_text = "t4cheats | " + menu::get_caption() + " | " + utilities::get_time_as_string();
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

void __stdcall draw_model_execute(void* ctx, void* state, const model_render_info_t& info, matrix_t* custom_bone_to_world)
{
	if (!csgo::local_player || interfaces::studio_render->is_force_material_override())
		return hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, custom_bone_to_world);

	if (visuals::chams::render(ctx, state, info, custom_bone_to_world))
		hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, custom_bone_to_world);

	interfaces::studio_render->forced_material_override(nullptr);
}

void __stdcall lock_cursor()
{
	if (variables::menu::opened)
		return interfaces::surface->unlock_cursor();

	return hooks::original_lock_cursor();
}

#define hook_method(target, function, original) \
if (MH_CreateHook(reinterpret_cast<void*>(target), &function, reinterpret_cast<void**>(&original)) != MH_OK) { \
    MessageBoxA(nullptr, "failed to intialize hook", "t4cheats", MB_OK | MB_ICONERROR); \
    std::exit(EXIT_FAILURE); \
}

bool hooks::initialize() {
	auto get_virtual = [](void* base, std::size_t index) -> void* {
		return reinterpret_cast<void*>(static_cast<unsigned int>((*reinterpret_cast<int**>(base))[index]));
	};

	wnd = FindWindowW(L"Valve001", NULL);
	wnd_proc_original = WNDPROC(SetWindowLongPtr(wnd, GWLP_WNDPROC, LONG_PTR(::wnd_proc)));

	reset_address = *reinterpret_cast<std::uintptr_t*>(utilities::pattern_scan("gameoverlayrenderer.dll", "53 57 C7 45") + 11);

	reset_original = *reinterpret_cast<decltype(reset_original)*>(reset_address);
	*reinterpret_cast<decltype(reset)**>(reset_address) = reset;

	present_original = *reinterpret_cast<decltype(present_original)*>(reset_address + 4);
	*reinterpret_cast<decltype(present)**>(reset_address + 4) = present;

	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("failed to initialize MH_Initialize.");
		return false;
	}

	hook_method(get_virtual(interfaces::clientmode, 24), create_move, original_create_move)
	hook_method(get_virtual(interfaces::engine->get_bsp_tree_query(), 6), list_leaves_in_box, original_list_leaves_in_box)
	hook_method(get_virtual(interfaces::panel, 41), paint_traverse, original_paint_traverse)
	hook_method(get_virtual(interfaces::model_render, 21), draw_model_execute, original_draw_model_execute)
	hook_method(get_virtual(interfaces::surface, 67), lock_cursor, original_lock_cursor)

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		throw std::runtime_error("failed to enable hooks.");
		return false;
	}

	console::log("[setup] hooks initialized!\n");
	return true;
}

void hooks::release() {
	interfaces::surface->unlock_cursor();
	interfaces::input->m_fCameraInThirdPerson = false;

	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);

	*reinterpret_cast<void**>(reset_address + 4) = present_original;
	SetWindowLongPtrA(wnd, GWLP_WNDPROC, LONG_PTR(wnd_proc_original));

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}