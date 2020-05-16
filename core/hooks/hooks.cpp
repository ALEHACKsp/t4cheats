#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../menu/menu.hpp"

hooks::create_move::fn create_move_original = nullptr;
hooks::paint_traverse::fn paint_traverse_original = nullptr;
hooks::draw_model_execute::fn dme_original = nullptr;

bool hooks::initialize() {
	auto create_move_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 24));
	auto paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces::panel, 41));
	auto draw_model_execute = reinterpret_cast<void*>(get_virtual(interfaces::model_render, 21));

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

	MH_DisableHook(MH_ALL_HOOKS);
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

	prediction::start(cmd); {

		aimbot::run(cmd);

	} prediction::end();

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

color from_hsv(int h, double s, double v) {
	double C = s * v;
	double X = C * (1 - abs(fmod(h / 60.0, 2) - 1));
	double m = v - C;
	double Rs, Gs, Bs;

	if (h >= 0 && h < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (h >= 60 && h < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (h >= 120 && h < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (h >= 180 && h < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (h >= 240 && h < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	return color((Rs + m) * 255, (Gs + m) * 255, (Bs + m) * 255);
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
			render::draw_text_string(15, 13, render::fonts::main, watermark_text, false, color::white(255));;

			menu::toggle();
			menu::render();
		}
		break;
	case fnv::hash("FocusOverlayPanel"):
		//interfaces::panel->set_keyboard_input_enabled(panel, variables::menu::opened); //uncomment if you dont want to be able to walk when in menu
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