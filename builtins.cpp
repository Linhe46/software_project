#include "./builtins.h"
#include <iostream>
#include <cmath>
std::unordered_map<std::string,ValuePtr> procDict::dict={};
ValuePtr procDict::operator[](const std::string& name){
    return dict[name];
}
std::unordered_map<std::string, ValuePtr>::iterator procDict::begin(){
    return dict.begin();
}
std::unordered_map<std::string, ValuePtr>::iterator procDict::end(){
    return dict.end();
}
void procDict::insert(const std::string& name, BuiltinFuncType* proc){
    if(dict.find(name)!=dict.end())
        throw LispError("Redefining a built-in procedure");
    dict[name]=std::make_shared<BuiltinProcValue>(proc);
}
void procDict::loadProcs(){
    insert("+", &add); insert("-", &sub); insert("*", &mult); insert("/", &divi);
    insert("abs", &abs_); insert("expt", &expt); insert("quotient", &quotient); insert("modulo", &modulo); insert("remainder", &remainder_);

    insert("eq?", &eq);
    insert("equal?", &equal);
    insert("not", &negation);
    insert(">", &greater);
    insert("<", &less);
    insert(">=", &greater_equal);
    insert("<=", &less_equal);
    insert("=", &numeric_equal);
    insert("even?", &even);
    insert("odd?", &odd);
    insert("zero?", &zero);
    
    insert("atom?", &is_atom);
    insert("boolean?", &is_boolean);
    insert("integer?", &is_integer);
    insert("list?", &is_list);
    insert("number?", &is_number);
    insert("null?", &is_null);
    insert("pair?", &is_pair);
    insert("procedure?", &is_procedure);
    insert("string?", &is_string);
    insert("symbol?", &is_symbol);

    insert("print", &print);
}

/*std::unordered_map<std::string,ValuePtr>procDict(){
    std::unordered_map<std::string,ValuePtr>procs;
    procs["+"]=std::make_shared<BuiltinProcValue>(&add);
    procs["-"]=std::make_shared<BuiltinProcValue>(&sub);
    procs["*"]=std::make_shared<BuiltinProcValue>(&mult);
    procs["/"]=std::make_shared<BuiltinProcValue>(&divi);

    procs["abs"]=std::make_shared<BuiltinProcValue>(&abs_);
    procs["expt"]=std::make_shared<BuiltinProcValue>(&expt);
    procs["quotient"]=std::make_shared<BuiltinProcValue>(&quotient);
    procs["modulo"]=std::make_shared<BuiltinProcValue>(&modulo);
    procs["remainder"]=std::make_shared<BuiltinProcValue>(&remainder_);

    procs["eq?"]=std::make_shared<BuiltinProcValue>(&eq);
    procs["equal?"]=std::make_shared<BuiltinProcValue>(&equal);
    procs["not"]=std::make_shared<BuiltinProcValue>(&negation);*/
/*procs[">"]=std::make_shared<BuiltinProcValue>(&greater);
    procs["<"]=std::make_shared<BuiltinProcValue>(&less);
    procs[">="]=std::make_shared<BuiltinProcValue>(&greater_equal);
    procs["<="]=std::make_shared<BuiltinProcValue>(&less_equal);
    procs["="]=std::make_shared<BuiltinProcValue>(&numeric_equal);
    procs["even?"]=std::make_shared<BuiltinProcValue>(&even);
    procs["odd?"]=std::make_shared<BuiltinProcValue>(&odd);
    procs["zero?"]=std::make_shared<BuiltinProcValue>(&zero);*/
/*procs["atom?"]=std::make_shared<BuiltinProcValue>(&is_atom);
    procs["boolean?"]=std::make_shared<BuiltinProcValue>(&is_boolean);
    procs["integer?"]=std::make_shared<BuiltinProcValue>(&is_integer);
    procs["list?"]=std::make_shared<BuiltinProcValue>(&is_list);
    procs["number?"]=std::make_shared<BuiltinProcValue>(&is_number);
    procs["null?"]=std::make_shared<BuiltinProcValue>(&is_null);
    procs["pair?"]=std::make_shared<BuiltinProcValue>(&is_pair);
    procs["procedure?"]=std::make_shared<BuiltinProcValue>(&is_procedure);
    procs["string?"]=std::make_shared<BuiltinProcValue>(&is_string);
    procs["symbol?"]=std::make_shared<BuiltinProcValue>(&is_symbol);*/
