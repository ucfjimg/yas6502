#ifndef PASS1_H_
#define PASS1_H_

#include <memory>
#include <vector>

#include "opcodes.h"

namespace yas6502
{
    namespace ast 
    {
        class Node;
    }

    class SymbolTable;

    class Pass1
    {
    public:
        Pass1(SymbolTable &symtab, const OpcodeMap &opcodes);

        void pass1(std::vector<std::unique_ptr<ast::Node>> &ast);

        int loc() const;
        void setLoc(int loc);

        SymbolTable &symtab();
        std::unique_ptr<Opcode> findOpcode(const std::string &op);

        void emitByte(unsigned byte);
        void emitWord(unsigned word);

    private:
        SymbolTable &symtab_;
        const OpcodeMap &opcodes_;
        int loc_;
    };
}

#endif
