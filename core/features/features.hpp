#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"

struct aimbot_target {
	player_t* player;
	vec3_t point;
	float damage;
};

namespace aimbot {
	void run(c_usercmd* cmd);
	extern aimbot_target target;

	namespace autowall {
		float can_hit(player_t* player, vec3_t point);
	};
}

namespace visuals {
	namespace chams {
		bool render(void* ecx, void* ctx, void* state, const model_render_info_t& info, matrix_t* matrix);
	};

	namespace esp {
		void draw();
	};

	namespace other_visuals {
		void draw();
	};
}

namespace misc {
	void clantag_spammer();
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
		void fake_lag(bool& send_packet);
	};
}

namespace anti_aim {
	void desync(c_usercmd* cmd, bool& send_packet);
}