#ifndef FORM_H
#define FORM_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>

#include "./eval_env.h"

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
extern const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;
SpecialFormType defineForm;
SpecialFormType quoteForm;
SpecialFormType conditionForm;  // if
SpecialFormType andForm;
SpecialFormType orForm;
SpecialFormType lambdaForm;
SpecialFormType condForm;  // cond
SpecialFormType beginForm;
SpecialFormType letForm;
SpecialFormType quasiquoteForm;
SpecialFormType unquoteForm;
#endif