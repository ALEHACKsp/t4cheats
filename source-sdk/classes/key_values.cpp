#include <cstdint>

#include "key_values.h"
#include "../../dependencies/utilities/utilities.hpp"

template <typename T>
static auto relative_to_absolute(int* address) {
    return reinterpret_cast<T>(reinterpret_cast<char*>(address + 1) + *address);
}

key_values* key_values::find_key(const char* key_name, bool create) {
    static const auto key_values_find_key = relative_to_absolute<key_values*(__thiscall*)(key_values*, const char*, bool)>(reinterpret_cast<int*>("client_panorama.dll", "E8 ? ? ? ? F7 45", 1));
    return key_values_find_key(this, key_name, create);
}

key_values* key_values::from_string(const char* name, const char* value) {
    static const std::uintptr_t key_values_from_string = relative_to_absolute<std::uintptr_t>(reinterpret_cast<int*>(utilities::pattern_scan("client_panorama.dll", "E8 ? ? ? ? 83 C4 04 89 45 D8") + 1));
    key_values* key_values;

    __asm {
        push 0
        mov edx, value
        mov ecx, name
        call key_values_from_string
        add esp, 4
        mov key_values, eax
    }

    return key_values;
}

void key_values::set_string(const char* key_name, const char* value) {
    static const auto key_values_set_string = relative_to_absolute<void(__thiscall*)(key_values*, const char*)>(reinterpret_cast<int*>("client_panorama.dll", "E8 ? ? ? ? 89 77 38", 1));

    if (const auto key = find_key(key_name, true))
        key_values_set_string(key, value);
}