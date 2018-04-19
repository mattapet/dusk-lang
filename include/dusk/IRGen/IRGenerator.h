//===--- IRGenerator.h - Dusk IR generator ----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IR_GENERATOR_H
#define DUSK_IR_GENERATOR_H

#include "dusk/AST/ASTWalker.h"

namespace dusk {
namespace irgen {
    
    class IRGenerator: public ASTWalker {
        
    };
    
} // namespace ir
    
} // namespace dusk

#endif /* DUSK_IR_GENERATOR_H */
