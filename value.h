#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "./error.h"

class EvalEnv;
using EvalEnvPtr = std::shared_ptr<EvalEnv>;

class Value;
using ValuePtr = std::shared_ptr<Value>;

class Value {
public:
    virtual std::string toString() const = 0;
    bool isNil() const;
    bool isPair() const;
    bool isList() const;
    bool isSelfEvaluating() const;
    bool isNumber() const;
    bool isSymbol() const;
    bool isProc() const;
    bool isLambda() const;
    bool isBoolean() const;
    bool isString() const;
    virtual std::vector<ValuePtr> toVector();
    std::optional<std::string> asSymbol() const;
    virtual double asNumber() const;
};

class BooleanValue : public Value {
private:
    bool value;

public:
    BooleanValue(bool value) : value{value} {}
    std::string toString() const override;
};
class NumericValue : public Value {
private:
    double value;

public:
    NumericValue(double value) : value{value} {}
    std::string toString() const override;
    double asNumber() const override;
};
class StringValue : public Value {
private:
    std::string value;

public:
    StringValue(const std::string& value) : value{value} {}
    std::string toString() const override;
};
class NilValue : public Value {
public:
    NilValue() {}
    std::string toString() const override;
    std::vector<ValuePtr> toVector() override;
};
class SymbolValue : public Value {
private:
    std::string name;

public:
    SymbolValue(const std::string& name) : name{name} {}
    std::string toString() const override;
};
class PairValue : public Value {
private:
    ValuePtr first, second;

public:
    PairValue(ValuePtr p1, ValuePtr p2)
        : first(std::move(p1)), second(std::move(p2)) {}
    PairValue(const std::vector<ValuePtr>& arg, int pos = 0);
    std::string toString() const override;
    std::vector<ValuePtr> toVector() override;
    ValuePtr getCdr() const;
    ValuePtr getCar() const;
};

// using BuiltinFuncType=ValuePtr(const std::vector<ValuePtr>&);
using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
class BuiltinProcValue : public Value {
private:
    BuiltinFuncType* func = nullptr;

public:
    BuiltinProcValue(BuiltinFuncType* func_) : func(func_) {}
    std::string toString() const override;
    ValuePtr operator()(const std::vector<ValuePtr>& params, EvalEnv& env);
};

class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    EvalEnvPtr env;

public:
    LambdaValue(const std::vector<std::string>&, const std::vector<ValuePtr>&,
                EvalEnvPtr env);
    ValuePtr apply(const std::vector<ValuePtr>& args);
    std::string toString() const override;
};

std::ostream& operator<<(std::ostream& os, const Value& value);
#endif