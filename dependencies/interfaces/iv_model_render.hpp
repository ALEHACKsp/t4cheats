#pragma once
#include "../../source-sdk/structs/materials.hpp"

class iv_model_render {
public:
	void override_material(i_material* material) {
		using original_fn = void(__thiscall*)(iv_model_render*, i_material*, int, int);
		return (*(original_fn * *)this)[1](this, material, 0, 0);
	}
	bool is_forced() {
		using original_fn = bool(__thiscall*)(void*);
		return (*(original_fn * *)this)[2](this);
	}
};