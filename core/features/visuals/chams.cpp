#include "../features.hpp"
#include "../../../dependencies/interfaces/i_studio_render.h"

static void override_material(bool ignorez, bool flat, const color& color) {
	static const auto material_shaded = interfaces::material_system->find_material("debug/debugambientcube", TEXTURE_GROUP_MODEL);
	static const auto material_flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL);

	const auto material = flat ? material_flat : material_shaded;

	// change the color, alpha, and set whether to render through walls
	material->color_modulate(color.r / 255.f, color.g / 255.f, color.b / 255.f);
	material->alpha_modulate(color.a / 255.f);
	material->set_material_var_flag(material_var_flags_t::material_var_ignorez, ignorez);

	interfaces::model_render->override_material(material);
}

bool visuals::chams::render(void* ctx, void* state, const model_render_info_t& info, matrix_t* matrix) {
	bool should_redraw = true;

	if (!variables::visuals::chams_enable || !std::string_view{ info.model->name }._Starts_with("models/player"))
		return should_redraw;

	const auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
	if (!player || player == csgo::local_player || !player->is_alive() || player->is_dormant()) // don't override the local player for now
		return should_redraw;

	if (player->is_enemy(csgo::local_player)) {
		if (variables::visuals::chams_through_walls) {
			override_material(true, true, variables::visuals::chams_colors::enemies_invisible);
			hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, matrix);
		}
		override_material(false, false, variables::visuals::chams_colors::enemies_visible);
		hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, matrix);
		should_redraw = false;
	}
	else {
		if (variables::visuals::chams_through_walls) {
			override_material(true, true, variables::visuals::chams_colors::team_invisible);
			hooks::original_draw_model_execute(interfaces::model_render, ctx, state, info, matrix);
		}
		override_material(false, false, variables::visuals::chams_colors::team_visible);
	}

	return should_redraw;
}