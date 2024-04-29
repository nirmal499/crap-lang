#pragma once

/* Keep less header files as this will lead to less compile time */

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <vector>
#include <memory>
#include <string>
#include <ast/ast.hpp>

namespace lang
{
    class Generator
    {
        public:
            Generator();
            ~Generator();
            
            std::vector<std::string> generate(std::vector<std::unique_ptr<lang::ast::Statement>>&& statements);

            void save_module_to_file(const std::string& file_name);
            void print_module();
        
        private:

            void module_initialization();

            llvm::Value* gen(std::vector<std::unique_ptr<lang::ast::Statement>>&& statements);
            
            llvm::Function* create_function(const std::string& fnName, llvm::FunctionType* fnType);
            llvm::Function* create_function_proto(const std::string& fnName, llvm::FunctionType* fnType);
            void create_function_block(llvm::Function* fn);
            llvm::BasicBlock* create_BB(const std::string& name, llvm::Function* fn = nullptr);

            llvm::Value* error(const Token& token, const std::string& message);
            void generate_error(int line, const std::string& message);
            

        private:
            std::unique_ptr<llvm::LLVMContext> m_ctx;
            std::unique_ptr<llvm::Module> m_module;
            std::unique_ptr<llvm::IRBuilder<>> m_builder;

            std::vector<std::string> m_errors;

            llvm::Function* fn;

    };
}