#pragma once

#include <array>

#include "collideable.hpp"
#include "client_class.hpp"
#include "../../dependencies/interfaces/iv_model_info.hpp"
#include "../../dependencies/math/math.hpp"
#include "../../dependencies/utilities/netvars/netvars.hpp"
#include "../../dependencies/utilities/virtual_method.h"

struct anim_state;
class matrix_t;
struct weapon_info_t;

class collideable_t {
public:
	virtual_method(const vec3_t&, mins(), 1, (this))
	virtual_method(const vec3_t&, maxs(), 2, (this))
};

enum class cs_weapon_type {
	knife = 0,
	pistol,
	submachine_gun,
	rifle,
	shotgun,
	sniper_rifle,
	machine_gun,
	c4,
	placeholder,
	grenade
};

enum client_frame_stage_t {
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

enum move_type {
	movetype_none = 0,
	movetype_isometric,
	movetype_walk,
	movetype_step,
	movetype_fly,
	movetype_flygravity,
	movetype_vphysics,
	movetype_push,
	movetype_noclip,
	movetype_ladder,
	movetype_observer,
	movetype_custom,
	movetype_last = movetype_custom,
	movetype_max_bits = 4,
	max_movetype
};

enum entity_flags {
	fl_onground = (1 << 0),
	fl_ducking = (1 << 1),
	fl_waterjump = (1 << 2),
	fl_ontrain = (1 << 3),
	fl_inrain = (1 << 4),
	fl_frozen = (1 << 5),
	fl_atcontrols = (1 << 6),
	fl_client = (1 << 7),
	fl_fakeclient = (1 << 8),
	fl_inwater = (1 << 9),
	fl_fly = (1 << 10),
	fl_swim = (1 << 11),
	fl_conveyor = (1 << 12),
	fl_npc = (1 << 13),
	fl_godmode = (1 << 14),
	fl_notarget = (1 << 15),
	fl_aimtarget = (1 << 16),
	fl_partialground = (1 << 17),
	fl_staticprop = (1 << 18),
	fl_graphed = (1 << 19),
	fl_grenade = (1 << 20),
	fl_stepmovement = (1 << 21),
	fl_donttouch = (1 << 22),
	fl_basevelocity = (1 << 23),
	fl_worldbrush = (1 << 24),
	fl_object = (1 << 25),
	fl_killme = (1 << 26),
	fl_onfire = (1 << 27),
	fl_dissolving = (1 << 28),
	fl_transragdoll = (1 << 29),
	fl_unblockable_by_player = (1 << 30)
};

enum item_definition_indexes {
	WEAPON_NONE = 0,
	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};

enum obs_modes {
	obs_mode_none = 0,
	obs_mode_deathcam,
	obs_mode_freezecam,
	obs_mode_fixed,
	obs_mode_in_eye,
	obs_mode_chase,
	obs_mode_roaming
};

class entity_t {
public:
	netvar("DT_CSPlayer", "m_fFlags", flags, int)
	netvar("DT_CSPlayer", "m_iTeamNum", team, int)
	netvar("DT_CSPlayer", "m_flSimulationTime", simulation_time, float)
	netvar("DT_CSPlayer", "m_nSurvivalTeam", survival_team, int)
	netvar("DT_BaseEntity", "m_bSpotted", spotted, bool)
	netvar("DT_BasePlayer", "m_vecOrigin", origin, vec3_t)
	netvar("DT_BasePlayer", "m_vecViewOffset[0]", view_offset, vec3_t)

