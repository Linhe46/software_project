#include "./forms.h"
#include <optional>
//#include <iostream>
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define",defineForm},
    {"quote",quoteForm},
    {"if",conditionForm},
    {"and",andForm},
    {"or",orForm},
    {"lambda",lambdaForm},
    {"cond",condForm},
    {"begin", beginForm},
    {"let", letForm},
    {"quasiquote", quasiquoteForm},
    {"unquote", unquoteForm}
};
ValuePtr defineForm(const std::vector<ValuePtr>&args, EvalEnv& env){
    if(auto name=args[0]->asSymbol()){
        if(args.size()<2)
            throw LispError("Malformed Define");
        auto first=name.value();
        auto second=env.eval(args[1]);
        env.defineBinding(std::move(first),std::move(second));
    }
    else if(args[0]->isPair()){//转换为lambda
        if(args.size()<2)
                    throw SyntaxError("Malformed Define");
        auto proc_args=args[0]->toVector();
        if(auto name=proc_args[0]->asSymbol()){//定义过程名
            ValuePtr proc_params=static_cast<PairValue&>(*args[0]).getCdr();//形参列表
            ValuePtr proc_body=std::make_shared<PairValue>(args,1);//表达式列表
            auto lambda_args=std::make_shared<PairValue>(std::move(proc_params),std::move(proc_body));
            auto lambda_arg=std::make_shared<PairValue>(std::make_shared<SymbolValue>("lambda"),std::move(lambda_args));
            auto lambda=std::make_shared<PairValue>(std::move(lambda_arg),std::make_shared<NilValue>());//包装为lambda表达式
            auto proc=std::make_shared<PairValue>(std::make_shared<SymbolValue>(name.value()),std::move(lambda));
            auto lambda_form_define=std::make_shared<PairValue>(std::make_shared<SymbolValue>("define"),std::move(proc));
            //std::cout<<*lambda_form_define<<"\n";
            env.eval(lambda_form_define);//形如(define f (lambda (x..) y...))
    }   }
    else{
        throw LispError("Malformed Define");
    }
    return std::make_shared<NilValue>();
}
ValuePtr quoteForm(const std::vector<ValuePtr>&args, EvalEnv& env){
    if(args.size()==0)
        return std::make_shared<NilValue>();
    auto head=args[0];
    if(args.size()==1)
        return head;
    else return std::make_shared<PairValue>(args);
}
ValuePtr conditionForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()<2)
        throw LispError("Less than two clause");
    if(env.eval(args[0])->toString()=="#f"){
        try{
            return env.eval(args.at(2));
            }
        catch (std::out_of_range const&e){
            return std::make_shared<NilValue>();
        }
    }
    else
        return env.eval(args[1]);
}
ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()==0)
        return std::make_shared<BooleanValue>(true);
    for(const auto& param:args){
        auto res=env.eval(param);
        if(res->toString()=="#f")
            return res;
    }
    return env.eval(*args.rbegin());
}
ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()==0)
        return std::make_shared<BooleanValue>(false);
    for(const auto& param:args){
        auto res=env.eval(param);
        if(res->toString()!="#f")
            return res;
    }
    return env.eval(*args.rbegin());
}
ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    auto param_list=args[0]->toVector();//形参表
    std::vector<std::string>params;
    if(!(param_list.size()==1&&param_list[0]->isNil()))//判断形参是否为空     
        for(const auto x:param_list){
            if(auto name=x->asSymbol())
                params.push_back(name.value());
            else throw LispError("Non-identifier in lambda params");//非symbol型
        }
    //检查是否均为符号？
    auto body=PairValue(args).getCdr()->toVector();//右半部分为过程表达式
    //for(auto pt:body)
     //std::cout<<*pt<<'\n';
    return std::make_shared<LambdaValue>(params,body,env.shared_from_this());
}
ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()==0)
        return std::make_shared<NilValue>();
    ValuePtr res=nullptr;
    //检查参数是否正确
    for(int i=0;i<args.size();i++)
        if(!args[i]->isList())
            throw LispError("cond: bad syntax (clause is not a test-value pair)");
    for(int i=0;i<args.size();i++){
        auto arg=args[i];
        std::vector<ValuePtr> params{};
        auto pair=static_cast<PairValue&>(*arg);
        auto test=pair.getCar();
        if(test->isSymbol()&&test->asSymbol()=="else"){
            if(i!=args.size()-1)//else只能位于最后一个子句
                throw LispError("cond: bad syntax ('else' clause must be last)");
            params=pair.getCdr()->toVector();
            if(params.size()==0)//未定义行为，报错
                throw LispError("cond: missing expressions in `else' clause");
            else for(auto expr:params)
                res=env.eval(expr);
        }
        else if(env.eval(test)->toString()=="#f")
            continue;
        else{
            params=pair.getCdr()->toVector();
            if(params.size()==0)
                res=env.eval(test);
            else{
                for(auto expr:params)
                res=env.eval(expr);
            }
            break;
        }
    }
    return res;
}
ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()==0)
        return std::make_shared<NilValue>();
    ValuePtr res=nullptr;
    for(auto arg:args)
        res=env.eval(arg);
    return res;
}
ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()<2)
        throw LispError("let: bad syntax (missing binding pairs or body)");
    //转化为lambda形式
    std::vector<ValuePtr>param_names{};
    std::vector<ValuePtr>param_values{};
    std::vector<ValuePtr>param_body{};
    for(auto arg:args[0]->toVector()){
        if(arg->isPair()){
            auto binding_pair=static_cast<PairValue&>(*arg).toVector();
            auto name=binding_pair[0];
            if(!name->isSymbol())
                throw LispError("bad syntax (not an identifier and expression for a binding)");
            auto value=env.eval(binding_pair[1]);
            param_names.push_back(name);
            param_values.push_back(value);
        }
        else 
            throw LispError("bad syntax (not an identifier and expression for a binding)");
    }
    for(int i=1;i<args.size();i++)
        param_body.push_back(args[i]);
    auto lambda_body=std::make_shared<PairValue>(std::make_shared<PairValue>(param_names),std::make_shared<PairValue>(param_body));//获取lambda的形参表和过程体
    auto lambda_names=std::make_shared<PairValue>(std::make_shared<SymbolValue>("lambda"),std::move(lambda_body));//构建lambda形式
    auto lambda_form=std::make_shared<PairValue>(std::move(lambda_names),std::make_shared<PairValue>(param_values));//赋值
    return env.eval(std::move(lambda_form));
}
ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env){
    if(args.size()==0)
        return std::make_shared<NilValue>();
    else if(args.size()>1)
        throw LispError("let: bad syntax (missing binding pairs or body)");
    auto elems=args[0]->toVector();
    std::vector<ValuePtr>new_elems{};
    std::transform(elems.begin(),elems.end(),std::back_inserter(new_elems),
    [&](ValuePtr e){
        if(e->isPair()){
            auto params=e->toVector();
            if(params[0]->isSymbol()&&params[0]->asSymbol()=="unquote"){
                return env.eval(params[1]);
            }
        }
        return e;});
    //return std::make_shared<PairValue>(new_elems);
    auto res=std::make_shared<PairValue>(new_elems);;
    return res;
}
ValuePtr unquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env){
   throw LispError("unquote: bad syntax (only used in quasiquote syntax)");//显示调用是未定义行为
}