//===--- Diagnostics.h - Dusk language diagnostics engine -------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_DIAGNOSTICS_H
#define DUSK_DIAGNOSTICS_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"

namespace dusk {
    
namespace diag {


    class Diagnostic {
        unsigned DiagID;
        llvm::SMLoc SourceLoc;
    };


    class DiagnosticEngine {
        llvm::SourceMgr &SourceManager;
        llvm::DenseMap<unsigned, Diagnostic> Diagnostics;

    public:
        DiagnosticEngine(llvm::SourceMgr &SM)
        : SourceManager(SM) {}

        void diagnose(const Diagnostic &diagnostic) {}
    };

} // namespace diag
    
} // namespace dusk

#endif /* DUSK_DIAGNOSTICS_H */
