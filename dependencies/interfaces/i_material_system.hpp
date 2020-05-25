#pragma once

#include "../utilities/virtual_method.h"

class i_material;

class i_material_system {
public:
	virtual_method(i_material*, find_material(const char* material_name, const char* texture_group_name = nullptr), 84, (this, material_name, texture_group_name, true, nullptr))
};