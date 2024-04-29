#pragma once

#include <memory>
#include <string>

namespace lang
{
    class Lexer;
    class Parser;
    class Generator;

    class Lang
    {
        public:
            Lang();
            ~Lang();
            
            int run_source_code(const char* absolute_path_of_source_code);

        private:

            void run(std::string&& source);

        private:
            /** First lexer will be created then parser and then interpreter */
            std::unique_ptr<lang::Lexer> m_lexer;

            std::unique_ptr<lang::Parser> m_parser;

            std::unique_ptr<lang::Generator> m_generator;
            
    };
}