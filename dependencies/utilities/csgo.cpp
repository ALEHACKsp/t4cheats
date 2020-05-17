#include "csgo.hpp"

namespace csgo {
	player_t* local_player = nullptr;
	IDirect3DDevice9* device = nullptr;
	vec3_t angles::fake = { 0, 0, 0 };
	vec3_t angles::real = { 0, 0, 0 };

	namespace fonts {
		unsigned long watermark_font;
		unsigned long name_font;
	}
}