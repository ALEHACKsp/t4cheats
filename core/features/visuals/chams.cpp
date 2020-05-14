#include "../features.hpp"
#include "../../../dependencies/interfaces/i_studio_render.h"

extern hooks::draw_model_execute::fn dme_original;

i_material* material = nullptr;

void override_material(bool ignorez, const color& color) {
	//finding materials
	auto material_shaded = interfaces::material_system->find_material("debug/debugambientcube", TEXTURE_GROUP_MODEL);
	auto material_flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL);

	material = material_shaded; //lets use this material (shaded)

	material->color_modulate(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f); //change color of the material
	material->alpha_modulate(color.a / 255.0f); //change opacity of the material

	material->set_material_var_flag(material_var_ignorez, ignorez); //set ignorez flag (if visible through walls)
	interfaces::model_render->override_material(material); //override material of the model
}

void visuals::chams::render(IMatRenderContext* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix) {
	auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
	if (!player || !player->is_alive() || player->dormant() || player == csgo::local_player) //dont override localplayer for now
		return;
	if (variables::visuals::chams_enable) {
		if (player->is_enemy(csgo::local_player)) {
			if (variables::visuals::chams_through_walls) { //do we want to see people through walls?
				override_material(true, variables::visuals::chams_colors::enemies_invisible); //override ignorez material
				dme_original(interfaces::model_render, 0, ctx, state, info, matrix); //render first layer
			}
			override_material(false, variables::visuals::chams_colors::enemies_visible); //override regular material
			dme_original(interfaces::model_render, 0, ctx, state, info, matrix); //render second layer
		}
		else {
			if (variables::visuals::chams_through_walls) { //do we want to see people through walls?
				override_material(true, variables::visuals::chams_colors::team_invisible); //override ignorez material
				dme_original(interfaces::model_render, 0, ctx, state, info, matrix); //render first layer
			}
			override_material(false, variables::visuals::chams_colors::team_visible); //override regular material
			dme_original(interfaces::model_render, 0, ctx, state, info, matrix); //render second layer
		}
	}
}