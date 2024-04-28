#include <parser/parser.hpp>

namespace lang
{
    std::pair<std::vector<std::unique_ptr<lang::ast::Statement>>, std::vector<std::string>> Parser::parse(std::vector<lang::Token>&& tokens)
    {
        m_tokens = std::move(tokens);

        m_current = 0;
        /* It is important because we are moving from this class to outside at the end of tokenize function */
        m_errors = std::vector<std::string>();
        // m_temp_exprs.clear();
        m_statements = std::vector<std::unique_ptr<lang::ast::Statement>>();

        while(!this->is_at_end())
        {
            m_statements.emplace_back(this->parse_declaration());
        }

        return std::make_pair(std::move(m_statements), std::move(m_errors));;
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_declaration()
    {
        try
        {
            if(this->match({lang::TokenType::FUN}))
            {
                return this->parse_function_statement();
            }
            
            if(this->match({lang::TokenType::VAR}))
            {
                return this->parse_var_declaration();
            }

            return this->parse_statement();
        }
        catch(const std::exception& e)
        {
            this->synchronize_after_an_exception();

            return nullptr;
        }
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_function_statement()
    {
        /* name will store the 'fun' */
        lang::Token name = this->consume(lang::TokenType::IDENTIFIER,"Expect 'function' name");
        (void)this->consume(lang::TokenType::LEFT_PAREN, "Expect '(' after function name");
        
        std::vector<lang::Token> parameters;
        if(!this->check(lang::TokenType::RIGHT_PAREN))
        {
            do
            {
                if(parameters.size() >= 255)
                {
                    this->generate_error(this->peek().m_line, "Can't have more than 255 parameters");
                }

                parameters.emplace_back(this->consume(lang::TokenType::IDENTIFIER, "Expect parameter name."));

            } while(this->match({lang::TokenType::COMMA}));
        }

        this->consume(lang::TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

        this->consume(lang::TokenType::LEFT_BRACE, "Expect '{' before function body.");
        std::vector<std::unique_ptr<lang::ast::Statement>> body_stmts = this->parse_block();

        auto function_statement = std::make_unique<lang::ast::FunctionStatement>(name, std::move(parameters), std::move(body_stmts));

        return std::move(function_statement);

    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_var_declaration()
    {
        lang::Token name = this->consume(lang::TokenType::IDENTIFIER, "Expect variable name.");
        std::unique_ptr<lang::ast::Expression> initializer = nullptr;

        if(this->match({lang::TokenType::EQUAL}))
        {
            initializer = this->parse_expression();
        }

        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after variable declaration");

        auto var_statement = std::make_unique<lang::ast::VarStatement>(name, std::move(initializer));
        return std::move(var_statement);
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_statement()
    {
        if(this->match({lang::TokenType::IF}))
        {
            return this->parse_if_statement();
        }

        if(this->match({lang::TokenType::PRINT}))
        {
            return this->parse_print_statement();
        }

        if(this->match({lang::TokenType::RETURN}))
        {
            return this->parse_return_statement();
        }

        if(this->match({lang::TokenType::WHILE}))
        {
            return this->parse_while_statement();
        }

        if(this->match({lang::TokenType::LEFT_BRACE}))
        {
            std::vector<std::unique_ptr<lang::ast::Statement>> stmts = this->parse_block();
            auto block_statement = std::make_unique<lang::ast::BlockStatement>(std::move(stmts));

            return std::move(block_statement);
        }

        return this->parse_expression_statement();
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_return_statement()
    {
        Token keyword = this->previous(); /* Stores the 'return' keyword Token */
        std::unique_ptr<lang::ast::Expression> value = nullptr;

        if(!this->check({lang::TokenType::SEMICOLON}))
        {
            value = this->parse_expression();
        }

        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after return value");

        auto return_statement = std::make_unique<lang::ast::ReturnStatement>(keyword, std::move(value));

        return std::move(return_statement);
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_while_statement()
    {
        (void)this->consume(lang::TokenType::LEFT_PAREN, "Expect '(' after 'while'.");

        std::unique_ptr<lang::ast::Expression> condition = this->parse_expression();

        (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expect ')' after condition.");

        std::unique_ptr<lang::ast::Statement> body = this->parse_statement();

        auto while_statement = std::make_unique<lang::ast::WhileStatement>(std::move(condition), std::move(body));

        return std::move(while_statement);
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_if_statement()
    {
        (void)this->consume(lang::TokenType::LEFT_PAREN, "Expect '(' after 'if'.");

        std::unique_ptr<lang::ast::Expression> condition = this->parse_expression();

        (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expect ')' after 'if'.");

        std::unique_ptr<lang::ast::Statement> thenBranch = this->parse_statement();
        std::unique_ptr<lang::ast::Statement> elseBranch = nullptr;

        if(this->match({lang::TokenType::ELSE}))
        {
            elseBranch = this->parse_statement();
        }

        auto if_statement = std::make_unique<lang::ast::IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));

        return std::move(if_statement);
    }

    std::vector<std::unique_ptr<lang::ast::Statement>> Parser::parse_block()
    {
        std::vector<std::unique_ptr<lang::ast::Statement>> statements;

        while(!this->check(lang::TokenType::RIGHT_BRACE) && !this->is_at_end())
        {
            statements.emplace_back(this->parse_declaration());
        }

        (void)this->consume(lang::TokenType::RIGHT_BRACE, "Expect '}' after block");

        return std::move(statements);
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_print_statement()
    {
        std::unique_ptr<lang::ast::Expression> expr = this->parse_expression();

        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after value");

        auto print_statement = std::make_unique<lang::ast::PrintStatement>(std::move(expr));

        return std::move(print_statement);
    }

    std::unique_ptr<lang::ast::Statement> Parser::parse_expression_statement()
    {
        std::unique_ptr<lang::ast::Expression> expr = this->parse_expression();
        
        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after expression");
        auto expression_statement = std::make_unique<lang::ast::ExpressionStatement>(std::move(expr));

        return std::move(expression_statement);
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_expression()
    {
        return this->parse_assignment();
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_assignment()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_logical_or_expression();

        if(this->match({lang::TokenType::EQUAL}))
        {
            lang::Token equals = this->previous();

            if(lang::ast::VariableExpression* var_expr = dynamic_cast<lang::ast::VariableExpression*>(left_expr.get()))
            {
                lang::Token name = var_expr->name;
                std::unique_ptr<lang::ast::Expression> assignment_expr = this->parse_assignment();
                
                auto assignment_expression = std::make_unique<lang::ast::AssignmentExpression>(name, std::move(assignment_expr));
                return std::move(assignment_expression);
            }

            this->error(equals, "Invalid assignment target");
        }

        return left_expr;
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_logical_or_expression()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_logical_and_expression();
        
        while(this->match({lang::TokenType::OR}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_logical_and_expression();
            
            auto logical_expression = std::make_unique<lang::ast::LogicalExpression>(std::move(left_expr), op, std::move(right_expr));
            left_expr = std::move(logical_expression);
        }

        return left_expr;
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_logical_and_expression()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_equality();
        
        while(this->match({lang::TokenType::AND}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_equality();
            
            auto logical_expression = std::make_unique<lang::ast::LogicalExpression>(std::move(left_expr), op, std::move(right_expr));
            left_expr = std::move(logical_expression);
        }

        return left_expr;
    }


    std::unique_ptr<lang::ast::Expression> Parser::parse_equality()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_comparison();

        while(this->match({lang::TokenType::BANG_EQUAL, lang::TokenType::EQUAL_EQUAL}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_comparison();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(std::move(left_expr), op, std::move(right_expr));
            left_expr = std::move(binary_expression);
        }

        return left_expr;
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_comparison()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_term();

        while(this->match({lang::TokenType::GREATER, lang::TokenType::GREATER_EQUAL, lang::TokenType::LESS, lang::TokenType::LESS_EQUAL}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_term();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(std::move(left_expr), op, std::move(right_expr));
            left_expr = std::move(binary_expression);
        }

        return left_expr;
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_term()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_factor();

        while(this->match({lang::TokenType::MINUS, lang::TokenType::PLUS}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_factor();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(std::move(left_expr), op, std::move(right_expr));
            left_expr = std::move(binary_expression);
        }

        return left_expr;
    }
    
    std::unique_ptr<lang::ast::Expression> Parser::parse_factor()
    {
        std::unique_ptr<lang::ast::Expression> left_expr = this->parse_unary();

        while(this->match({lang::TokenType::SLASH, lang::TokenType::STAR}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_unary();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(std::move(left_expr), op, std::move(right_expr));
            left_expr = std::move(binary_expression);
        }

        return left_expr;
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_unary()
    {
        if(this->match({lang::TokenType::BANG, lang::TokenType::MINUS}))
        {
            lang::Token op = this->previous();
            std::unique_ptr<lang::ast::Expression> right_expr = this->parse_unary();

            auto unary_expression = std::make_unique<lang::ast::UnaryExpression>(op, std::move(right_expr));

            return std::move(unary_expression);
        }

        return this->parse_call_expression();
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_call_expression()
    {
        std::unique_ptr<lang::ast::Expression> expr = this->parse_primary();

        while(this->match({lang::TokenType::LEFT_PAREN}))
        {
            auto single_call_expression = this->parse_single_call_expression_helper_function(std::move(expr));
            expr = std::move(single_call_expression);
        }

        return expr;
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_single_call_expression_helper_function(std::unique_ptr<lang::ast::Expression> callee)
    {
        std::vector<std::unique_ptr<lang::ast::Expression>> arguments;

        if(!this->check({lang::TokenType::RIGHT_PAREN}))
        {
            /* We have arguments */
            do
            {
                if(arguments.size() >= 255)
                {
                    this->generate_error(this->peek().m_line, "Can't have more than 255 arguments");
                }
                arguments.emplace_back(this->parse_expression());

            } while(this->match({lang::TokenType::COMMA}));
        }

        /* We do not have any arguments */
        lang::Token paren = this->consume(lang::TokenType::RIGHT_PAREN, "Expect ')' after arguments");

        auto call_expression = std::make_unique<lang::ast::CallExpression>(std::move(callee), paren, std::move(arguments));
        
        return std::move(call_expression);
    }

    std::unique_ptr<lang::ast::Expression> Parser::parse_primary()
    {
        std::unique_ptr<lang::ast::Expression> expr;

        if(this->match({lang::TokenType::FALSE}))
        {
            double value = 10.1;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            
            expr = std::move(literal_expression);

            return expr;
        }

        if(this->match({lang::TokenType::TRUE}))
        {
            double value = 10.2;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            
            expr = std::move(literal_expression);

            return expr;
        }

        if(this->match({lang::TokenType::NIL}))
        {
            double value = 10.3;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            
            expr = std::move(literal_expression);

            return expr;
        }

        if(this->match({lang::TokenType::NUMBER}))
        {
            // double value = this->previous().m_literal;
            double value = 10.4;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            
            expr = std::move(literal_expression);

            return expr;
        }

        if(this->match({lang::TokenType::STRING}))
        {
            double value = 10.5;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            
            expr = std::move(literal_expression);

            return expr;
        }

        if(this->match({lang::TokenType::IDENTIFIER}))
        {
            auto variable_expression = std::make_unique<lang::ast::VariableExpression>(this->previous());
            
            expr = std::move(variable_expression);

            return expr;
        }

        if(this->match({lang::TokenType::LEFT_PAREN}))
        {
            expr = this->parse_expression();
            (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expecting ')' after expression.");

            auto grouping_expression = std::make_unique<lang::ast::GroupingExpression>(std::move(expr));
            
            expr = std::move(grouping_expression);

            return expr;
        }

        this->error(this->peek(), "Expect Expression.");

        return nullptr; // Unreachable
    }

    lang::Token Parser::consume(lang::TokenType type, std::string message)
    {
        if(this->check(type))
        {
            return this->advance();
        }

        this->error(this->peek(), message);

        return lang::Token{lang::TokenType::MYEOF, "DEAD END", lang::util::null, 1}; // Unreachable
    }
    
    void Parser::error(const Token& token, std::string message)
    {
        if(token.m_type == lang::TokenType::MYEOF)
        {
            this->generate_error(token.m_line, " at end: " + message);
        }
        else
        {
            this->generate_error(token.m_line, " at '" + token.m_lexeme + "' " + message);
        }

        throw lang::util::parser_error("Parser Error Caught");
    }
    
    void Parser::generate_error(int line, std::string message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());
    }
    
    bool Parser::match(const std::initializer_list<lang::TokenType>& matching_list)
    {
        for(const auto& type: matching_list)
        {
            if(this->check(type))
            {
                this->advance();
                return true;
            }
        }

        return false;
    }

    bool Parser::check(lang::TokenType type)
    {
        if(this->is_at_end())
        {
            return false;
        }

        return this->peek().m_type == type;
    }

    void Parser::synchronize_after_an_exception()
    {
        this->advance();

        while(!this->is_at_end())
        {
            if(this->previous().m_type == lang::TokenType::SEMICOLON)
            {
                /* 
                    We have reached the end of the statement which caused an exception. 
                    So we return inorder to parse the other statements after this error statements
                */
                return;
            }

            switch(this->peek().m_type)
            {
                case lang::TokenType::CLASS:
                case lang::TokenType::FUN:
                case lang::TokenType::VAR:
                case lang::TokenType::FOR:
                case lang::TokenType::IF:
                case lang::TokenType::WHILE:
                case lang::TokenType::PRINT:
                case lang::TokenType::RETURN:
                    return;

            }

            this->advance();
        }
    }

    lang::Token Parser::advance()
    {
        if(!this->is_at_end())
        {
            m_current++;
        }

        return this->previous();
    }

    bool Parser::is_at_end()
    {
        return this->peek().m_type == lang::TokenType::MYEOF;
    }

    Token Parser::peek()
    {
        return m_tokens.at(m_current);
    }

    Token Parser::previous()
    {
        return m_tokens.at(m_current - 1);
    }
}