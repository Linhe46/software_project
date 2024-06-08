#include "./eval_env.h"
#include "./builtins.h"
#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <ranges>

using namespace std::literals;

EvalEnv::EvalEnv():parent{nullptr}{//初始化求值器
    auto procs=procDict();
    for(auto pair:procs)//加载内置过程
        defineBinding(pair.first,pair.second);
}
EvalEnv::EvalEnv(const std::shared_ptr<EvalEnv>&parent,const std::unordered_map<std::string,ValuePtr>&inner_params):
parent(parent),symbolList(inner_params){}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    if(!expr) return {};//无剩余变量
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}
ValuePtr EvalEnv::apply(ValuePtr proc,std::vector<ValuePtr>args){
    if(proc->isLambda()){//lambda表达式
        auto lambda=static_cast<LambdaValue&>(*proc);
        return lambda.apply(args);
    }
    else if(proc->isProc()){//内置过程
        auto procVar=static_cast<BuiltinProcValue&>(*proc);
        return procVar(args);
    }
    else{
        throw LispError("Unimplemented in apply");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    //PAIR型，即作为列表处理
    if (expr->isPair()){
        auto temp=expr.get();
        PairValue* expr=static_cast<PairValue*>(temp);
        //特殊式
        if(auto name=expr->getCar()->asSymbol())
            if(SPECIAL_FORMS.find(name.value())!=SPECIAL_FORMS.end())//特殊形式
                return SPECIAL_FORMS.at(name.value())(expr->getCdr()->toVector(),*this);//at()返回特殊过程的函数指针
        //过程式
        ValuePtr proc=this->eval(expr->getCar());//求解过程
        ValuePtr right=expr->getCdr();
        std::vector<ValuePtr>args=evalList(std::move(right));//剩余的变量作为过程的参数
        return this->apply(proc,args);
    }
    //可自求值
    else if (expr->isSelfEvaluating())
        return expr;
    else if(auto name=expr->asSymbol())
        return this->lookupBinding(name.value());
    //禁止对空表求值
    else if (expr->isNil())
        throw LispError("Evaluating nil is prohibited.");
    else
        throw LispError("Unimplemented in eval");
    return std::make_shared<NilValue>();
}

void EvalEnv::defineBinding(std::string name,ValuePtr ptr){
    symbolList[name]=ptr;
}

ValuePtr EvalEnv::lookupBinding(std::string name){
    auto it=symbolList.find(name);
    if(it!=symbolList.end())
        return it->second;
    else if(parent)//向上级环境查找
        return parent->lookupBinding(std::move(name));
    else
        throw LispError("Variable "+name+" not defined.");
}
EvalEnvPtr EvalEnv::createGlobal(){
    return EvalEnvPtr(new EvalEnv());
}

EvalEnvPtr EvalEnv::createChild(const std::vector<std::string>&params,
const std::vector<ValuePtr>& args){
    std::unordered_map<std::string,ValuePtr>list;
    if(params.size()!=args.size())
        throw LispError("params and args not match");
    for(int i=0;i<params.size();i++)
        list.insert({params[i],args[i]});
    return EvalEnvPtr(new EvalEnv(this->shared_from_this(),std::move(list)));
}

ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args){
    auto cur_env=env->createChild(params,args);
    ValuePtr res=nullptr;
    for(const auto&proc:body)
        res=cur_env->eval(proc);
    return res;
}

