#include "./forms.h"
#include <optional>
const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define",defineForm}
};
ValuePtr defineForm(const std::vector<ValuePtr>&args, EvalEnv& env){
    if(auto name=args[0]->asSymbol()){
        if(args.size()<2)
            throw LispError("Malformed Define");
        env.addToSymbol(name.value(),env.eval(args[1]));
    }
    else{
        throw LispError("Unimplemented");
    }
    return std::make_shared<NilValue>();
}