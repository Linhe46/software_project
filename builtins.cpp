#include "./builtins.h"
#include <iostream>
ValuePtr addVal=std::make_shared<BuiltinProcValue>(&add);
ValuePtr printVal=std::make_shared<BuiltinProcValue>(&print);
ValuePtr subVal=std::make_shared<BuiltinProcValue>(&sub);
ValuePtr multVal=std::make_shared<BuiltinProcValue>(&mult);
ValuePtr divVal=std::make_shared<BuiltinProcValue>(&divi);
ValuePtr biggerVal=std::make_shared<BuiltinProcValue>(&bigger);
std::unordered_map<std::string,ValuePtr>procDict(){
    std::unordered_map<std::string,ValuePtr>procs;
    procs["+"]=addVal;
    procs["print"]=printVal;
    procs["-"]=subVal;
    procs["*"]=multVal;
    procs["/"]=divVal;
    procs[">"]=biggerVal;
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
ValuePtr numericCompare(const std::vector<ValuePtr>& params, numericCompareType func){
    if(params.size()==0)
        throw LispError("At least one param required");
    if(params.size()==1&&params[0]->isNumber())
        return std::make_shared<BooleanValue>(true);
    int pos=0;
    while(pos<params.size()-1){
        auto left=params[pos];
        auto right=params[pos+1];
        if(!left->isNumber()||!left->isNumber())
            throw LispError("Cannot compare non numeric values");
        if(!func(left->asNumber(),right->asNumber()))
            return std::make_shared<BooleanValue>(false);
        pos++;
    }
    return std::make_shared<BooleanValue>(true);
}
ValuePtr add(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a+b;},0);
}
ValuePtr sub(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a-b;},0);
}
ValuePtr mult(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a*b;},1);
}
ValuePtr divi(const std::vector<ValuePtr>& params){//默认参数问题
    return arithmetic(params,[](double a,double b){
        if(b==0)
            throw LispError("Divided by Zero!");
        return a/b;},1);
}
ValuePtr bigger(const std::vector<ValuePtr>& params){
    return numericCompare(params,[](double a,double b){return a>b;});
}
ValuePtr print(const std::vector<ValuePtr>& params){
    for(auto& i:params)
        std::cout<<i->toString()<<' ';
    std::cout<<"\n";
    return std::make_shared<NilValue>();
}