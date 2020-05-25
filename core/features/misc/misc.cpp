#include "../features.hpp"

#include "../../../dependencies/interfaces/i_client_state.hpp"

void misc::clantag_spammer() {
	static float last_change = 0.f;
	if (interfaces::global_vars->realtime - last_change >= 0.25f) {
		std::string clantag = "";
		switch (int(interfaces::global_vars->cur_time * 4.f) % 41) { //add latency for better sync
			case 0:  clantag = "               "; break;
			case 1:  clantag = "              t"; break;
			case 2:  clantag = "             t4"; break;
			case 3:  clantag = "            t4c"; break;
			case 4:  clantag = "           t4ch"; break;
			case 5:  clantag = "          t4che"; break;
			case 6:  clantag = "         t4chea"; break;
			case 7:  clantag = "        t4cheat"; break;
			case 8:  clantag = "       t4cheats"; break;
			case 9:  clantag = "      t4cheats "; break;
			case 10:  clantag = "     t4cheats  "; break;
			case 11:  clantag = "    t4cheats   "; break;
			case 12:  clantag = "   t4cheats    "; break;
			case 13:  clantag = "  t4cheats     "; break;
			case 14:  clantag = " t4cheats      "; break;
			case 15:  clantag = "t4cheats       "; break;
			case 16:  clantag = "4cheats        "; break;
			case 17:  clantag = "cheats         "; break;
			case 18:  clantag = "heats          "; break;
			case 19:  clantag = "eats           "; break;
			case 20:  clantag = "ats            "; break;
			case 21:  clantag = "ts             "; break;
			case 22:  clantag = "s              "; break;
			case 23:  clantag = "               "; break;
			case 24:  clantag = "t"; break;
			case 25:  clantag = "t4"; break;
			case 26:  clantag = "t4"; break;
			case 27:  clantag = "4"; break;
			case 28:  clantag = " "; break;
			case 29:  clantag = "c"; break;
			case 30:  clantag = "ch"; break;
			case 31:  clantag = "che"; break;
			case 32:  clantag = "chea"; break;
			case 33:  clantag = "cheat"; break;
			case 34:  clantag = "cheats"; break;
			case 35:  clantag = "cheats"; break;
			case 36:  clantag = "heats"; break;
			case 37:  clantag = "eats"; break;
			case 38:  clantag = "ats"; break;
			case 39:  clantag = "ts"; break;
			case 40:  clantag = "s"; break;
			case 41:  clantag = " "; break;
		}
		utilities::set_clantag(clantag);
		last_change = interfaces::global_vars->realtime;
	}
}

void misc::movement::bunny_hop(c_usercmd* cmd) { //any public source, was already in here
	if (!variables::bunnyhop)
		return;

	static bool last_jumped = false, should_fake = false;

	if (!last_jumped && should_fake) {
		should_fake = false;
		cmd->buttons |= c_usercmd::buttons::in_jump;
	}
	else if (cmd->buttons & c_usercmd::buttons::in_jump) {
		if (csgo::local_player->flags() & fl_onground) {
			last_jumped = true;
			should_fake = true;
		}
		else {
			cmd->buttons &= ~c_usercmd::buttons::in_jump;
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
	std::srand(std::time(0));
	send_packet = interfaces::clientstate->net_channel->choked_packets >= (variables::misc::fake_lag_max - (variables::misc::fake_lag_jitter > 0 ? std::rand() % variables::misc::fake_lag_jitter : 0));
};