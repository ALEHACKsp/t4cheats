#include "../features.hpp"

void anti_aim::desync(c_usercmd* cmd, bool& send_packet) {
	static bool desync_side = true;
	const float side_amount = (csgo::local_player->flags() & entity_flags::fl_ducking) ? 3.25f : 1.1f;

	if (std::abs(cmd->side_move) > side_amount) {
		if (!send_packet)
			cmd->view_angles.y += desync_side ? -120.f : 120.f;
	}
	else {
		static int dir = 0;

		switch (dir) {
		case 1:
			cmd->side_move = side_amount;
			cmd->view_angles.y += desync_side ? 120.f : -120.f;
			send_packet = false;
			break;
		case 2:
			cmd->side_move = 0.f;
			cmd->view_angles.y += desync_side ? -120.f : 120.f;
			send_packet = false;
			break;
		case 3:
			cmd->side_move = 0.f;
			send_packet = true;
			break;
		default:
			dir = 0;
			cmd->view_angles.y += desync_side ? 120.f : -120.f;
			cmd->side_move = -side_amount;
			send_packet = false;
			break;
		}

		++dir;
	}

	cmd->view_angles.clamp();
}