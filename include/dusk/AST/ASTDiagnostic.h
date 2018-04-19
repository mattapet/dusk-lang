//===--- ASTDiagnostic.h - A diagnostic AST node ----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_DIAGNOSTIC_H
#define DUSK_AST_DIAGNOSTIC_H

#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"


namespace dusk {
class ASTNode;
    
namespace diag {
    
    class ASTDiag {
        unsigned DiagnosticID;
        
    public:
        ASTDiag(unsigned ID): DiagnosticID(ID) {}
        unsigned getDiagID() const { return DiagnosticID; }
    };
    
} // namespace diag
    
} // namespace dusk


#endif /* DUSK_AST_DIAGNOSTIC_H */
