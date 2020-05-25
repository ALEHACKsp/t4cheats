#pragma once

#include <memory>
#include <sstream>
#include <type_traits>
#include <Windows.h>

struct base_client_dll;
struct i_input;
class i_client_entity_list;
class engine_client;
class i_client_state;
class i_panel;
class i_surface;
struct global_vars_base;
class i_material_system;
class iv_model_info;
class iv_model_render;
class i_studio_render;
class i_physics_surface_props;
class i_cvar;
class i_game_event_manager2;
class i_inputsytem;
class trace;
class player_game_movement;
class player_prediction;
class player_move_helper;
class i_weapon_system;

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
	inline i_cvar* cvar;
	inline i_game_event_manager2* event_manager;
	inline i_inputsytem* inputsystem;
	inline trace* trace_ray;
	inline player_game_movement* game_movement;
	inline player_prediction* prediction;
	inline player_move_helper* move_helper;
	inline i_weapon_system* weapon_system;
}