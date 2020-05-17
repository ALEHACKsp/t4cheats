#include "../features.hpp"

void anti_aim::desync(c_usercmd* cmd, bool& send_packet) {
	if (true)
		return;
	static int dir = 0;
	static bool desync_side = true;
	float side_amount = (csgo::local_player->flags() & fl_ducking) ? 3.3f : 1.1f;
	if (csgo::local_player->velocity().Length2D() > side_amount) {
		if (send_packet == 0) {
			cmd->viewangles.y += desync_side ? -120.0f : 120.0f;
		}
	}
	else {
		switch (dir) {
		case 1:
			cmd->sidemove = side_amount;
			cmd->viewangles.y += desync_side ? 120.f : -120.f;
			send_packet = false;
			break;
		case 2:
			cmd->sidemove = 0.0f;
			cmd->viewangles.y += desync_side ? -120.f : 120.f;
			send_packet = false;
			break;
		case 3:
			cmd->sidemove = 0.0f;
			send_packet = true;
			break;
		default:
			dir = 0;
			cmd->viewangles.y += desync_side ? 120.f : -120.f;
			cmd->sidemove = -side_amount;
			send_packet = false;
			break;
		};
		dir++;
	}
	cmd->viewangles.clamp();
}