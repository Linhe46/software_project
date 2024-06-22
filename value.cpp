#include "./value.h"

#include <iomanip>
#include <sstream>
bool Value::isNil() const {
    return typeid(*this) == typeid(NilValue);
}
bool Value::isPair() const {
    return typeid(*this) == typeid(PairValue);
}
// 递归检查右半部分是否含有空表
bool Value::isList() const {
    if (this->isNil())
        return 1;
    else if (!this->isPair())
        return 0;
    auto pair = static_cast<const PairValue&>(*this);
    return pair.getCdr()->isList();
}
bool Value::isSelfEvaluating() const {
    return typeid(*this) == typeid(NumericValue) ||
           typeid(*this) == typeid(BooleanValue) ||
           typeid(*this) == typeid(StringValue);
}
bool Value::isNumber() const {
    return typeid(*this) == typeid(NumericValue);
}
bool Value::isSymbol() const {
    return typeid(*this) == typeid(SymbolValue);
}
bool Value::isProc() const {
    return typeid(*this) == typeid(BuiltinProcValue);
}
bool Value::isLambda() const {
    return typeid(*this) == typeid(LambdaValue);
}
bool Value::isBoolean() const {
    return typeid(*this) == typeid(BooleanValue);
}
bool Value::isString() const {
    return typeid(*this) == typeid(StringValue);
}
std::vector<ValuePtr> Value::toVector() {  // 只处理列表
    throw LispError("Converted to vector failed");
}
std::optional<std::string> Value::asSymbol() const {  // 获取符号名
    if (typeid(*this) != typeid(SymbolValue)) return std::nullopt;
    return this->toString();
}
double Value::asNumber() const {
    throw LispError("Not a Numeric type");
}
double NumericValue::asNumber() const {
    return value;
}

std::string BooleanValue::toString() const {
    return (value ? "#t" : "#f");
}
std::string NumericValue::toString() const {
    std::string str;
    str = std::to_string(value);
    while (str.back() == '0')  // 不断移除末尾0
        str.pop_back();
    if (str.back() == '.')  // 移除末尾小数点
        str.pop_back();
    return str;
}
std::string StringValue::toString() const {
    std::ostringstream ss;
    ss << std::quoted(value);
    return ss.str();
}
std::string NilValue::toString() const {
    return "()";
}
std::string SymbolValue::toString() const {
    return name;
}
std::string PairValue::toString() const {
    if (typeid(*second) == typeid(PairValue)) {     // 右半部分为对子
        std::string str_pair = second->toString();  // 右半部分递归展开
        str_pair.erase(str_pair.begin());           // 删除子列表左括号
        return "(" + first->toString() + " " + str_pair;
    } else if (typeid(*second) ==
               typeid(NilValue))  // 右边部分为空表，右括号闭合
        return "(" + first->toString() + ")";
    return "(" + first->toString() + " . " + second->toString() +
           ")";  // 右半部分不再是列表，点分隔，右括号闭合
}
PairValue::PairValue(const std::vector<ValuePtr>& args, int pos)
    : first(nullptr), second(nullptr) {  // vector转换到列表
    if (args.size() <= 0) throw("Vector to Pair Failed");
    first = args[pos];
    if (pos == args.size() - 1)
        second = std::make_shared<NilValue>();
    else
        second = std::make_shared<PairValue>(args, ++pos);
}

std::vector<ValuePtr> NilValue::toVector() {  // 单独转换空表，一定在末尾，可忽略
    return {};
}
std::vector<ValuePtr> PairValue::toVector() {  // 递归地转换为数组
    std::vector<ValuePtr> values;
    if (!this->isList())
        throw LispError("Error: Converting a Pair but not a List");
    auto& pair = static_cast<PairValue&>(*this);

    values.push_back(pair.first);
    auto child_vec = pair.second->toVector();  // 列表的右半部分一定是列表
    std::copy(child_vec.begin(), child_vec.end(), std::back_inserter(values));
    return values;
}

ValuePtr PairValue::getCdr() const {
    // return
    // std::make_shared<PairValue>(std::make_shared<NilValue>(),this->second);
    return second;
}
ValuePtr PairValue::getCar() const {
    return this->first;
}
std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}
ValuePtr BuiltinProcValue::operator()(const std::vector<ValuePtr>& params,
                                      EvalEnv& env) {
    return func(params, env);
}

LambdaValue::LambdaValue(const std::vector<std::string>& params_,
                         const std::vector<ValuePtr>& body_, EvalEnvPtr env_)
    : params(params_), body(body_), env(env_) {}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}

std::ostream& operator<<(std::ostream& os, const Value& value) {
    return os << value.toString();
}
