//===--- CompilerInstance.h - Dusk compiler configuration -----*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/ASTContext.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Frontend/CompilerInvocation.h"
#include "dusk/Frontend/SourceFile.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/SourceMgr.h"
#include <memory>

#ifndef DUSK_COMPILER_INSTANCE_H
#define DUSK_COMPILER_INSTANCE_H

namespace dusk {
class ModuleDecl;

/// Encapsulation of compiler state and execution.
class CompilerInstance : public DiagnosticConsumer {
  CompilerInvocation Invocation;
  SourceMgr SourceManager;
  DiagnosticEngine Diag{SourceManager};
  
  /// Compilation context.
  std::unique_ptr<ASTContext> Context;

  /// Main compilation module
  ModuleDecl *MainModule = nullptr;

public:
  /// Constructs a default compiler instance.
  CompilerInstance();

  /// Retuns compilers source manager.
  SourceMgr &getSourceManager() { return SourceManager; }

  /// Returns compilers diagnostics.
  DiagnosticEngine &getDiags() { return Diag; }

  /// Returns AST context of current compilation
  ASTContext &getContext() { return *Context; }

  /// Returns \c true, if copmiler instance has an AST context, \c false
  /// otherwise.
  bool hasASTContext() const { return Context != nullptr; }

  /// Retuns parsed file as a module.
  ModuleDecl *getModule();

  /// Returns the input file.
  SourceFile *getInputFile();

  /// Compiles a source file.
  void performCompilation();
  
  /// Parses file and performs a semantic analysis.
  void performSema();

  /// Parses input file without performing any semantic analysis.
  void performParseOnly();
  
  /// Free AST context to enable reuse of current compiler instance.
  void freeContext();
  
  /// Reset compiler instance with new configuration.
  void reset(CompilerInvocation &&I);
  
  virtual void consume(SMDiagnostic &Diagnostic);
  
private:
  // Explicitly forbid copying of any kind.
  CompilerInstance(const CompilerInstance &other) = delete;
  CompilerInstance &operator=(const CompilerInstance &other) = delete;
  CompilerInstance(CompilerInstance &&other) = delete;
  CompilerInstance &operator=(CompilerInstance &&other) = delete;
};

} // namespace dusk

#endif /* DUSK_COMPILER_INSTANCE_H */
