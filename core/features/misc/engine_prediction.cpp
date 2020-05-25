#include "engine_prediction.hpp"
#include "../../../dependencies/utilities/csgo.hpp"

static player_move_data move_data;

void prediction::start(c_usercmd* cmd) {
	if (!prediction_random_seed)
		prediction_random_seed = *reinterpret_cast<int**>(utilities::pattern_scan("client_panorama.dll", sig_prediction_random_seed) + 2);

	*prediction_random_seed = cmd->random_seed;

	old_cur_time = interfaces::global_vars->cur_time;
	old_frame_time = interfaces::global_vars->frame_time;

	interfaces::global_vars->cur_time = csgo::local_player->get_tick_base() * interfaces::global_vars->interval_per_tick;
	interfaces::global_vars->frame_time = interfaces::global_vars->interval_per_tick;

	interfaces::game_movement->start_track_prediction_errors(csgo::local_player);

	std::memset(&move_data, 0, sizeof(player_move_data));
	interfaces::move_helper->set_host(csgo::local_player);
	interfaces::prediction->setup_move(csgo::local_player, cmd, interfaces::move_helper, &move_data);
	interfaces::game_movement->process_movement(csgo::local_player, &move_data);
	interfaces::prediction->finish_move(csgo::local_player, cmd, &move_data);
}

void prediction::end() {
	interfaces::game_movement->finish_track_prediction_errors(csgo::local_player);
	interfaces::move_helper->set_host(nullptr);

	*prediction_random_seed = -1;

	interfaces::global_vars->cur_time = old_cur_time;
	interfaces::global_vars->frame_time = old_cur_time;
}
