//===--- Diagnostics.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Diagnostics.h"

using namespace dusk;
using namespace diag;

unsigned Diagnostics::diagnose(llvm::SMDiagnostic &&D) {
    auto ID = NextID++;
    Diags.insert({ID, std::move(D)});
    return ID;
}

void Diagnostics::consume(llvm::raw_ostream &OS) {
    for (unsigned i = 1; i <= Diags.size(); i++)
        Diags[i].print("duskc", OS);
}

llvm::SMDiagnostic &Diagnostics::operator[](unsigned ID) {
    assert(ID > 0 && ID < NextID && "Invalid diagnostic ID.");
    return Diags[ID];
}
