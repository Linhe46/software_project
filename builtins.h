#ifndef BUILTINS_H
#define BUILTINS_H

#include "./value.h"
#include <unordered_map>
std::unordered_map<std::string,ValuePtr>procDict();
ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);
#endif