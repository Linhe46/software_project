#include "./forms.h"
#include <optional>

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define",defineForm}
};
ValuePtr defineForm(const std::vector<ValuePtr>&args, EvalEnv& env){
    if(auto name=args[0]->asSymbol()){
        if(args.size()<2)
            throw LispError("Malformed Define");
        auto first=name.value();
        auto second=env.eval(args[1]);
        env.addToSymbol(std::move(first),std::move(second));
    }
    else{
        throw LispError("Unimplemented");
    }
    return std::make_shared<NilValue>();
}