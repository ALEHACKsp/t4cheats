#pragma once

struct global_vars_base {
public:
	const float realtime;
	const int frame_count;
	const float absolute_frametime;
private:
	const float pad;
public:
	float cur_time;
	float frame_time;
	const int max_clients;
	const int tick_count;
	const float interval_per_tick;
};