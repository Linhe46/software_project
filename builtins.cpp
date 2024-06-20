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
    insert("+", &add);
    insert("-", &sub);
    insert("*", &mult);
    insert("/", &divi);
    insert("abs", &abs_);
    insert("expt", &expt);
    insert("quotient", &quotient);
    insert("modulo", &modulo);
    insert("remainder", &remainder_);

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
    insert("display", &display);
    insert("displayln", &displayln);
    insert("eval", &eval);
    insert("error", &error);
    insert("apply", &apply);
    insert("exit", &exit);
    insert("newline", &newline);

    insert("append", &append);
    insert("car", &car);
    insert("cdr", &cdr);
    insert("cons", &cons);
    insert("length", &length);
    insert("list", &list);
    insert("map", &map);
    insert("filter", &filter);
    insert("reduce", &reduce);
}

//算术库 比较库 类型检查库 模板
ValuePtr arithmetic(const std::vector<ValuePtr>& params,arithmeticType func,double init,int least_params){
    if(params.size()<least_params)
        throw LispError("At least "+ std::to_string(least_params)+ " param required");
    else{
        double res=init;
        for(int i=0;i<params.size();i++){
            if(!params[i]->isNumber())
                throw("Cannot calculate non-numeric values");
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
            throw LispError("Cannot compare non-numeric values");
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


ValuePtr add(const std::vector<ValuePtr>& params,EvalEnv& env){
    return arithmetic(params,[](double a,double b){return a+b;},0,0);
}
ValuePtr sub(const std::vector<ValuePtr>& params,EvalEnv& env){
    return arithmetic(params,[](double a,double b){return a-b;},0,1);
}
ValuePtr mult(const std::vector<ValuePtr>& params,EvalEnv& env){
    return arithmetic(params,[](double a,double b){return a*b;},1,0);
}
ValuePtr divi(const std::vector<ValuePtr>& params,EvalEnv& env){//默认参数问题
    return arithmetic(params,[](double a,double b){
        if(b==0)
            throw LispError("Divided by Zero!");
        return a/b;},1,1);
}
ValuePtr abs_(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("abs: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber())
        throw LispError("Cannot calculate non-numeric values");
    else 
        return std::make_shared<NumericValue>(std::abs(params[0]->asNumber()));
}
ValuePtr expt(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("expt: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot calculate non-numeric values");
    else 
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(x<=0)
                throw LispError("expt: a positive base is expected");
            return std::make_shared<NumericValue>(std::pow(x,y));
        }
}
ValuePtr quotient(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("quotient: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot calculate non-numeric values");
    else
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(y==0)
                throw LispError("quotient: Divided by zero");
            double res=x/y;
            return std::make_shared<NumericValue>(res>0 ? std::floor(res) : std::ceil(res));
        }
}
ValuePtr modulo(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("modulo: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot calculate non-numeric values");
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
ValuePtr remainder_(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("remainder: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else if(!params[0]->isNumber()||!params[1]->isNumber())
        throw LispError("Cannot calculate non-numeric values");
    else
        {
            double x=params[0]->asNumber(),y=params[1]->asNumber();
            if(y==0)
                throw LispError("remainder: Cannot apply a zero remainder");
            return std::make_shared<NumericValue>(x-y*(x/y > 0 ? std::floor(x/y) : std::ceil(x/y)));
        }
}

ValuePtr eq(const std::vector<ValuePtr>& params,EvalEnv& env){
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
ValuePtr equal(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("eq?: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    else{
        auto arg1=params[0],arg2=params[1];
        if(typeid(arg1)==typeid(arg2)&&arg1->toString()==arg2->toString())
            return std::make_shared<BooleanValue>(true);
        else return std::make_shared<BooleanValue>(false);
    }
}
ValuePtr negation(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("not: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    return std::make_shared<BooleanValue>(arg->toString()=="#f" ? true : false);
}
ValuePtr less(const std::vector<ValuePtr>& params,EvalEnv& env){
    return numericCompare(params,[](double a,double b){return a<b;});
}
ValuePtr less_equal(const std::vector<ValuePtr>& params,EvalEnv& env){
    return numericCompare(params,[](double a,double b){return a<=b;});
}
ValuePtr greater_equal(const std::vector<ValuePtr>& params,EvalEnv& env){
    return numericCompare(params,[](double a,double b){return a>=b;});
}
ValuePtr greater(const std::vector<ValuePtr>& params,EvalEnv& env){
    return numericCompare(params,[](double a,double b){return a>b;});
}
ValuePtr numeric_equal(const std::vector<ValuePtr>& params,EvalEnv& env){
    return numericCompare(params,[](double a,double b){return a==b;});
}
ValuePtr even(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("even?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else{
        auto arg=params[0];
        if(arg->isNumber()){
            double x=arg->asNumber();
            if(!isIntegar(x))
                throw LispError("even?: Contract violation\n expected: integer\n given: "+arg->toString());
            return std::make_shared<BooleanValue>((int)x%2==0 ? true : false);
        }
        else 
            throw LispError("even?: Contract violation\n expected: integer\n given: "+arg->toString());
    }
}
ValuePtr odd(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("odd?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else{
        auto arg=params[0];
        if(arg->isNumber()){
            double x=arg->asNumber();
            if(!isIntegar(x))
                throw LispError("odd?: Contract violation\n expected: integer\n given: "+arg->toString());
            return std::make_shared<BooleanValue>((int)x%2==0 ? false : true);//负数存在，注意不能用x%2==1
        }
        else 
            throw LispError("odd?: Contract violation\n expected: integer\n given: "+arg->toString());
    }
}
ValuePtr zero(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("zero?: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    else{
        auto arg=params[0];
        if(arg->isNumber()){
            double x=arg->asNumber();
            return std::make_shared<BooleanValue>(x==0 ? true : false);
        }
        else 
            throw LispError("zero?: Contract violation\n expected: numeric\n given: "+arg->toString());
    }
}

ValuePtr is_atom(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isSelfEvaluating()||arg->isSymbol()||arg->isNil();}, "atom?");
}
ValuePtr is_boolean(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isBoolean();}, "boolean?");
}
ValuePtr is_integer(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isNumber()&&arg->asNumber()==std::trunc(arg->asNumber());}, "integer?");
}
ValuePtr is_list(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isList();}, "list?");
}
ValuePtr is_number(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isNumber();}, "number?");
}
ValuePtr is_null(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isNil();}, "null?");
}
ValuePtr is_pair(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isPair();}, "pair?");
}
ValuePtr is_procedure(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isProc()||arg->isLambda();}, "procedure?");
}
ValuePtr is_string(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isString();}, "string?");
}
ValuePtr is_symbol(const std::vector<ValuePtr>& params,EvalEnv& env){
    return typeCheck(params, [](ValuePtr arg){return arg->isSymbol();}, "symbol?");
}

