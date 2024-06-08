#include <iostream>
#include <string>
#include <fstream>

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
/*
std::string readFromFile(const std::string& filename){
    std::ifstream file(filename);
    if(!file.is_open())//input文件不存在，退出
        std::exit(1);
    std::string s;
    char x;
    while(file.get(x))
        s += x;
    return s;
}*/
//./bin/mini_lisp D:\mini_lisp\src\test.txt
int main(int argc, char* argv[]){
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    EvalEnvPtr env=EvalEnv::createGlobal();//全局求值环境
    bool read_from_file=false;
    std::ifstream file;
    if(argc==2){
        file=std::ifstream(argv[1]);
        if(file.is_open())
            read_from_file=true;
        else std::exit(1);
    }
    std::istream& input=(read_from_file ? file : std::cin);
    while (true) {
        try {
            if(!read_from_file)
                std::cout << ">>> ";
            if (input.eof()) {
                std::exit(0);
            }
            std::string line;
            std::getline(input, line);
            //std::cout<<line<<'\n';
            if(line=="")
                continue;
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));  // TokenPtr不支持复制，用移动
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            if(!read_from_file)
                std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}