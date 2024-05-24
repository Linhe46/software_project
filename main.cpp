#include <iostream>
#include <string>

#include "./eval_env.h"
#include "./parser.h"
#include "./tokenizer.h"
#include "./value.h"
#include "rjsj_test.hpp"

struct TestCtx {
    EvalEnvPtr env=EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result =env->eval(std::move(value));
        return result->toString();
    }
};

int main() {
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv6);
    EvalEnvPtr env=EvalEnv::createGlobal();//全局求值环境
    while (true) {
        try {
            std::cout << ">>> ";
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));  // TokenPtr不支持复制，用移动
            auto value = parser.parse();
            //EvalEnv env;
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}