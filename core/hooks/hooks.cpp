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
	auto create_move_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 24));
	auto paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces::panel, 41));
	auto draw_model_execute = reinterpret_cast<void*>(get_virtual(interfaces::model_render, 21));

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
	MH_Uninitialize();
	**reinterpret_cast<void***>(present_address) = reinterpret_cast<void*>(present_original);
	SetWindowLongW(FindWindowW(L"Valve001", NULL), GWL_WNDPROC, reinterpret_cast<LONG>(wndproc_original));
	MH_DisableHook(MH_ALL_HOOKS);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hooks::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM	 lparam) noexcept {
	if (variables::menu::opened && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return true;
	return CallWindowProcW(wndproc_original, hwnd, message, wparam, lparam);
}

long __stdcall hooks::present(IDirect3DDevice9* pDevice, RECT* source_rect, RECT* dest_rect, HWND dest_window_override, RGNDATA* dirty_region) noexcept {
	static uintptr_t gameoverlay_return_address = NULL;
	if (!gameoverlay_return_address) {
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(_ReturnAddress(), &info, sizeof(MEMORY_BASIC_INFORMATION));
		char mod[MAX_PATH];
		GetModuleFileNameA((HMODULE)info.AllocationBase, mod, MAX_PATH);
		if (strstr(mod, "gameoverlay"))
			gameoverlay_return_address = (uintptr_t)(_ReturnAddress());
	}
	if (gameoverlay_return_address != (uintptr_t)(_ReturnAddress()))
		return present_original(pDevice, source_rect, dest_rect, dest_window_override, dirty_region);
	{ //disable csgo's color grading
		IDirect3DVertexDeclaration9* vertDec;
		IDirect3DVertexShader9* vertShader;
		pDevice->GetVertexDeclaration(&vertDec);
		pDevice->GetVertexShader(&vertShader);
		pDevice->SetVertexDeclaration(nullptr);
		pDevice->SetVertexShader(nullptr);
		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetVertexDeclaration(vertDec);
		pDevice->SetVertexShader(vertShader);
	}
	csgo::device = pDevice;
	static bool init = false;
	if (!init) {
		menu::init(wnd);
		init = true;
	}
	menu::toggle();
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame(); { //all ImGui rendering in here
		menu::render();
	}
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
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

	auto old_viewangles = cmd->viewangles;
	auto old_forwardmove = cmd->forwardmove;
	auto old_sidemove = cmd->sidemove;

	misc::movement::bunny_hop(cmd);
	if (csgo::local_player && csgo::local_player->is_alive())
		anti_aim::desync(cmd, send_packet);

	prediction::start(cmd); {
		if (csgo::local_player->is_alive()) {
			aimbot::run(cmd);
			if (variables::misc::fake_lag_enable)
				misc::movement::fake_lag(send_packet);
		}

	} prediction::end();

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
	auto panel_to_draw = fnv::hash(interfaces::panel->get_panel_name(panel));

	switch (panel_to_draw) {
	case fnv::hash("MatSystemTopPanel"): {
			if (interfaces::engine->is_in_game() && csgo::local_player) {
				if (variables::visuals::esp_enable)
					visuals::esp::draw();
				visuals::other_visuals::draw();
			}

			std::string watermark_text = "t4cheats | " + menu::caption + " | " + utilities::get_time_as_string();
			auto watermark_width = render::get_text_size(render::fonts::main, watermark_text).x;
			render::draw_filled_rect(11, 11, watermark_width + 8, 18, color(50, 50, 50, 150));
			render::draw_outline(10, 10, watermark_width + 9, 19, color(25, 25, 25, 150));
			render::draw_gradient(color(255, 100, 0, 150), color(255, 200, 0, 150), 10, 10, watermark_width + 9, 1, true);
			render::draw_text_string(15, 13, render::fonts::main, watermark_text, false, color::white(255));;
		}
		break;
	case fnv::hash("FocusOverlayPanel"):
		interfaces::panel->set_mouse_input_enabled(panel, variables::menu::opened); //disable mouse input when in menu
		break;
	}

	paint_traverse_original(interfaces::panel, panel, force_repaint, allow_force);
}

void __fastcall hooks::draw_model_execute::hook(void* _this, int edx, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& pInfo, matrix_t* pCustomBoneToWorld)
{
	if (interfaces::engine->is_in_game() && interfaces::engine->is_connected() && csgo::local_player) {
		const auto mdl = pInfo.model;
		if (!mdl)
			return;
		bool is_player = strstr(mdl->name, "models/player") != nullptr;
		if (!interfaces::studio_render->is_force_material_override() && is_player) {
			visuals::chams::render(ctx, state, pInfo, pCustomBoneToWorld);
			dme_original(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
			interfaces::model_render->override_material(nullptr);
		}
		else
			dme_original(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
	}

}