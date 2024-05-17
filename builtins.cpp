#include "./builtins.h"
ValuePtr addVal=std::make_shared<BuiltinProcValue>(&add);
std::unordered_map<std::string,ValuePtr>procDict(){
    std::unordered_map<std::string,ValuePtr>procs;
    procs["+"]=addVal;
    return procs;
}
ValuePtr add(const std::vector<ValuePtr>& params){
    double result=0;
    for(const auto&i:params){
        if(!i->isSelfEvaluating(ValueType::NUMERIC)){
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
