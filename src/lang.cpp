#include <lang/lang.hpp>
#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <generator/generator.hpp>
#include <ast/ast.hpp> /* For "statements" variable */

#include <fstream>

namespace lang
{
    Lang::Lang()
    {
        m_lexer = std::make_unique<lang::Lexer>();
        m_parser = std::make_unique<lang::Parser>();
        m_generator = std::make_unique<lang::Generator>();
    }

    Lang::~Lang()
    {}

    int Lang::run_source_code(const char* absolute_path_of_source_code)
    {
        std::ifstream file(absolute_path_of_source_code);

        if(!file.is_open())
        {
            std::cout << "Error opening the file\n";
            return -1;
        }

        file.seekg(0, std::ios::end); /* Seek to end of file */
        auto file_size = file.tellg();

        std::string file_content;
        file_content.resize(file_size);

        file.seekg(0, std::ios::beg);
        file.read(file_content.data(), file_size);

        file.close();
        
        /* run the file contents */
        this->run(std::move(file_content));

        return 0;
    }

    void Lang::run(std::string&& source)
    {
        /********************************************************************************************************/
        auto [tokens, tokenization_errors] = m_lexer->tokenize(std::move(source));
        
        if(tokenization_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING TOKENIZATION:\n";
            for(const auto& error: tokenization_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
        std::cout << "Successfully tokenize\n";

        /********************************************************************************************************/
        auto [statements, parsing_errors] = m_parser->parse(std::move(tokens));

        if(statements.size() == 0 || parsing_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING PARSING:\n";
            for(const auto& error: parsing_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
        std::cout << "Successfully parsed\n";
        
        /********************************************************************************************************/

        auto evaluation_errors = m_generator->generate(std::move(statements));

        if(evaluation_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING EVALUATION:\n";
            for(const auto& error: evaluation_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }

        m_generator->save_module_to_file("out.ll");
        // m_generator->print_module(); /* Print in the console */
    }
}