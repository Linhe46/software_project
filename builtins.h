#ifndef BUILTINS_H
#define BUILTINS_H

#include "./value.h"
#include <unordered_map>
class procDict{
    static std::unordered_map<std::string,ValuePtr>dict;
    void insert(const std::string& name, BuiltinFuncType* proc);
    void loadProcs();
    public:
        procDict(){
            if(dict.size()==0)
                loadProcs();
        }
        ValuePtr operator[](const std::string& name);
        static std::unordered_map<std::string, ValuePtr>::iterator begin();
        static std::unordered_map<std::string, ValuePtr>::iterator end();
};

//std::unordered_map<std::string,ValuePtr>procDict();

using arithmeticType=double(*)(double,double);
using numericCompareType=bool(*)(double,double);
using typeCheckType=bool(*)(ValuePtr);

bool isIntegar(double x);//删去更改为is_integer实现

//算术库
ValuePtr arithmetic(const std::vector<ValuePtr>& params,arithmeticType func,double init, int least_params);
BuiltinFuncType add;
BuiltinFuncType sub;
BuiltinFuncType mult;
BuiltinFuncType divi;
BuiltinFuncType abs_;
BuiltinFuncType expt;
BuiltinFuncType quotient;
BuiltinFuncType modulo;
BuiltinFuncType remainder_;

//比较库
ValuePtr numericCompare(const std::vector<ValuePtr>& params, numericCompareType func);
BuiltinFuncType eq;
BuiltinFuncType equal;
BuiltinFuncType negation;
BuiltinFuncType greater;
BuiltinFuncType less;
BuiltinFuncType less_equal;
BuiltinFuncType greater_equal;
BuiltinFuncType numeric_equal;
BuiltinFuncType even;
BuiltinFuncType odd;
BuiltinFuncType zero;

//类型检查库
ValuePtr typeCheck(const std::vector<ValuePtr>&, typeCheckType, const std::string&);
BuiltinFuncType is_atom;
BuiltinFuncType is_boolean;
BuiltinFuncType is_integer;
BuiltinFuncType is_list;
BuiltinFuncType is_number;
BuiltinFuncType is_null;
BuiltinFuncType is_pair;
BuiltinFuncType is_procedure;
BuiltinFuncType is_string;
BuiltinFuncType is_symbol;

//核心库
BuiltinFuncType apply;
BuiltinFuncType display;
BuiltinFuncType displayln;
BuiltinFuncType error;
BuiltinFuncType eval;
BuiltinFuncType exit;
BuiltinFuncType newline;
BuiltinFuncType print;
#endif