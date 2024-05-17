#include "./value.h"

#include <iomanip>
#include <sstream>
bool Value::isSelfEvaluating(ValueType type) {
    return this->type == type;
}
std::vector<ValuePtr>Value::toVector(){//非列表抛出异常
    throw LispError("Not a list");
}
std::optional<std::string> Value::asSymbol() const{//获取符号名
    if(this->type!=ValueType::SYMBOL)
        return std::nullopt;
    return this->toString();
}
double Value::asNumber() const{
        throw LispError("Not a Numeric type");
}
double NumericValue::asNumber()const{
    return value;
}

std::string BooleanValue::toString() const{
    return (value? "#t" : "#f");
}
std::string NumericValue::toString() const{
    std::string str;
    str=std::to_string(value);  
    while(str.back()=='0')//不断移除末尾0
        str.pop_back();
    if(str.back()=='.')//移除末尾小数点
        str.pop_back();
    return str;
}
std::string StringValue::toString() const{
    std::ostringstream ss;
    ss<<std::quoted(value);
    return ss.str();
}
std::string NilValue::toString() const{
    return "()";
}
std::string SymbolValue::toString() const{
    return name;
}
std::string PairValue::toString() const{
    if(typeid(*second)==typeid(PairValue)){//右半部分为对子
        std::string str_pair=second->toString();//右半部分递归展开
        str_pair.erase(str_pair.begin());//删除子列表左括号
        return "("+first->toString()+" "+str_pair;
        }
    else if(typeid(*second)==typeid(NilValue))//右边部分为空表，右括号闭合
        return "("+first->toString()+")";
    return "("+first->toString()+" . "+second->toString()+")";//右半部分不再是列表，点分隔，右括号闭合
}

std::vector<ValuePtr>PairValue::toVector(){//递归地转换为数组

    std::vector<ValuePtr>values;
    auto& pair=static_cast<PairValue&>(*this);
    if(pair.first->getType()!=ValueType::NIL)
        values.push_back(pair.first);
    if(pair.second->isSelfEvaluating(ValueType::NIL))
        return values;
    else if(pair.second->isSelfEvaluating(ValueType::PAIR))
    {//递归展开右边部分
        auto child_vec=pair.second->toVector();
        std::copy(child_vec.begin(),child_vec.end(),std::back_inserter(values));
    }
    else values.push_back(pair.second);
    return values;
}
std::ostream& operator<<(std::ostream& os, const Value& value){
    return os<<value.toString();
}

std::string BuiltinProcValue::toString()const{
    return std::string("#<procedure>");
}
ValuePtr BuiltinProcValue::operator()(const std::vector<ValuePtr>&params){
    return func(params);
}
ValuePtr PairValue::getRight()const{
    return this->second->isSelfEvaluating(ValueType::NIL)?nullptr:this->second;
}