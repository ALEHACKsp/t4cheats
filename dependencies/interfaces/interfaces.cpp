#include "interfaces.hpp"
#include "../utilities/csgo.hpp"

static void* find(const char* module, const char* name) {
	if (const auto create_interface = reinterpret_cast<std::add_pointer_t<void* __cdecl (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandle(module), "CreateInterface"))) {
		if (void* found_interface = create_interface(name, nullptr)) {
			std::printf("[hook] %s hook initialized at module %s\n", name, module);
			return found_interface;
		}
	}

	MessageBoxA(nullptr, ("failed to find " + std::string(name) + " interface!").c_str(), "t4cheats", MB_OK | MB_ICONERROR);
	std::exit(EXIT_FAILURE);
}

#define find_interface(type, module, version) \
reinterpret_cast<type*>(find(##module, version));

void interfaces::initialize() {
	client = find_interface(base_client_dll, "client_panorama", "VClient018");
	entity_list = find_interface(i_client_entity_list, "client_panorama.dll", "VClientEntityList003");
	engine = find_interface(engine_client, "engine.dll", "VEngineClient014");
	panel = find_interface(i_panel, "vgui2.dll", "VGUI_Panel009");
	surface = find_interface(i_surface, "vguimatsurface", "VGUI_Surface031");
	material_system = find_interface(i_material_system, "materialsystem", "VMaterialSystem080");
	model_info = find_interface(iv_model_info, "engine", "VModelInfoClient004");
	model_render = find_interface(iv_model_render, "engine", "VEngineModel016");
	studio_render = find_interface(i_studio_render, "studiorender", "VStudioRender026");
	physics_surface = find_interface(i_physics_surface_props, "vphysics", "VPhysicsSurfaceProps001");
	render_view = find_interface(i_render_view, "engine", "VEngineRenderView014");
	console = find_interface(i_console, "vstdlib", "VEngineCvar007");
	event_manager = find_interface(i_game_event_manager2, "engine", "GAMEEVENTSMANAGER002");
	inputsystem = find_interface(i_inputsytem, "inputsystem", "InputSystemVersion001");
	trace_ray = find_interface(trace, "engine", "EngineTraceClient004");
	game_movement = find_interface(player_game_movement, "client_panorama", "GameMovement001");
	prediction = find_interface(player_prediction, "client_panorama", "VClientPrediction001");

	clientmode = **reinterpret_cast<void***>((*reinterpret_cast<std::uintptr_t**>(client))[10] + 5);
	global_vars = **reinterpret_cast<global_vars_base***>((*reinterpret_cast<std::uintptr_t**>(client))[11] + 10);
	input = *reinterpret_cast<i_input**>((*reinterpret_cast<std::uintptr_t**>(client))[16] + 1);

	clientstate = **(i_client_state ***)(utilities::pattern_scan("engine.dll", sig_client_state) + 1);
	move_helper = **(player_move_helper***)(utilities::pattern_scan("client_panorama.dll", sig_player_move_helper) + 2);
	weapon_system = *(i_weapon_system**)(utilities::pattern_scan("client_panorama.dll", sig_weapon_data) + 2);

	console::log("[setup] interfaces initialized!\n");
}