#include "./eval_env.h"
#include "./builtins.h"
#include <algorithm>
#include <iterator>
#include <ranges>

std::unordered_map<std::string,ValuePtr>symbolList;
EvalEnv::EvalEnv(){
    auto procs=procDict();
    for(auto pair:procs)
        symbolList.insert({pair.first,pair.second});
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
    if(proc->isSelfEvaluating(ValueType::PROC)){
        auto procVar=static_cast<BuiltinProcValue&>(*proc);
        return procVar(args);
    }
    else{
        throw LispError("Unimplemented");
    }
}
ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating(ValueType::PAIR)){//PAIR型，即作为列表处理
        using namespace std::literals;
        std::vector<ValuePtr>v=expr->toVector();
        if(v[0]->asSymbol()=="define"s){//实现define name value
            if(auto name=v[1]->asSymbol()){
                if(v.size()<3) 
                    throw LispError("Malformed define.");//未定义值
                symbolList[name.value()]=eval(v[2]);
                return std::make_shared<NilValue>();
            }
            else throw LispError("Malformed define.");//定义错误
        }
        //是列表且不是特殊形式
        else{
            auto pairVar=static_cast<PairValue&>(*expr);
            ValuePtr proc=this->eval(std::move(v[0]));
            ValuePtr right=pairVar.getRight();
            std::vector<ValuePtr>args=evalList(std::move(right));//剩余的变量
            return this->apply(proc,args);
        }
        //else throw LispError("Unimplemented");
    }
    else if (//直接返回值型变量
        expr->isSelfEvaluating(ValueType::BOOLEAN) ||
        expr->isSelfEvaluating(ValueType::NUMERIC) ||
        expr->isSelfEvaluating(ValueType::STRING))
        return expr;
    else if(expr->isSelfEvaluating(ValueType::SYMBOL)){//对于符号，获取其值
        if(auto name=expr->asSymbol()){
            auto it=symbolList.find(name.value());
            if(it!=symbolList.end())
                return it->second;
            else throw LispError("Variable "+name.value()+" not defined.");
        }
    }
    else if (expr->isSelfEvaluating(ValueType::NIL))
        throw LispError("Evaluating nil is prohibited.");
    else
        throw LispError("Unimplemented");
    return std::make_shared<NilValue>();
}