#pragma once

#include "../utilities/virtual_method.h"

struct c_usercmd;
class player_t;

struct player_move_data {
	char pad[182]; // size of move data
};

class player_move_helper {
public:
	virtual_method(void, set_host(player_t* host), 1, (this, host))
};

class player_game_movement {
public:
	virtual_method(void, process_movement(player_t* player, player_move_data* moveData), 1, (this, player, moveData))
	virtual_method(void, start_track_prediction_errors(player_t* player), 3, (this, player))
	virtual_method(void, finish_track_prediction_errors(player_t* player), 4, (this, player))
};

class player_prediction {
public:
	virtual_method(void, setup_move(player_t* player, c_usercmd* cmd, player_move_helper* helper, player_move_data* data), 20, (this, player, cmd, helper, data))
	virtual_method(void, finish_move(player_t* player, c_usercmd* cmd, player_move_data* data), 21, (this, player, cmd, data))
};