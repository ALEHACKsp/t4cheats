#pragma once

#include <cstddef>

namespace virtual_method_handler
{
    template <typename T, std::size_t index, typename ..._args>
    auto call(void* base, _args... args)
    {
        return ((*reinterpret_cast<T(__thiscall***)(void*, _args...)>(base))[index])(base, args...);
    }
}

#define virtual_method(return_type, name, idx, args) \
auto name \
{ \
    return virtual_method_handler::call<return_type, idx>##args; \
}