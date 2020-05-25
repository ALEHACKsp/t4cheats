#pragma once

#include <windows.h>
#include <cstdint>
#include <string>
#include <stdexcept>

#include "i_base_client_dll.hpp"
#include "i_client_entity_list.hpp"
#include "iv_engine_client.hpp"
#include "i_client_state.hpp"
#include "i_panel.hpp"
#include "i_surface.hpp"
#include "c_global_vars_base.hpp"
#include "i_material_system.hpp"
#include "iv_model_info.hpp"
#include "iv_model_render.hpp"
#include "i_console.hpp"
#include "i_game_event_manager.hpp"
#include "i_input.hpp"
#include "i_input_system.hpp"
#include "i_trace.hpp"
#include "i_render_view.hpp"
#include "i_player_movement.hpp"
#include "i_weapon_system.hpp"
#include "i_studio_render.h"
#include "i_physics_surface_props.hpp"

#include "../utilities/console/console.hpp"

namespace interfaces {
	void initialize();

	inline base_client_dll* client;
	inline i_input* input;
	inline i_client_entity_list* entity_list;
	inline engine_client* engine;
	inline void* clientmode;
	inline i_client_state* clientstate;
	inline i_panel* panel;
	inline i_surface* surface;
	inline global_vars_base* global_vars;
	inline i_material_system* material_system;
	inline iv_model_info* model_info;
	inline iv_model_render* model_render;
	inline i_studio_render* studio_render;
	inline i_physics_surface_props* physics_surface;
	inline i_render_view* render_view;
	inline i_console* console;
	inline i_game_event_manager2* event_manager;
	inline i_inputsytem* inputsystem;
	inline trace* trace_ray;
	inline player_game_movement* game_movement;
	inline player_prediction* prediction;
	inline player_move_helper* move_helper;
	inline i_weapon_system* weapon_system;
}