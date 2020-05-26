#pragma once

class key_values {
public:
    key_values* find_key(const char* key_name, bool create);
    static key_values* from_string(const char* name, const char* value);
    void set_string(const char* key_name, const char* value);
};