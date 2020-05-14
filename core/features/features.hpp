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
		void render(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix);
	};
	namespace esp {
		void draw();
	};
	namespace other_visuals {
		void draw();
	};
}

namespace misc {
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
	};
}