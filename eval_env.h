#ifndef EVALENV_H
#define EVALENV_H
#include "./error.h"
#include "./value.h"
#include <unordered_map>
extern std::unordered_map<std::string,ValuePtr>symbolList;//符号表
class EvalEnv {
public:
    ValuePtr eval(ValuePtr expr);
};
#endif