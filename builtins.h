#ifndef BUILTINS_H
#define BUILTINS_H

#include "./value.h"
#include <unordered_map>
std::unordered_map<std::string,ValuePtr>procDict();

using arithmeticType=double(*)(double,double);
using numericCompareType=bool(*)(double,double);
ValuePtr arithmetic(const std::vector<ValuePtr>& params,arithmeticType func,double init);
ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr sub(const std::vector<ValuePtr>& params);
ValuePtr mult(const std::vector<ValuePtr>& params);
ValuePtr divi(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);
ValuePtr bigger(const std::vector<ValuePtr>& params);
#endif