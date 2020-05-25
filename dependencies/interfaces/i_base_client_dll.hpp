#pragma once

#include "../utilities/virtual_method.h"

struct c_client_class;

struct base_client_dll {
	virtual_method(c_client_class*, get_client_classes(), 8, (this))
};