#pragma once

struct c_usercmd;

namespace prediction {
	void start(c_usercmd* cmd);
	void end();

	inline float old_cur_time;
	inline float old_frame_time;
	inline int* prediction_random_seed;
};