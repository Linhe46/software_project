#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "./error.h"

enum class ValueType{
    BOOLEAN,
    NUMERIC,
    STRING,
    NIL,
    SYMBOL,
    PAIR,
    PROC//过程值
};

class Value;
using ValuePtr = std::shared_ptr<Value>;

class Value{
    private:
        ValueType type;
    protected:
        Value(ValueType type):type{type} {}
    public:
        ValueType getType()const{
            return type;
        }
        virtual std::string toString() const=0;
        bool isSelfEvaluating(ValueType);
        virtual std::vector<ValuePtr>toVector();
        std::optional<std::string> asSymbol() const;
        virtual double asNumber() const;
};

class BooleanValue:public Value{
    private:
        bool value;
    public:
        BooleanValue(bool value):Value(ValueType::BOOLEAN),value{value}{}
        std::string toString() const override;
};
class NumericValue:public Value{
    private:
        double value;
    public:
        NumericValue(double value):Value(ValueType::NUMERIC),value{value} {}
        std::string toString() const override;
        double asNumber()const override;
};
class StringValue:public Value{
    private:
        std::string value;
    public:
        StringValue(const std::string& value):Value(ValueType::STRING),value{value}{}
        std::string toString() const override;
};
class NilValue:public Value{
    public:
        NilValue():Value(ValueType::NIL){}
        std::string toString() const override;
};
class SymbolValue:public Value{
    private:
        std::string name;
    public:
        SymbolValue(const std::string& name):Value(ValueType::SYMBOL),name{name}{}
        std::string toString() const override;
};
class PairValue:public Value{
    private:
    public:
        ValuePtr first,second;//破坏封装性？
    public:
        PairValue(ValuePtr p1,ValuePtr p2):Value(ValueType::PAIR),first(std::move(p1)),second(std::move(p2)){} 
        std::string toString() const override;
        std::vector<ValuePtr>toVector()override;
        ValuePtr getRight() const;
};

std::ostream& operator<<(std::ostream& os ,const Value& value);

using BuiltinFuncType=ValuePtr(const std::vector<ValuePtr>&);
class BuiltinProcValue:public Value{
    private:
        BuiltinFuncType* func=nullptr;
    public:
        BuiltinProcValue(BuiltinFuncType* func_):func(func_),Value(ValueType::PROC){}
        std::string toString() const override;
        ValuePtr operator()(const std::vector<ValuePtr>&params);
};
#endif