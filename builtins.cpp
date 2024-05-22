#include "./builtins.h"
#include <iostream>
ValuePtr addVal=std::make_shared<BuiltinProcValue>(&add);
ValuePtr printVal=std::make_shared<BuiltinProcValue>(&print);
std::unordered_map<std::string,ValuePtr>procDict(){
    std::unordered_map<std::string,ValuePtr>procs;
    procs["+"]=addVal;
    procs["print"]=printVal;
    return procs;
}

ValuePtr add(const std::vector<ValuePtr>& params){
    double result=0;
    for(const auto&i:params){
        if(!i->isNumber()){
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr print(const std::vector<ValuePtr>& params){
    for(auto& i:params)
        std::cout<<i->toString()<<' ';
    std::cout<<"\n";
    return std::make_shared<NilValue>();
}