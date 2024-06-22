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
int main(int argc, char* argv[]){
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    EvalEnvPtr env=EvalEnv::createGlobal();//全局求值环境
    bool read_from_file=false;
    std::ifstream file;
    if(argc==2){
        file=std::ifstream(argv[1]);
        if(file.is_open())
            read_from_file=true;
        else std::exit(1);//文件不存在
    }
    std::istream& input=(read_from_file ? file : std::cin);
    while (true) {
        try {
            if(!read_from_file)
                std::cout << ">>> ";
            //分行输入括号匹配
            if(input.eof())
                break;
            char x;
            int counter=0;
            std::string input_std="";
            while(input.get(x)){
                if(x==char(26)){
                    break;
                }
                if(x=='\n'){
                    if(counter==0)
                        break;
                    else if(counter<0)
                        throw SyntaxError("Bad syntax: Unclosed brackets.");
                    else{
                        input_std+=' ';
                        continue;
                    }                     
                }
                if(x=='(')
                    counter++;
                if(x==')')
                    counter--;
                input_std+=x;
            }
            std::string line;
            std::getline(std::stringstream(input_std), line);
            if(line=="")
                continue;
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));  // TokenPtr不支持复制，用移动
            auto value = parser.parse();
            auto result = env->eval(std::move(value));
            if(!read_from_file)
                std::cout << result->toString() << std::endl;
        } catch (SyntaxError& e) {
            std::cerr <<"\033[31m"<<"SyntaxError: " << e.what() <<"\033[0m"<< std::endl;
        }
        catch (LispError& e) {
            std::cerr <<"\033[31m"<<"LispError: " << e.what() <<"\033[0m"<< std::endl;
        }
        catch (std::runtime_error& e) {
            std::cerr <<"\033[31m"<<"Error: " << e.what() <<"\033[0m"<< std::endl;
        }
    }
}
//./bin/mini_lisp D:\mini_lisp\src\lv7-answer.scm