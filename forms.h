#ifndef FORM_H
#define FORM_H

#include "./eval_env.h"
using SpecialFormType=ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
extern const std::unordered_map<std::string,SpecialFormType*>SPECIAL_FORMS;
ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr conditionForm(const std::vector<ValuePtr>& args, EvalEnv& env);
SpecialFormType andForm;
SpecialFormType orForm;
SpecialFormType lambdaForm;

#endif