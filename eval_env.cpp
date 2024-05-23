#include "./eval_env.h"
#include "./builtins.h"
#include "./forms.h"
#include <algorithm>
#include <iterator>
#include <ranges>

using namespace std::literals;

std::unordered_map<std::string,ValuePtr>EvalEnv::symbolList;
EvalEnv::EvalEnv(){//初始化求值器
    symbolList.clear();
    auto procs=procDict();
    for(auto pair:procs)//加载内置过程
        addToSymbol(pair.first,pair.second);
}
std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    if(!expr) return {};//无剩余变量
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}
ValuePtr EvalEnv::apply(ValuePtr proc,std::vector<ValuePtr>args){
    if(proc->isProc()){
        auto procVar=static_cast<BuiltinProcValue&>(*proc);
        return procVar(args);
    }
    else{
        throw LispError("Unimplemented");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isPair()){//PAIR型，即作为列表处理
        auto temp=expr.get();
        PairValue* expr=static_cast<PairValue*>(std::move(temp));
        if(auto name=expr->getCar()->asSymbol()){//实现define name value
            if(SPECIAL_FORMS.find(name.value())!=SPECIAL_FORMS.end())
                return SPECIAL_FORMS.at(name.value())(expr->getCdr()->toVector(),*this);//可能不存在，不能使用[]
            else throw LispError("Proc not Defined");
        }
        //是列表且不是特殊形式
        else{
            ValuePtr proc=this->eval(expr->getCar());
            ValuePtr right=expr->getCdr();
            std::vector<ValuePtr>args=evalList(std::move(right));//剩余的变量
            return this->apply(proc,args);
        }
        //else throw LispError("Unimplemented");
    }
    else if (expr->isSelfEvaluating())//可自求值
        return expr;
    else if(expr->isSymbol()){
        if(auto name=expr->asSymbol()){
            auto it=symbolList.find(name.value());
            if(it!=symbolList.end())
                return it->second;
            else throw LispError("Variable "+name.value()+" not defined.");
        }
    }
    else if (expr->isNil())//禁止对空表求值
        throw LispError("Evaluating nil is prohibited.");
    else
        throw LispError("Unimplemented");
    return std::make_shared<NilValue>();
}

void EvalEnv::addToSymbol(std::string name,ValuePtr ptr){
    EvalEnv::symbolList.insert({name,std::move(ptr)});
}