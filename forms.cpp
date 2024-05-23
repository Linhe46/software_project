#include "./forms.h"
#include <optional>
#include<iostream>
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define",defineForm}
};
ValuePtr defineForm(const std::vector<ValuePtr>&args, EvalEnv& env){
    if(auto name=args[0]->asSymbol()){
        if(args.size()<2)
            throw LispError("Malformed Define");
        auto first=name.value();
        auto second=env.eval(args[1]);
        std::cout<<first<<" "<<*second<<'\n';
        env.addToSymbol(first,*second);
    }
    else{
        throw LispError("Unimplemented");
    }
    return std::make_shared<NilValue>();
}