	virtual_method(collideable_t*, get_collideable(), 3, (this))
	virtual_method(bool, is_alive(), 155, (this))
	virtual_method(bool, is_player(), 157, (this))
	virtual_method(const model_t*, get_model(), 8, (this + 4))
	virtual_method(const matrix_t&, coordinate_frame(), 32, (this + 4))
	virtual_method(c_client_class*, get_client_class(), 2, (this + 8))
	virtual_method(bool, is_dormant(), 9, (this + 8))
	virtual_method(int, index(), 10, (this + 8))
	virtual_method(bool, setup_bones(matrix_t* out, int max_bones, int mask, float time), 13, (this + 4, out, max_bones, mask, time))
};

class econ_view_item_t {
public:
	netvar("DT_ScriptCreatedItem", "m_bInitialized", is_initialized, bool)
	netvar("DT_ScriptCreatedItem", "m_iEntityLevel", entity_level, int)
	netvar("DT_ScriptCreatedItem", "m_iAccountID", account_id, int)
	netvar("DT_ScriptCreatedItem", "m_iItemIDLow", item_id_low, int)
};

class base_view_model_t : public entity_t {
public:
	netvar("DT_BaseViewModel", "m_nModelIndex", model_index, int)
	netvar("DT_BaseViewModel", "m_nViewModelIndex", view_model_index, int)
	netvar("DT_BaseViewModel", "m_hWeapon", m_hweapon, int)
	netvar("DT_BaseViewModel", "m_hOwner", m_howner, int)
};

class weapon_t : public entity_t {
public:
	netvar("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", next_primary_attack, float)
	netvar("DT_BaseCombatWeapon", "m_flNextSecondaryAttack", next_secondary_attack, float)
	netvar("DT_BaseCombatWeapon", "m_iClip1", ammo, int)
	netvar("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount", primary_reserve_ammo_acount, int)
	netvar("DT_WeaponCSBase", "m_flRecoilIndex", recoil_index, float)
	netvar("DT_WeaponCSBaseGun", "m_zoomLevel", zoom_level, float)
	netvar("DT_BaseAttributableItem", "m_iItemDefinitionIndex", item_definition_index, short)
	netvar("DT_BaseCombatWeapon", "m_iEntityQuality", entity_quality, int)

	virtual_method(weapon_info_t*, get_weapon_data(), 460, (this))
	virtual_method(float, get_spread(), 452, (this))
	virtual_method(float, get_inaccuracy(), 482, (this))
	virtual_method(void, update_accuracy_penalty(), 483, (this))

