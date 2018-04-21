//===--- Diagnostics.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Diagnostics.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Basic/SourceManager.h"
#include <iostream>

using namespace dusk;

// MARK: - Diagnostic reference

void DiagnosticRef::flush() {
  // Ensure the current diagnostic is active.
  if (!IsActive)
    return;

  IsActive = false;
  if (Engine)
    Engine->flushActiveDiag();
}

DiagnosticRef &DiagnosticRef::fixIt(llvm::StringRef FixIt, llvm::SMLoc Loc) {
  // Verify that the referenced diagnostic is till active.
  assert(IsActive && "Cannot modify inactive diagnostic.");
  // Add location iff there is an engine and the location is valid.
  if (Engine && Loc.isValid())
    Engine->getActiveDiag().addFixIt({Loc, FixIt});
  return *this;
}

DiagnosticRef &DiagnosticRef::fixItBefore(llvm::StringRef FixIt,
                                          llvm::SMLoc Loc) {
  // Add location iff there is an engine and the location is valid.
  if (Engine && Loc.isValid()) {
    auto T = Lexer::getTokenAtLocation(Engine->SourceManager, Loc);
    auto L = T.getLoc();
    return fixIt(FixIt, L);
  }
  return *this;
}

DiagnosticRef &DiagnosticRef::fixItAfter(llvm::StringRef FixIt,
                                         llvm::SMLoc Loc) {
  // Add location iff there is an engine and the location is valid.
  if (Engine && Loc.isValid()) {
    auto L = Lexer::getLocForEndOfToken(Engine->SourceManager, Loc);
    return fixIt(FixIt, L);
  }
  return *this;
}

// MARK: - Diagnostic engine

void DiagnosticEngine::flushActiveDiag() {
  assert(ActiveDiag && "No active diagnostic to flush.");
  emitDiagnostic(*ActiveDiag);
  ActiveDiag.reset();
}

void DiagnosticEngine::emitDiagnostic(const Diagnostic &Diag) {
  auto Loc = Diag.getLoc();
  auto ID = getBufferForLoc(SourceManager, Loc);
  auto FN = SourceManager.getMemoryBuffer(ID)->getBufferIdentifier();
  auto[L, C] = SourceManager.getLineAndColumn(Loc);
  auto K = llvm::SourceMgr::DiagKind::DK_Error;
  auto Line = Lexer::getLineForLoc(SourceManager, Loc);
  auto Msg = diag::getTextForID(Diag.getID());
  auto D = llvm::SMDiagnostic(SourceManager, Loc, FN, L, C, K, Msg, Line,
                              llvm::None, Diag.getFixIts());

  for (auto C : Consumers) {
    C->consume(D);
  }
}
