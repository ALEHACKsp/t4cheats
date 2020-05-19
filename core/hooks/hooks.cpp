#pragma once

#include "../../dependencies/utilities/csgo.hpp"
#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../menu/menu.hpp"

hooks::create_move::fn create_move_original = nullptr;
hooks::paint_traverse::fn paint_traverse_original = nullptr;
hooks::draw_model_execute::fn dme_original = nullptr;

uint8_t* present_address;
hooks::present_fn present_original;
WNDPROC wndproc_original;
HWND wnd;

bool hooks::initialize() {
	void* create_move_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 24));
	void* paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces::panel, 41));
	void* draw_model_execute = reinterpret_cast<void*>(get_virtual(interfaces::model_render, 21));

	wnd = FindWindowW(L"Valve001", NULL);

	wndproc_original = reinterpret_cast<WNDPROC>(SetWindowLongW(wnd, GWL_WNDPROC, reinterpret_cast<LONG>(wndproc)));
	present_address = utilities::pattern_scan("gameoverlayrenderer.dll", "FF 15 ? ? ? ? 8B F8 85 DB") + 0x2;
	present_original = **reinterpret_cast<present_fn**>(present_address);
	**reinterpret_cast<void***>(present_address) = reinterpret_cast<void*>(&present);

	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("failed to initialize MH_Initialize.");
		return false;
	}

	if (MH_CreateHook(create_move_target, &create_move::hook, reinterpret_cast<void**>(&create_move_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize create_move. (outdated index?)");
		return false;
	}

	if (MH_CreateHook(paint_traverse_target, &paint_traverse::hook, reinterpret_cast<void**>(&paint_traverse_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize paint_traverse. (outdated index?)");
		return false;
	}

	if (MH_CreateHook(draw_model_execute, &draw_model_execute::hook, reinterpret_cast<void**>(&dme_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize draw_model_execute. (outdated index?)");
		return false;
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		throw std::runtime_error("failed to enable hooks.");
		return false;
	}

	console::log("[setup] hooks initialized!\n");
	return true;
}

void hooks::release() {
	interfaces::panel->set_mouse_input_enabled(true);
	interfaces::input->m_fCameraInThirdPerson = false;

	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);

	**reinterpret_cast<void***>(present_address) = present_original;
	SetWindowLongPtrA(wnd, GWLP_WNDPROC, LONG_PTR(wndproc_original));

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hooks::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM	 lparam) noexcept {
	if (GetAsyncKeyState(VK_INSERT) & 1) // switch opened state when menu key 'INSERT' is pressed
		variables::menu::opened = !variables::menu::opened;

	if (variables::menu::opened && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return true;

	return CallWindowProcW(wndproc_original, hwnd, message, wparam, lparam);
}

long __stdcall hooks::present(IDirect3DDevice9* pDevice, RECT* source_rect, RECT* dest_rect, HWND dest_window_override, RGNDATA* dirty_region) noexcept {
	static const auto once = [&]() {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(wnd);
		ImGui_ImplDX9_Init(pDevice);

		menu::init();

		return true;
	}();

	IDirect3DVertexDeclaration9* vertexDeclaration;
	pDevice->GetVertexDeclaration(&vertexDeclaration);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame(); { //all ImGui rendering in here
		menu::render();
	} ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	pDevice->SetVertexDeclaration(vertexDeclaration);
	vertexDeclaration->Release();

	return present_original(pDevice, source_rect, dest_rect, dest_window_override, dirty_region);
}

bool __fastcall hooks::create_move::hook(void* ecx, void* edx, int input_sample_frametime, c_usercmd* cmd) {
	create_move_original(input_sample_frametime, cmd);

	if (!cmd || !cmd->command_number)
		return create_move_original(input_sample_frametime, cmd);

	csgo::local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	uintptr_t* frame_pointer;
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

	send_packet ? csgo::angles::fake = cmd->viewangles : csgo::angles::real = cmd->viewangles;

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

void __stdcall hooks::paint_traverse::hook(unsigned int panel, bool force_repaint, bool allow_force) {
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
	case fnv::hash("FocusOverlayPanel"):
		interfaces::panel->set_mouse_input_enabled(panel, variables::menu::opened); //disable mouse input when in menu
		interfaces::panel->set_keyboard_input_enabled(panel, variables::menu::opened);
		break;
	default:
		break;
	}

	paint_traverse_original(interfaces::panel, panel, force_repaint, allow_force);
}

void __fastcall hooks::draw_model_execute::hook(void* ecx, void*, void* ctx, void* state, const model_render_info_t& info, matrix_t* custom_bone_to_world)
{
	if (!csgo::local_player || interfaces::studio_render->is_force_material_override())
		return dme_original(ecx, ctx, state, info, custom_bone_to_world);

	if (visuals::chams::render(ecx, ctx, state, info, custom_bone_to_world))
		dme_original(ecx, ctx, state, info, custom_bone_to_world);

	interfaces::studio_render->forced_material_override(nullptr);
}