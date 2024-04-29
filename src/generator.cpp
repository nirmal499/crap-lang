#include <generator/generator.hpp>

#include "llvm/IR/Verifier.h"

namespace lang
{
    void Generator::save_module_to_file(const std::string& file_name)
    {
        std::error_code ec;
        llvm::raw_fd_ostream out(file_name, ec);
        m_module->print(out, nullptr);
    }

    void Generator::print_module()
    {
        m_module->print(llvm::outs(), nullptr);
    }

    std::vector<std::string> Generator::generate(std::vector<std::unique_ptr<lang::ast::Statement>>&& statements)
    {

        m_errors = std::vector<std::string>();

        auto fn = this->create_function("main", llvm::FunctionType::get(
                /* return type*/ m_builder->getInt32Ty(),
                /* vararg */ false
            ));

        /* generate IR for main body aka compile main body */
        auto result = this->gen(std::move(statements));

        /* Cast to i32 to return from main: */
        auto i32Result = m_builder->CreateIntCast(result, m_builder->getInt32Ty(), true);

        m_builder->CreateRet(i32Result);


        return std::move(m_errors);
    }

    llvm::Value* Generator::gen(std::vector<std::unique_ptr<lang::ast::Statement>>&& statements)
    {
        return m_builder->getInt32(42);
    }

    llvm::Function* Generator::create_function(const std::string& fnName, llvm::FunctionType* fnType)
    {
        /* Function prototype might already be defined */
        auto fn = m_module->getFunction(fnName);

        /* If not, allocate the function */
        if(fn == nullptr)
        {
            fn = this->create_function_proto(fnName, fnType);
        }

        this->create_function_block(fn);

        return fn;
    }

    llvm::Function* Generator::create_function_proto(const std::string& fnName, llvm::FunctionType* fnType)
    {
        auto fn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, fnName, *m_module);
        
        llvm::verifyFunction(*fn);

        return fn;
    }

    void Generator::create_function_block(llvm::Function* fn)
    {
        auto entry = this->create_BB("entry", fn);
        m_builder->SetInsertPoint(entry);
    }

    llvm::BasicBlock* Generator::create_BB(const std::string& name, llvm::Function* fn)
    {
        return llvm::BasicBlock::Create(*m_ctx, name, fn);
    }

    llvm::Value* Generator::error(const Token& token, const std::string& message)
    {
        if(token.m_type == lang::TokenType::MYEOF)
        {
            this->generate_error(token.m_line, " at end: " + message);
        }
        else
        {
            this->generate_error(token.m_line, " at '" + token.m_lexeme + "' " + message);
        }

        return nullptr;
    }
    
    void Generator::generate_error(int line, const std::string& message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());
    }


    void Generator::module_initialization()
    {
        m_ctx = std::make_unique<llvm::LLVMContext>();
        m_module = std::make_unique<llvm::Module>("crap_lang", *m_ctx);

        m_builder = std::make_unique<llvm::IRBuilder<>>(*m_ctx);
    }

    Generator::Generator()
    {
        this->module_initialization();
    }
    
    Generator::~Generator(){}
}