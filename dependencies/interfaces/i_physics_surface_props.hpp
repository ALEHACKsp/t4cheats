#pragma once

struct surface_physics_params_t {
	float friction;
	float elasticity; // collision elasticity - used to compute coefficient of restitution
	float density;    // physical density (in kg / m^3)
	float thickness;  // material thickness if not solid (sheet materials) in inches
	float dampening;
};

struct surface_audio_params_t {
	float reflectivity;            // like elasticity, but how much sound should be reflected by this surface
	float hardness_factor;         // like elasticity, but only affects impact sound choices
	float roughness_factor;        // like friction, but only affects scrape sound choices   
	float rough_threshold;         // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float hard_threshold;          // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float hard_velocity_threshold; // collision velocity > this causes "hard" impacts, < this causes "soft" impacts   
	float high_pitch_occlusion;    //a value between 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float mid_pitch_occlusion;
	float low_pitch_pcclusion;
};

struct surface_sound_names_t {
	unsigned short step_left;
	unsigned short step_right;
	unsigned short impact_soft;
	unsigned short impact_hard;
	unsigned short scrape_smooth;
	unsigned short scrape_rough;
	unsigned short bullet_impact;
	unsigned short rolling;
	unsigned short break_sound;
	unsigned short strain_sound;
};

struct surface_game_props_t {
public:
	float max_speed_factor;        //0x0000
	float jump_factor;             //0x0004
	char pad00[0x4];               //0x0008
	float penetration_modifier;    //0x000C
	float damage_modifier;         //0x0010
	unsigned short material;       //0x0014
	char pad01[0x3];

};

struct surface_data_t {
	surface_physics_params_t physics_params;
	surface_audio_params_t   audio_params;
	surface_sound_names_t    sound_names;
	surface_game_props_t     game_props;
};

class i_physics_surface_props {
public:
	virtual ~i_physics_surface_props(void) {}
	virtual int parse_surface_data(const char* filename, const char* textfile) = 0;
	virtual int surface_prop_count(void) const = 0;
	virtual int get_surface_index(const char* surface_prop_name) const = 0;
	virtual void get_physics_properties(int surface_data_index, float* density, float thickness, float friction, float elasticity) const = 0;
	virtual surface_data_t* get_surface_data(int surface_data_index) = 0;
	virtual const char get_string(unsigned short string_table_index) const = 0;
	virtual const char get_prop_name(int surface_data_index) const = 0;
	virtual void set_world_material_index_table(int* map_array, int map_size) = 0;
	virtual void get_physics_parameters(int surface_data_index, surface_physics_params_t* params_out) const = 0;
};