#pragma once

#include "../utilities/virtual_method.h"

class entity_t;

class i_client_entity_list {
public:
	virtual_method(entity_t*, get_client_entity(int index), 3, (this, index))
	virtual_method(entity_t*, get_client_entity_handle(int handle), 4, (this, handle))
	virtual_method(int, get_highest_index(), 6, (this))
};