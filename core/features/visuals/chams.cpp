#include "../features.hpp"
#include "../../../dependencies/interfaces/i_studio_render.h"

extern hooks::draw_model_execute::fn dme_original;

static i_material* material = nullptr;

void override_material(bool ignorez, bool flat, const color& color) {
	//finding materials
	auto material_shaded = interfaces::material_system->find_material("debug/debugambientcube", TEXTURE_GROUP_MODEL);
	auto material_flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL);

	material = flat ? material_flat : material_shaded; //use flat / shaded material

	material->color_modulate(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f); //change color of the material
	material->alpha_modulate(color.a / 255.0f); //change opacity of the material

	material->set_material_var_flag(material_var_ignorez, ignorez); //set ignorez flag (if visible through walls)
	interfaces::model_render->override_material(material); //override material of the model
}

bool visuals::chams::render(void* ecx, void* ctx, void* state, const model_render_info_t& info, matrix_t* matrix) {
	bool should_redraw = true;

	if (!variables::visuals::chams_enable)
		return should_redraw;

	if (const std::string_view model_name = info.model->name; !model_name._Starts_with("models/player"))
		return should_redraw;

	const auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
	if (!player || !player->is_alive() || player->dormant() || player == csgo::local_player) //dont override localplayer for now
		return should_redraw;

	if (player->is_enemy(csgo::local_player)) {
		if (variables::visuals::chams_through_walls) { //do we want to see people through walls?
			override_material(true, true, variables::visuals::chams_colors::enemies_invisible); //override ignorez material
			dme_original(ecx, ctx, state, info, matrix); //render first layer
		}
		override_material(false, false, variables::visuals::chams_colors::enemies_visible); //override regular material
		dme_original(ecx, ctx, state, info, matrix); //render second layer
		should_redraw = false;
	}
	else {
		if (variables::visuals::chams_through_walls) { //do we want to see people through walls?
			override_material(true, true, variables::visuals::chams_colors::team_invisible); //override ignorez material
			dme_original(ecx, ctx, state, info, matrix); //render first layer
		}
		override_material(false, false, variables::visuals::chams_colors::team_visible); //override regular material
	}

	return should_redraw;
}