#include "./eval_env.h"

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating(ValueType::PAIR)){//PAIR型，即作为列表处理
        using namespace std::literals;
        std::vector<ValuePtr>v=expr->toVector();
        if(v[0]->asSymbol()=="define"s){//实现define name value
            if(auto name=v[1]->asSymbol()){
                symbolList[name.value()]=eval(v[2]);
                return std::make_shared<NilValue>();
            }
        else throw LispError("Malformed define.");//定义错误
        }
        else throw LispError("Unimplemented");
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
        }else throw LispError("Variable "+name.value()+" not defined.");
    }
    else if (expr->isSelfEvaluating(ValueType::NIL))
        throw LispError("Evaluating nil is prohibited.");
    else
        throw LispError("Unimplemented");
    return std::make_shared<NilValue>();
}