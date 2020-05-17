#include "csgo.hpp"

namespace csgo {
	player_t* local_player = nullptr;
	IDirect3DDevice9* device = nullptr;

	namespace fonts {
		unsigned long watermark_font;
		unsigned long name_font;
	}
}