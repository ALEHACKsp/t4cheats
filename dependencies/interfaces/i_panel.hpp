#pragma once

#include "../utilities/virtual_method.h"

class i_panel {
public:
	virtual_method(const char*, get_panel_name(int id), 36, (this, id))
};