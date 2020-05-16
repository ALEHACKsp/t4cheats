#include "../features.hpp"

void misc::movement::bunny_hop(c_usercmd* cmd) { //any public source, was already in here
	if (!variables::bunnyhop)
		return;

	static bool last_jumped = false, should_fake = false;

	if (!last_jumped && should_fake) {
		should_fake = false;
		cmd->buttons |= in_jump;
	}
	else if (cmd->buttons & in_jump) {
		if (csgo::local_player->flags() & fl_onground) {
			last_jumped = true;
			should_fake = true;
		}
		else {
			cmd->buttons &= ~in_jump;
			last_jumped = false;
		}
	}
	else {
		last_jumped = false;
		should_fake = false;
	}
};

void misc::movement::fake_lag(bool& send_packet) {
	//dont send a packet if your current amount of choked packets is below or equal to your maximum fakelag value minus your jitter
	send_packet = interfaces::clientstate->net_channel->choked_packets >= (variables::misc::fake_lag_max - (variables::misc::fake_lag_jitter > 0 ? rand() % variables::misc::fake_lag_jitter : 0));
};