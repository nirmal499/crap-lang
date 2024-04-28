#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace ast
    {   

        /**********************************************************************************************************************8*/
        struct ExpressionStatement;
        struct PrintStatement;
        struct VarStatement;
        struct BlockStatement;
        struct IfStatement;
        struct WhileStatement;
        struct FunctionStatement;
        struct ReturnStatement;
        
        struct BaseVisitorForStatement
        {
            virtual void visit(ExpressionStatement* statement) = 0;
            virtual void visit(PrintStatement* statement) = 0;
            virtual void visit(VarStatement* statement) = 0;
            virtual void visit(BlockStatement* statement) = 0;
            virtual void visit(IfStatement* statement) = 0;
            virtual void visit(WhileStatement* statement) = 0;
            virtual void visit(FunctionStatement* statement) = 0;
            virtual void visit(ReturnStatement* statement) = 0;
        };

        struct Statement
        {
            virtual void accept(BaseVisitorForStatement* visitor) = 0;
            virtual ~Statement() = default;
        };
        /**********************************************************************************************************************8*/


        struct BinaryExpression;
        struct GroupingExpression;
        struct LiteralExpression;
        struct UnaryExpression;
        struct VariableExpression;
        struct AssignmentExpression;
        struct LogicalExpression;
        struct CallExpression;
        struct ParenthesizeExpression;

        struct BaseVisitorForExpression
        {
            virtual void visit(BinaryExpression* expression) = 0;
            virtual void visit(GroupingExpression* expression) = 0;
            virtual void visit(LiteralExpression* expression) = 0;
            virtual void visit(UnaryExpression* expression) = 0;
            virtual void visit(VariableExpression* expression) = 0;
            virtual void visit(AssignmentExpression* expression) = 0;
            virtual void visit(LogicalExpression* expression) = 0;
            virtual void visit(CallExpression* expression) = 0;
            virtual void visit(ParenthesizeExpression* expression) = 0;
        };

        struct Expression
        {
            virtual void accept(BaseVisitorForExpression* visitor) = 0;
            virtual ~Expression() = default;
        };

        /**********************************************************************************************************************8*/
        struct ExpressionStatement: public Statement
        {
            std::unique_ptr<Expression> expr;

            ExpressionStatement(std::unique_ptr<Expression> expr)
                : expr(std::move(expr))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct PrintStatement: public Statement
        {
            std::unique_ptr<Expression> expr;

            PrintStatement(std::unique_ptr<Expression> expr)
                : expr(std::move(expr))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct VarStatement: public Statement
        {
            lang::Token name; /* Stores the TokenType::IDENTIFIER */
            std::unique_ptr<Expression> initializer;

            VarStatement(const lang::Token& name, std::unique_ptr<Expression> initializer)
                : name(name), initializer(std::move(initializer))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct BlockStatement: public Statement
        {
            std::vector<std::unique_ptr<Statement>> statements;

            BlockStatement(std::vector<std::unique_ptr<Statement>>&& statements)
                : statements(std::move(statements))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct IfStatement: public Statement
        {
            std::unique_ptr<Expression> condition;
            std::unique_ptr<Statement> thenBranch;
            std::unique_ptr<Statement> elseBranch;

            IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenBranch, std::unique_ptr<Statement> elseBranch)
                : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct WhileStatement: public Statement
        {
            std::unique_ptr<Expression> condition_expr;
            std::unique_ptr<Statement> body_stmt;

            WhileStatement(std::unique_ptr<Expression> condition_expr, std::unique_ptr<Statement> body_stmt)
                : condition_expr(std::move(condition_expr)), body_stmt(std::move(body_stmt))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct FunctionStatement: public Statement
        {
            lang::Token name;   /* Stores the function name */
            std::vector<lang::Token> params; /* Stores the parametes*/
            std::vector<std::unique_ptr<Statement>> body_stmts;

            FunctionStatement(const lang::Token& name, std::vector<lang::Token>&& params, std::vector<std::unique_ptr<Statement>>&& body_stmts)
                : name(name), params(std::move(params)), body_stmts(std::move(body_stmts))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct ReturnStatement: public Statement
        {
            lang::Token keyword; /* stores the keyword 'return' */
            std::unique_ptr<Expression> expr;

            ReturnStatement(const lang::Token& keyword, std::unique_ptr<Expression> expr)
                : keyword(keyword), expr(std::move(expr))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };
        /**********************************************************************************************************************8*/


        struct BinaryExpression: public Expression
        {
            std::unique_ptr<Expression> left;
            lang::Token op;
            std::unique_ptr<Expression> right;

            BinaryExpression(std::unique_ptr<Expression> left, const lang::Token& op, std::unique_ptr<Expression> right)
                : left(std::move(left)), right(std::move(right)), op(op)
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct GroupingExpression: public Expression
        {
            /* '(' expression ')' */
            std::unique_ptr<Expression> expr;

            GroupingExpression(std::unique_ptr<Expression> expr)
                : expr(std::move(expr))
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct LiteralExpression: public Expression
        {
            /* We can only store double */
            double value;

            LiteralExpression(double value)
                : value(value)
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct UnaryExpression: public Expression
        {
            lang::Token op;
            std::unique_ptr<Expression> expr;

            UnaryExpression(const lang::Token& op, std::unique_ptr<Expression> expr)
                : op(op), expr(std::move(expr))
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        /* Expression class for referencing a variable, like "a". */
        struct VariableExpression: public Expression
        {
            lang::Token name; /** Stores the TokenType::IDENTIFIER */

            VariableExpression(const lang::Token& name)
                : name(name)
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct AssignmentExpression: public Expression
        {
            std::unique_ptr<Expression> expr;
            lang::Token name; /* Stores assignment operator*/


            AssignmentExpression(const lang::Token& name, std::unique_ptr<Expression> expr)
                : name(name), expr(std::move(expr))
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct LogicalExpression: public Expression
        {
            std::unique_ptr<Expression> left;
            lang::Token op; /* Stores logical operator*/
            std::unique_ptr<Expression> right;


            LogicalExpression(std::unique_ptr<Expression> left, const lang::Token& op, std::unique_ptr<Expression> right)
                : left(std::move(left)), op(op), right(std::move(right))
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct CallExpression: public Expression
        {
            std::unique_ptr<Expression> callee;
            lang::Token closing_paren;
            std::vector<std::unique_ptr<Expression>> arguments;


            CallExpression(std::unique_ptr<Expression> callee, const lang::Token& closing_paren, std::vector<std::unique_ptr<Expression>>&& arguments)
                : callee(std::move(callee)), closing_paren(closing_paren), arguments(std::move(arguments))
            {}

            void accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };
    }
}