	std::string get_weapon_name() {
		switch (this->get_client_class()->class_id)
		{
		case class_ids::cak47: return "ak47";
		case class_ids::cbreachcharge: return "breachcharge";
		case class_ids::cbumpmine: return "bumpmine";
		case class_ids::cc4: return "c4";
		case class_ids::cdeagle: return "deagle";
		case class_ids::cdecoygrenade: return "decoygrenade";
		case class_ids::cflashbang: return "flashbang";
		case class_ids::chegrenade: return "hegrenade";
		case class_ids::cincendiarygrenade: return "incendiarygrenade";
		case class_ids::cknife: return "knife";
		case class_ids::cknifegg: return "golden knife";
		case class_ids::cmelee: return "melee";
		case class_ids::cfists: return "fists";
		case class_ids::cmolotovgrenade: return "molotovgrenade";
		case class_ids::cscar17: return "scar17";
		case class_ids::csensorgrenade: return "sensorgrenade";
		case class_ids::csmokegrenade: return "smokegrenade";
		case class_ids::csnowball: return "snowball";
		case class_ids::ctablet: return "tablet";
		case class_ids::cweaponaug: return "aug";
		case class_ids::cweaponawp: return "awp";
		case class_ids::cweaponbizon: return "bizon";
		case class_ids::cweaponelite: return "elites";
		case class_ids::cweaponfamas: return "famas";
		case class_ids::cweaponfiveseven: return "fiveseven";
		case class_ids::cweapong3sg1: return "g3sg1";
		case class_ids::cweapongalil: return "galil";
		case class_ids::cweapongalilar: return "galilar";
		case class_ids::cweaponglock: return "glock";
		case class_ids::cweaponhkp2000: return "hkp2000";
		case class_ids::cweaponm249: return "m249";
		case class_ids::cweaponm4a1: return "m4a1";
		case class_ids::cweaponmac10: return "mac10";
		case class_ids::cweaponmag7: return "mag7";
		case class_ids::cweaponmp5navy: return "mp5sd";
		case class_ids::cweaponmp7: return "mp7";
		case class_ids::cweaponmp9: return "mp9";
		case class_ids::cweaponnegev: return "negev";
		case class_ids::cweaponnova: return "nova";
		case class_ids::cweaponp250: return "p250";
		case class_ids::cweaponp90: return "p90";
		case class_ids::cweaponsawedoff: return "sawedoff";
		case class_ids::cweaponscar20: return "scar20";
		case class_ids::cweaponscout: return "scout";
		case class_ids::cweaponsg550: return "sg550";
		case class_ids::cweaponsg552: return "sg552";
		case class_ids::cweaponsg556: return "sg556";
		case class_ids::cweaponshield: return "shield";
		case class_ids::cweaponssg08: return "ssg08";
		case class_ids::cweapontaser: return "taser";
		case class_ids::cweapontec9: return "tec9";
		case class_ids::cweaponump45: return "ump45";
		case class_ids::cweaponusp: return "usp-s";
		case class_ids::cweaponxm1014: return "xm1014";
		default: return "";
		}
	}
};

class player_t : public entity_t {
private:
	template <typename T>
	T& read(uintptr_t offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	void write(uintptr_t offset, T data) {
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset) = data;
	}

public:
	netvar("DT_CSPlayer", "m_ArmorValue", armor, int)
	netvar("DT_CSPlayer", "m_bHasHelmet", has_helmet, bool)
	netvar("DT_CSPlayer", "m_bIsScoped", is_scoped, bool)
	netvar("DT_CSPlayer", "m_flFlashDuration", flash_duration, float)
	netvar("DT_CSPlayer", "m_flFlashMaxAlpha", flash_alpha, float)
	netvar("DT_CSPlayer", "m_bHasNightVision", m_bHasNightVision, float)
	netvar("DT_CSPlayer", "m_bNightVisionOn", m_bNightVisionOn, float)
	netvar("DT_CSPlayer", "m_iHealth", health, int)
	netvar("DT_CSPlayer", "m_lifeState", life_state, int)
	netvar("DT_CSPlayer", "m_fFlags", flags, int)
	netvar("DT_CSPlayer", "m_nTickBase", get_tick_base, int)
	netvar("DT_CSPlayer", "m_flDuckAmount", duck_amount, float)
	netvar("DT_CSPlayer", "m_bHasHeavyArmor", has_heavy_armor, bool)
	netvar("DT_BasePlayer", "m_hViewModel[0]", view_model, int)
	netvar("DT_BasePlayer", "m_viewPunchAngle", punch_angle, vec3_t)
	netvar("DT_BasePlayer", "m_aimPunchAngle", aim_punch_angle, vec3_t)
	netvar("DT_BasePlayer", "m_vecVelocity[0]", velocity, vec3_t)
	netvar("DT_BasePlayer", "m_flMaxspeed", max_speed, float)
	netvar("DT_BaseEntity", "m_flShadowCastDistance", m_flFOVTime, float)
	netvar("DT_BasePlayer", "m_iObserverMode", observer_mode, int)
	netvar("DT_BasePlayer", "m_nHitboxSet", hitbox_set, int)
	netvar("DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin", smoke_grenade_tick_begin, int)

	offset(anim_state*, animstate, 0x3900)
	offset(float, spawn_time, 0xA360)

	virtual_method(vec3_t&, get_abs_origin(), 10, (this))
	virtual_method(bool, is_alive(), 155, (this))
	virtual_method(void, update_client_side_animations(), 223, (this))
	virtual_method(weapon_t*, get_active_weapon(), 267, (this))
	virtual_method(player_t*, get_observer_target(), 294, (this))

	vec3_t get_eye_pos() {
		vec3_t vec;
		virtual_method_handler::call<void, 284>(this, std::ref(vec));
		return vec;
	}

	vec3_t get_hitbox_position(int i) {
		if (const auto studio_model = interfaces::model_info->get_studio_model(this->get_model())) {
			if (const auto hitbox = studio_model->hitbox_set(0)->hitbox(i)) {
				matrix_t bone_matrix[MAXSTUDIOBONES];

				if (!setup_bones(bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f))
					return { };

				vec3_t min, max;

				math::transform_vector(hitbox->mins, bone_matrix[hitbox->bone], min);
				math::transform_vector(hitbox->maxs, bone_matrix[hitbox->bone], max);

				return (min + max) * .5f;
			}
		}

		return { };
	}

	bool is_enemy(player_t* target) {
		return this->team() != target->team();
	}
};

class inferno_t : public entity_t {
public:
	offset(float, spawn_time, 0x20)
};