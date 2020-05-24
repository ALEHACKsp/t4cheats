#pragma once

#include <cstddef>
#include <memory>

#include "../../../source-sdk/classes/recv_props.hpp"
#include "../../utilities/fnv.hpp"

namespace netvar_manager {
    std::uintptr_t get_net_var(std::uint32_t table, std::uint32_t prop);
}

#define netvar_offset(table, prop, func_name, type, offset) \
std::add_lvalue_reference_t<type> func_name() { \
    static const std::uintptr_t hash = netvar_manager::get_net_var(fnv::hash(table), fnv::hash(prop)); \
    return *reinterpret_cast<std::add_pointer_t<type>>(this + hash + offset); \
}

#define netvar(table, prop, func_name, type) \
	netvar_offset(table, prop, func_name, type, 0)

#define netvar_ptr_offset(table, prop, func_name, type, offset) \
auto func_name() { \
    static const std::uintptr_t hash = netvar_manager::get_net_var(fnv::hash(table), fnv::hash(prop)); \
    return reinterpret_cast<std::add_pointer_t<type>>(this + hash + offset); \
}

#define netvar_ptr(table, prop, func_name, type) \
	netvar_ptr_offset(table, prop, func_name, type, 0)

#define offset(type, name, offset) \
std::add_lvalue_reference_t<type> name() { \
    return *reinterpret_cast<std::add_pointer_t<type>>(this + offset); \
}