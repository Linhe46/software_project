#ifndef PARSER_H
#define PARSER_H
#include <deque>
#include <unordered_map>

#include "./error.h"
#include "./token.h"
#include "./value.h"
class Parser {
private:
    std::deque<TokenPtr> tokens;

public:
    Parser(std::deque<TokenPtr> tokens_) : tokens(std::move(tokens_)){};
    ValuePtr parse();
    ValuePtr parseTails();
};
#endif