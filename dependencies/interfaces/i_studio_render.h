#pragma once
#include "i_material_system.hpp"
#include <cstddef>
#include <string_view>

enum class override_type {
	normal = 0,
	build_shadows,
	depth_write,
	custom_material,
	ssao_depth_write
};

class i_studio_render {
	std::byte pad_0[0x250];
	i_material* material_override;
	std::byte pad_1[0xC];
	override_type override_type;
public:
	void forced_material_override(i_material* material, ::override_type type = override_type::normal, int index = -1) {
		using fn = void(__thiscall*)(void*, i_material*, ::override_type, int);
		return (*(fn**)this)[33](this, material, type, index);
	}

	bool is_force_material_override() {
		if (!material_override)
			return override_type == override_type::depth_write || override_type == override_type::ssao_depth_write;

		return std::string_view{ material_override->get_name() }._Starts_with("dev/glow");
	}
};
