#include "./value.h"

#include <iomanip>
#include <sstream>

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

std::ostream& operator<<(std::ostream& os, const Value& value){
    return os<<value.toString();
}