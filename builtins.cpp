#include "./builtins.h"
#include <iostream>
ValuePtr addVal=std::make_shared<BuiltinProcValue>(&add);
ValuePtr printVal=std::make_shared<BuiltinProcValue>(&print);
ValuePtr subVal=std::make_shared<BuiltinProcValue>(&sub);
ValuePtr multVal=std::make_shared<BuiltinProcValue>(&mult);
ValuePtr divVal=std::make_shared<BuiltinProcValue>(&divi);
std::unordered_map<std::string,ValuePtr>procDict(){
    std::unordered_map<std::string,ValuePtr>procs;
    procs["+"]=addVal;
    procs["print"]=printVal;
    procs["-"]=subVal;
    procs["*"]=multVal;
    procs["/"]=divVal;
    return procs;
}
ValuePtr arithmetic(const std::vector<ValuePtr>& params,arithmeticType func,double init){
    double result=init;
    for(const auto&i:params){
        if(!i->isNumber())
            throw LispError("Cannot calculate a non-numeric value.");
        result=func(result,i->asNumber());
    }
    return std::make_shared<NumericValue>(result);
};
ValuePtr add(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a+b;},0);
}
ValuePtr sub(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a-b;},0);
}
ValuePtr mult(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a*b;},1);
}
ValuePtr divi(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){
        if(b==0)
            throw LispError("Divided by Zero!");
        return a/b;},1);
}
ValuePtr print(const std::vector<ValuePtr>& params){
    for(auto& i:params)
        std::cout<<i->toString()<<' ';
    std::cout<<"\n";
    return std::make_shared<NilValue>();
}