ValuePtr apply(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("apply: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    auto proc=params[0];
    auto list=params[1];
    if((!proc->isProc()&&!proc->isLambda())||!list->isList())
        throw LispError("apply: Contract violation:\n expected: (apply <proc> <list>)");
    return EvalEnv::apply(proc, list->toVector(), env);
}
ValuePtr display(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("display: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    std::string ss;
    if(arg->isString()){
        ss=arg->toString();
        if(ss.length()>=2 && ss.front()=='"' && ss.back()=='"')
            ss=ss.substr(1,ss.length()-2);
    }
    else 
        ss=arg->toString();
    std::cout<<ss;
    return std::make_shared<NilValue>();
}
ValuePtr displayln(const std::vector<ValuePtr>& params,EvalEnv& env){
    try{
        display(params,env);}
    catch (LispError& e){
        throw LispError("displayln: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    }
    return newline({},env);
}
ValuePtr error(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()==0)
        throw LispError("");
    else if(params.size()==1)
        throw LispError(params[0]->toString());
    else
         throw LispError("error: Arguments mismatch:\n expected:0 or 1\n given: "+std::to_string(params.size()));
}
ValuePtr eval(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("eval: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    return env.eval(params[0]);
}
ValuePtr exit(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()==0||params.size()==1){
        int code=0;
        if(params.size()==1){
            try{
                if(is_integer(params,env))
                    code=params[0]->asNumber();
            }
            catch (std::runtime_error&e){
                throw LispError("exit: Contract violation:\n expected: (exit) (exit <val>)");
            }
        }
        std::exit(code);
    }
    else
        throw LispError("exit: Arguments mismatch:\n expected:0 or 1\n given: "+std::to_string(params.size()));
}
ValuePtr newline(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=0)
        throw LispError("newline: Arguments mismatch:\n expected:0\n given: "+std::to_string(params.size()));
    std::cout<<'\n';
    return std::make_shared<NilValue>();
}
ValuePtr print(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()==0)
        throw LispError("newline: Arguments mismatch:\n expected:at least 1\n given: 0");
    for(auto& i:params)
        std::cout<<i->toString()<<' ';
    std::cout<<"\n";
    return std::make_shared<NilValue>();
}

ValuePtr append(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()==0)
        return std::make_shared<NilValue>();
    std::vector<ValuePtr>elems;
    for(auto arg:params){
        if(!arg->isList())
            throw LispError("append: Contract violation:\n expected: (append <list>...)\n given: "+arg->toString());
        auto list_elems=arg->toVector();
        std::copy(list_elems.begin(),list_elems.end(),std::back_inserter(elems));
    }
    return std::make_shared<PairValue>(elems);
}
ValuePtr car(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("car: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    if(!arg->isPair())
            throw LispError("car: Contract violation:\n expected: (car <pair>)\n given: "+arg->toString());
    return static_cast<PairValue&>(*arg).getCar();
}
ValuePtr cdr(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("cdr: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    if(!arg->isPair())
            throw LispError("cdr: Contract violation:\n expected: (cdr <pair>)\n given: "+arg->toString());
    return static_cast<PairValue&>(*arg).getCdr();
}
ValuePtr cons(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("cons: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    return std::make_shared<PairValue>(params[0],params[1]);
}
ValuePtr length(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=1)
        throw LispError("length: Arguments mismatch:\n expected:1\n given: "+std::to_string(params.size()));
    auto arg=params[0];
    if(!arg->isList())
            throw LispError("length: Contract violation:\n expected: (length <list>)\n given: "+arg->toString());
    return std::make_shared<NumericValue>(arg->toVector().size());
}
ValuePtr list(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()==0)
        return std::make_shared<NilValue>();
    return std::make_shared<PairValue>(params);
}
ValuePtr map(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw LispError("map: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    auto proc=params[0];
    if(!(proc->isProc()||proc->isLambda()))
        throw LispError("map: Contract violation:\n expected: (map <proc> <list>)\n given: "+proc->toString());
    auto list=params[1];
    if(!list->isList())
        throw LispError("map: Contract violation:\n expected: (map <proc> <list>)\n given: "+proc->toString());
    auto elems=list->toVector();
    if(elems.size()==0)
        return std::make_shared<NilValue>();
    std::vector<ValuePtr>new_elems{};
    std::transform(elems.begin(), elems.end(), std::back_inserter(new_elems),
     [&](ValuePtr e){
        return EvalEnv::apply(proc,{e},env);});
    return std::make_shared<PairValue>(new_elems);
}
ValuePtr filter(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw SyntaxError("filter: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    auto proc=params[0];
    if(!(proc->isProc()||proc->isLambda()))
        throw LispError("filter: Contract violation:\n expected: (filter <proc> <list>)\n given: "+proc->toString());
    auto list=params[1];
    if(!list->isList())
        throw LispError("filter: Contract violation:\n expected: (filter <proc> <list>)\n given: "+list->toString());
    auto elems=list->toVector();
    if(elems.size()==0)
        return std::make_shared<NilValue>();
    std::vector<ValuePtr>new_elems{};
    try{
    std::copy_if(elems.begin(), elems.end(), std::back_inserter(new_elems),
     [&](ValuePtr e){auto res= EvalEnv::apply(proc,{e},env);
                    return res->isBoolean()&&res->toString()=="#f" ? false : true;});}
    catch(LispError& e){
        throw LispError("filter: Contract violation:\n expected: <proc> should accept one parameter\n given: "+proc->toString());
    }
    if(new_elems.size()==0)
        return std::make_shared<NilValue>();
    //return std::make_shared<PairValue>(std::make_shared<SymbolValue>("quasiquote"),std::make_shared<PairValue>(new_elems));
    return std::make_shared<PairValue>(new_elems);
}
ValuePtr reduce(const std::vector<ValuePtr>& params,EvalEnv& env){
    if(params.size()!=2)
        throw SyntaxError("reduce: Arguments mismatch:\n expected:2\n given: "+std::to_string(params.size()));
    auto proc=params[0];
    if(!(proc->isProc()||proc->isLambda()))
        throw LispError("reduce: Contract violation:\n expected: (reduce <proc> <list>)\n given: "+proc->toString());
    auto list=params[1];
    if(!list->isList())
        throw LispError("reduce: Contract violation:\n expected: (reduce <proc> <list>)\n given: "+list->toString());
    else if(list->isNil())
        throw LispError("reduce: Contract violation:\n expected: <list> cannot be a nil");
    else {
        auto elems=list->toVector();
        if(elems.size()==1)
            return elems[0];
        else{
            try{
            return std::accumulate(elems.begin()+1,elems.end(),elems[0],
            [&](ValuePtr e1, ValuePtr e2){
                auto res=EvalEnv::apply(proc,{e1, e2},env);
                return res;});
            }
            catch(LispError& e){
                throw LispError("reduce: Contract violation:\n expected: <proc> should accept two parameter and be numeric\n given: "+proc->toString());
            }
        }
    }
}

bool isIntegar(double x){
    return std::floor(x)==x;
}