#pragma once

#include "../utilities/virtual_method.h"

class convar;

class i_cvar {
public:
	virtual_method(convar*, get_convar(const char* name), 15, (this, name))
};