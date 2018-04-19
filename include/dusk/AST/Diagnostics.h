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
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

namespace dusk {

namespace diag {

enum struct LexerError {
    unexpected_symbol,
    missing_eol_multiline_comment
};

enum struct ParserError {
    // General
    unexpected_token,
    missing_semicolon,
    missing_r_paren,

    // Decl
    missing_identfier,
    missing_assign,

    // Stmt
    missing_in_kw,
    missing_r_brace,
    missing_ellipsis_op,

    // Pattern
    missing_colon,
    missing_r_bracket
};

/// Aggregates diagnostics.
class Diagnostics {
    llvm::DenseMap<unsigned, llvm::SMDiagnostic> Diags;
    
    /// ID of next diagnostic.
    unsigned NextID = 1;
    
public:
    Diagnostics() = default;
    bool isEmpty() const { return Diags.size() == 0; }
    
    /// Create a diagnosis.
    unsigned diagnose(llvm::SMDiagnostic &&D);
    
    /// Consumes a all diagnosis.
    void consume(llvm::raw_ostream &OS);
    
    /// Returns a diagnostics with given ID.
    llvm::SMDiagnostic &operator[](unsigned ID);
};

} // namespace diag

} // namespace dusk

#endif /* DUSK_DIAGNOSTICS_H */
