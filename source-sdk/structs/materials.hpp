#pragma once

#include "../../dependencies/utilities/virtual_method.h"

enum material_var_flags_t {
	material_var_ignorez = 1 << 15
};

class i_material {
public:
	virtual_method(const char*, get_name(), 0, (this))
	virtual_method(void, alpha_modulate(float alpha), 27, (this, alpha))
	virtual_method(void, color_modulate(float r, float g, float b), 28, (this, r, g, b))
	virtual_method(void, set_material_var_flag(material_var_flags_t flag, bool on), 29, (this, flag, on))
};