#include "./parser.h"

std::unordered_map<TokenType, std::string> isQuotes{
    {TokenType::QUOTE, "quote"},
    {TokenType::QUASIQUOTE, "quasiquote"},
    {TokenType::UNQUOTE, "unquote"}};

ValuePtr Parser::parse() {
    auto token = std::move(tokens.front());
    tokens.pop_front();  // 需要弹出
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    } else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    } else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    } else if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(value);
    } else if (token->getType() == TokenType::LEFT_PAREN) {
        return this->parseTails();
    } else if (isQuotes.find(token->getType()) != isQuotes.end()) {
        std::string quote_type = isQuotes[token->getType()];
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>(quote_type),
            std::make_shared<PairValue>(this->parse(),
                                        std::make_shared<NilValue>()));
    } else
        throw SyntaxError("Unimplemented");
}
ValuePtr Parser::parseTails() {
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = this->parse();
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = this->parse();
        if (tokens.front()->getType() != TokenType::RIGHT_PAREN)
            throw SyntaxError("Unimplemented");
        tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}