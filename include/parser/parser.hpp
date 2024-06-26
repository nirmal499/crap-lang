#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <token/token.hpp>
#include <types/token_type.hpp>

namespace lang
{
    namespace ast
    {
        struct Statement;
        struct Expression;
    }

    // class Token; /* Here in this header file we are not using a pointer to Token. */

    class Parser
    {
        public:
            Parser();
            ~Parser();

            std::pair<std::vector<std::unique_ptr<lang::ast::Statement>>, std::vector<std::string>> parse(std::vector<lang::Token>&& tokens);

        private:
            std::unique_ptr<lang::ast::Statement> parse_declaration();

            std::unique_ptr<lang::ast::Statement> parse_var_declaration();

            std::unique_ptr<lang::ast::Statement> parse_statement();
            std::unique_ptr<lang::ast::Statement> parse_print_statement();
            std::unique_ptr<lang::ast::Statement> parse_expression_statement();
            std::vector<std::unique_ptr<lang::ast::Statement>> parse_block();
            std::unique_ptr<lang::ast::Statement> parse_while_statement();
            std::unique_ptr<lang::ast::Statement> parse_function_statement();

            std::unique_ptr<lang::ast::Statement> parse_if_statement();
            std::unique_ptr<lang::ast::Statement> parse_return_statement();
            
            std::unique_ptr<lang::ast::Expression> parse_expression();
            std::unique_ptr<lang::ast::Expression> parse_call_expression();
            std::unique_ptr<lang::ast::Expression> parse_assignment();

            std::unique_ptr<lang::ast::Expression> parse_equality();
            std::unique_ptr<lang::ast::Expression> parse_logical_or_expression();
            std::unique_ptr<lang::ast::Expression> parse_logical_and_expression();

            std::unique_ptr<lang::ast::Expression> parse_comparison();

            std::unique_ptr<lang::ast::Expression> parse_term();
            

            std::unique_ptr<lang::ast::Expression> parse_factor();

            std::unique_ptr<lang::ast::Expression> parse_unary();

            std::unique_ptr<lang::ast::Expression> parse_primary();

            lang::Token consume(lang::TokenType type, std::string message);

            /* It returns nullptr */
            std::unique_ptr<lang::ast::Statement> error(const Token& token, const std::string& message);

            void generate_error(int line, const std::string& message);
            
            bool match(const std::initializer_list<lang::TokenType>& matching_list);

            bool check(lang::TokenType type);

            lang::Token advance();

            bool is_at_end();

            Token peek();

            Token previous();

            void synchronize_after_an_error();

            std::unique_ptr<lang::ast::Expression> parse_single_call_expression_helper_function(std::unique_ptr<lang::ast::Expression> callee);

        private:
            std::vector<lang::Token> m_tokens;
            int m_current{0};

            std::vector<std::string> m_errors;

            std::vector<std::unique_ptr<lang::ast::Statement>> m_statements;

    };
}