#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "./error.h"

class Value;
using ValuePtr = std::shared_ptr<Value>;

class Value{
    public:
        virtual std::string toString() const=0;
        bool isNil() const;
        bool isPair() const;
        bool isSelfEvaluating() const;
        bool isNumber() const;
        bool isSymbol() const;
        bool isProc() const;
        virtual std::vector<ValuePtr>toVector();
        std::optional<std::string> asSymbol() const;
        virtual double asNumber() const;
};

class BooleanValue:public Value{
    private:
        bool value;
    public:
        BooleanValue(bool value):value{value}{}
        std::string toString() const override;
};
class NumericValue:public Value{
    private:
        double value;
    public:
        NumericValue(double value):value{value} {}
        std::string toString() const override;
        double asNumber()const override;
};
class StringValue:public Value{
    private:
        std::string value;
    public:
        StringValue(const std::string& value):value{value}{}
        std::string toString() const override;
};
class NilValue:public Value{
    public:
        NilValue(){}
        std::string toString() const override;
};
class SymbolValue:public Value{
    private:
        std::string name;
    public:
        SymbolValue(const std::string& name):name{name}{}
        std::string toString() const override;
};
class PairValue:public Value{
    private:
        ValuePtr first,second;//破坏封装性？
    public:
        PairValue(ValuePtr p1,ValuePtr p2):first(std::move(p1)),second(std::move(p2)){} 
        std::string toString() const override;
        std::vector<ValuePtr>toVector()override;
        ValuePtr getCdr() const;
        ValuePtr getCar() const;
};

using BuiltinFuncType=ValuePtr(const std::vector<ValuePtr>&);
class BuiltinProcValue:public Value{
    private:
        BuiltinFuncType* func=nullptr;
    public:
        BuiltinProcValue(BuiltinFuncType* func_):func(func_){}
        std::string toString() const override;
        ValuePtr operator()(const std::vector<ValuePtr>&params);
};

std::ostream& operator<<(std::ostream& os ,const Value& value);
#endif