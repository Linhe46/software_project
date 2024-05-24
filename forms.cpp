#include "./forms.h"
#include <optional>
//#include <iostream>
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define",defineForm},
    {"quote",quoteForm},
    {"if",conditionForm},
    {"and",andForm},
    {"or",orForm},
    {"lambda",lambdaForm}
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
            //std::cout<<*lambda_args<<'\n';
            auto lambda_arg=std::make_shared<PairValue>(std::make_shared<SymbolValue>("lambda"),std::move(lambda_args));
            //std::cout<<*lambda_arg<<'\n';
            auto lambda=std::make_shared<PairValue>(std::move(lambda_arg),std::make_shared<NilValue>());//包装为表达式
            //std::cout<<*lambda<<"\n";
            auto proc=std::make_shared<PairValue>(std::make_shared<SymbolValue>(name.value()),std::move(lambda));
            //std::cout<<*proc<<"\n";
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
            else throw LispError("Non-identifier in lambda params");
        }
    //检查是否均为符号？
    auto body=PairValue(args).getCdr()->toVector();//右半部分为过程表达式
    //for(auto pt:body)
     //std::cout<<*pt<<'\n';
    return std::make_shared<LambdaValue>(params,body,env.shared_from_this());
}