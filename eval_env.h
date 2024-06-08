#ifndef EVALENV_H
#define EVALENV_H

#include "./error.h"
#include "./value.h"
#include <unordered_map>

class EvalEnv;
using EvalEnvPtr=std::shared_ptr<EvalEnv>;

class EvalEnv:public std::enable_shared_from_this<EvalEnv> {
private:
    std::shared_ptr<EvalEnv>parent;//上级环境
    std::unordered_map<std::string,ValuePtr>symbolList;//符号表

    std::vector<ValuePtr>evalList(ValuePtr expr);
    EvalEnv();//创建全局环境
    EvalEnv(const std::shared_ptr<EvalEnv>&parent,const std::unordered_map<std::string,ValuePtr>&inner_params);//由参数表创建环境
public:
    ValuePtr eval(ValuePtr expr);
    static ValuePtr apply(ValuePtr proc,std::vector<ValuePtr>args,EvalEnv& env);
    ValuePtr lookupBinding(std::string name);
    void defineBinding(std::string,ValuePtr);
    static EvalEnvPtr createGlobal();
    EvalEnvPtr createChild(const std::vector<std::string>& params,const std::vector<ValuePtr>& args);
};
#endif