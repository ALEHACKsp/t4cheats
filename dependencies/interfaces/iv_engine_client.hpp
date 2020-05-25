#pragma once

#include <cstdint>
#include <d3d9types.h>

#include "../utilities/virtual_method.h"

struct player_info_t {
	std::int64_t version;
	union {
		std::int64_t xuid;
		struct {
			int xuid_low;
			int xuid_high;
		};
	};
	char name[128];
	int user_id;
	char guid[33];
	std::uint32_t friends_id;
	char friends_name[128];
	bool fake_player;
	bool is_hltv;
	unsigned int custom_files[4];
	unsigned char files_downloaded;
	int entity_index;
};

class engine_client {
public:
	virtual_method(void, get_screen_size(int& width, int& height), 5, (this, std::ref(width), std::ref(height)))
	virtual_method(bool, get_player_info(int index, player_info_t* info), 8, (this, index, info))
	virtual_method(int, get_local_player(), 12, (this))
	virtual_method(bool, is_in_game(), 26, (this))
	virtual_method(bool, is_connected(), 27, (this))
	virtual_method(const D3DMATRIX&, world_to_screen_matrix(), 37, (this))
	virtual_method(void*, get_bsp_tree_query(), 43, (this))
};