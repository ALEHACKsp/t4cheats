#pragma once

#include "../../dependencies/utilities/virtual_method.h"

#include <vector>

template <typename T>
class utl_vector {
public:
	T& operator[](int i) { return memory[i]; };

	T* memory;
	int allocation_count;
	int grow_size;
	int size;
	T* elements;
};

class convar {
public:
	virtual_method(float, get_float(), 12, (this))
	virtual_method(int, get_int(), 13, (this))
	virtual_method(void, set_value(const char* value), 14, (this, value))
	virtual_method(void, set_value(float value), 15, (this, value))
	virtual_method(void, set_value(int value), 16, (this, value))

	std::byte pad[24];
	std::add_pointer_t<void __cdecl()> change_callback;
	convar* parent;
	const char* default_value;
	char* string;
	std::byte pad1[28];
	utl_vector<void __cdecl()> on_change_callbacks;
};