/*return procs;
}*/

//多参数过程
ValuePtr arithmetic(const std::vector<ValuePtr>& params,arithmeticType func,double init,int least_params){
    if(params.size()<least_params)
        throw LispError("At least "+ std::to_string(least_params)+ " param required");
    else{
        double res=0;
        for(int i=0;i<params.size();i++){
            if(!params[i]->isNumber())
                throw("Cannot calculate a non-numeric value");
            if(i==0){
                auto head=params[i]->asNumber();
                res=(params.size()==1?(func(init,head)):head);
            }
            else
                res=func(res,params[i]->asNumber());
        }
        return std::make_shared<NumericValue>(res);    
    }
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
        if(!left->isNumber()||!right->isNumber())
            throw LispError("Cannot compare non numeric values");
        if(!func(left->asNumber(),right->asNumber()))
            return std::make_shared<BooleanValue>(false);
        pos++;
    }
    return std::make_shared<BooleanValue>(true);
}
ValuePtr typeCheck(const std::vector<ValuePtr>& params, typeCheckType func, const std::string& name){
    if(params.size()!=1)
        throw LispError(name+": Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(func(arg) ? true : false);
}


ValuePtr add(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a+b;},0,0);
}
ValuePtr sub(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a-b;},0,1);
}
ValuePtr mult(const std::vector<ValuePtr>& params){
    return arithmetic(params,[](double a,double b){return a*b;},1,0);
}
ValuePtr divi(const std::vector<ValuePtr>& params){//默认参数问题
    return arithmetic(params,[](double a,double b){
        if(b==0)
            throw LispError("Divided by Zero!");
        return a/b;},1,1);
}
ValuePtr abs_(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("abs: arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber())
        throw LispError("Cannot compare non numeric values");
    else 
        return std::make_shared<NumericValue>(std::abs(params[0]->asNumber()));
}
ValuePtr expt(const std::vector<ValuePtr>& params){
    if(params.size()!=2)
        throw LispError("expt: arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot compare non numeric values");
    else 
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(x<=0)
                throw LispError("expt: a positive base is expected");
            return std::make_shared<NumericValue>(std::pow(x,y));
        }
}
ValuePtr quotient(const std::vector<ValuePtr>& params){
    if(params.size()!=2)
        throw LispError("quotient: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot compare non numeric values");
    else
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(y==0)
                throw LispError("quotient: Divided by zero");
            double res=x/y;
            return std::make_shared<NumericValue>(res>0 ? std::floor(res) : std::ceil(res));
        }
}
ValuePtr modulo(const std::vector<ValuePtr>& params){
    if(params.size()!=2)
        throw LispError("modulo: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot compare non numeric values");
    else
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(std::floor(x)!=x||std::floor(y)!=y)
                throw LispError("modulo: Cannot apply on non-integars");
            if(y==0)
                throw LispError("modulo: Cannot apply a zero modulo");
            while(x*y<0)
                x+=y;
            while(std::abs(x)>=std::abs(y))
                x-=y;
            return std::make_shared<NumericValue>(x);
        }
}
ValuePtr remainder_(const std::vector<ValuePtr>& params){
    if(params.size()!=2)
        throw LispError("remainder: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot compare non numeric values");
    else
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(y==0)
                throw LispError("remainder: Cannot apply a zero remainder");
            return std::make_shared<NumericValue>(x-y*(x/y > 0 ? std::floor(x/y) : std::ceil(x/y)));
        }
}
ValuePtr eq(const std::vector<ValuePtr>& params){
    if(params.size()!=2)
        throw LispError("eq?: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else{
        auto arg1=params[0],arg2=params[1];
        if(arg1->isSymbol()&&arg2->isSymbol())
            return std::make_shared<BooleanValue>(arg1->asSymbol()==arg2->asSymbol());
        else if(arg1->isNumber()&&arg2->isNumber())
            return std::make_shared<BooleanValue>(arg1->asNumber()==arg2->asNumber());
        else if(arg1->isBoolean()&&arg2->isBoolean())
            return std::make_shared<BooleanValue>(arg1->toString()==arg2->toString());
        else if(arg1->isNil()&&arg2->isNil())
            return std::make_shared<BooleanValue>(true);
        else if(arg1->isPair()&&arg2->isPair()||arg1->isProc()&&arg2->isProc()||
        arg1->isString()&&arg2->isString()||arg1->isLambda()&&arg2->isLambda())
            return std::make_shared<BooleanValue>(arg1==arg2);
        else 
            return std::make_shared<BooleanValue>(false);
    }
}
//语义相等
ValuePtr equal(const std::vector<ValuePtr>& params){
    if(params.size()!=2)
        throw LispError("eq?: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else{
        auto arg1=params[0],arg2=params[1];
        if(typeid(arg1)==typeid(arg2)&&arg1->toString()==arg2->toString())
            return std::make_shared<BooleanValue>(true);
        else return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr negation(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("not: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(arg->toString()=="#f" ? true : false);
}

ValuePtr less(const std::vector<ValuePtr>& params){
    return numericCompare(params,[](double a,double b){return a<b;});
}
ValuePtr less_equal(const std::vector<ValuePtr>& params){
    return numericCompare(params,[](double a,double b){return a<=b;});
}
ValuePtr greater_equal(const std::vector<ValuePtr>& params){
    return numericCompare(params,[](double a,double b){return a>=b;});
}
ValuePtr greater(const std::vector<ValuePtr>& params){
    return numericCompare(params,[](double a,double b){return a>b;});
}
ValuePtr numeric_equal(const std::vector<ValuePtr>& params){
    return numericCompare(params,[](double a,double b){return a==b;});
}
ValuePtr even(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("even?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else{
        auto arg=params[0];
        if(arg->isNumber()){
            double x=arg->asNumber();
            if(!isIntegar(x))
                throw LispError("even?: Cannot apply to non-integars");
            return std::make_shared<BooleanValue>((int)x%2==0 ? true : false);
        }
        else 
            throw LispError("even?: Cannot apply to non-integars");
    }
}
ValuePtr odd(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("odd?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else{
        auto arg=params[0];
        if(arg->isNumber()){
            double x=arg->asNumber();
            if(!isIntegar(x))
                throw LispError("odd?: Cannot apply to non-integars");
            return std::make_shared<BooleanValue>((int)x%2==1 ? true : false);
        }
        else 
            throw LispError("odd?: Cannot apply to non-integars");
    }
}
ValuePtr zero(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("zero?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else{
        auto arg=params[0];
        if(arg->isNumber()){
            double x=arg->asNumber();
            return std::make_shared<BooleanValue>(x==0 ? true : false);
        }
        else 
            throw LispError("zero?: Cannot apply to non-integars");
    }
}
ValuePtr print(const std::vector<ValuePtr>& params){
    for(auto& i:params)
        std::cout<<i->toString()<<' ';
    std::cout<<"\n";
    return std::make_shared<NilValue>();
}

ValuePtr is_atom(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("atom?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(arg->isSelfEvaluating()||arg->isSymbol()||arg->isNil() ? true : false);
}
ValuePtr is_boolean(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("boolean?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(arg->isBoolean() ? true : false);
}
ValuePtr is_integer(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("integer?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(arg->isNumber()&&arg->asNumber()==std::trunc(arg->asNumber())? true : false);
}
ValuePtr is_list(const std::vector<ValuePtr>& params){
    if(params.size()!=1)
        throw LispError("list?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(arg->isList() ? true : false);
}
ValuePtr is_number(const std::vector<ValuePtr>& params){
    return typeCheck(params, [](ValuePtr arg){return arg->isNumber();}, "number?");
}
ValuePtr is_null(const std::vector<ValuePtr>& params){
    return typeCheck(params, [](ValuePtr arg){return arg->isNil();}, "null?");
}
ValuePtr is_pair(const std::vector<ValuePtr>& params){
    return typeCheck(params, [](ValuePtr arg){return arg->isPair();}, "pair?");
}
ValuePtr is_procedure(const std::vector<ValuePtr>& params){
    return typeCheck(params, [](ValuePtr arg){return arg->isProc()||arg->isLambda();}, "procedure?");
}
ValuePtr is_string(const std::vector<ValuePtr>& params){
    return typeCheck(params, [](ValuePtr arg){return arg->isString();}, "string?");
}
ValuePtr is_symbol(const std::vector<ValuePtr>& params){
    return typeCheck(params, [](ValuePtr arg){return arg->isSymbol();}, "symbol?");
}



bool isIntegar(double x){
    return std::floor(x)==x;
}