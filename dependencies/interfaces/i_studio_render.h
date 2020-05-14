#pragma once
#include "i_material_system.hpp"
#include <cstddef>
#include <string_view>

enum class OverrideType {
	Normal = 0,
	BuildShadows,
	DepthWrite,
	CustomMaterial,
	SsaoDepthWrite
};

class i_studio_render {
	std::byte pad_0[0x250];
	i_material* material_override;
	std::byte pad_1[0xC];
	OverrideType override_type;
public:
	bool is_force_material_override() noexcept {
		if (!material_override)
			return override_type == OverrideType::DepthWrite || override_type == OverrideType::SsaoDepthWrite;
		return std::string_view{ material_override->get_name() }._Starts_with("dev/glow");
	}
};
