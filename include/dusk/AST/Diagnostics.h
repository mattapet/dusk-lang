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

#include "dusk/AST/DiagnosticsParse.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

namespace dusk {
class Decl;
class Diagnostic;
class DiagnosticRef;
class DiagnosticEngine;

namespace diag {

enum DiagID : unsigned;

} // namespace diag

/// Interface, which all diagnostic consumers must implement.
class DiagnosticConsumer {
public:
  /// Consumes a single diagnotic.
  virtual void consume(SMDiagnostic &Diagnostic) = 0;
};

/// Represents a single diagnostic.
///
/// This is a container object holding all necessary information to create
/// a \c SMDiagnostic for a diagnostic consumer.
class Diagnostic {
  diag::DiagID ID;
  SmallVector<SMFixIt, 2> FixIts;
  SMLoc SourceLoc;

  friend class DiagnosticEngine;

  /// Creates a diagnostic. This constructor is only available to \c
  /// DiagnosticEngine.
  Diagnostic(diag::DiagID ID) : ID(ID) {}

public:
  // Accessors

  diag::DiagID getID() const { return ID; }
  ArrayRef<SMFixIt> getFixIts() const { return FixIts; }
  SMLoc getLoc() const { return SourceLoc; }

  /// Sets the default location of the diagnostic.
  void setLoc(SMLoc Loc) { SourceLoc = Loc; }

  /// Adds a \c FixIt to the diagnostic.
  void addFixIt(SMFixIt &&FixIt) { FixIts.push_back(std::move(FixIt)); }
};

/// Reference interface to a diagnostic, which is currently active within the
/// diagnostic engine.
///
/// Only a single reference to a diagnostic can be active at a time. Diagnostic
/// can be emitted explicitly by calling a \c flush method or implicitly by
/// going out of scope.
class DiagnosticRef {
  friend class DiagnosticEngine;

  DiagnosticEngine *Engine;
  bool IsActive;

  /// Creates a single reference to an active diagnostic.
  ///
  /// This constructor is available only ot \c DiagnosticEngine.
  DiagnosticRef(DiagnosticEngine &E) : Engine(&E), IsActive(true) {}

  DiagnosticRef(const DiagnosticRef &other) = delete;
  DiagnosticRef &operator=(const DiagnosticRef &other) = delete;
  DiagnosticRef &operator=(DiagnosticRef &&other) = delete;

public:
  /// Create an empty, but active reference.
  ///
  /// Empty reference is not attached to a diagnostic engine, therefore does not
  /// emit any diagnostic, however it acts as a regular diagnostic reference.
  DiagnosticRef() : Engine(nullptr), IsActive(true) {}

  /// Transfering refernce objects.
  DiagnosticRef(DiagnosticRef &&Other)
      : Engine(Other.Engine), IsActive(Other.IsActive) {}

  ~DiagnosticRef() {
    if (IsActive)
      flush();
  }

  /// Flushes referenced diagnotic.
  void flush();

  /// Adds a fixit.
  ///
  /// A fixit will reference a provided \c Loc.
  DiagnosticRef &fixIt(StringRef FixIt, SMLoc Loc);

  /// Adds a fixit before a token located at provided location.
  DiagnosticRef &fixItBefore(StringRef FixIt, SMLoc Loc);

  /// Adds a fixit after a token located at provided location.
  DiagnosticRef &fixItAfter(StringRef FixIt, SMLoc Loc);
};

/// This class is acts as a pipeline between custom diagnostic objects and
/// the diagnostic consumers which consume standart \c SMDiagnostic
/// objects.
class DiagnosticEngine {
  llvm::SourceMgr &SourceManager;

  /// Consumers of diagnostics
  std::vector<DiagnosticConsumer *> Consumers;

  /// Currently active diagnostic
  Optional<Diagnostic> ActiveDiag;

  friend class Diagnostic;
  friend class DiagnosticRef;

public:
  DiagnosticEngine(llvm::SourceMgr &SM) : SourceManager(SM), ActiveDiag() {}

  /// Adds another \c DiagnosticConsumer.
  void addConsumer(DiagnosticConsumer *C) { Consumers.push_back(C); }

  /// Remove and return all \c DiagnosticConsumers.
  std::vector<DiagnosticConsumer *> takeConsumers() {
    auto Ret =
        std::vector<DiagnosticConsumer *>(Consumers.begin(), Consumers.end());
    Consumers.clear();
    return Ret;
  }

  /// \brief Create and emit a single diagnostic.
  ///
  /// \param SourceLoc Location to which the diagnostic referes in the source
  /// code.
  ///
  /// \param ID The diagnostic ID.
  ///
  /// \return A \c DiagnosticRef object, which is an interface referencing
  /// created diagnostic. User can add additional information via this
  /// diagnostic reference.
  DiagnosticRef diagnose(SMLoc SourceLoc, diag::DiagID ID) {
    assert(!ActiveDiag && "Cannot have two active diagnostics at one.");

    ActiveDiag = Diagnostic(ID);
    ActiveDiag->setLoc(SourceLoc);
    return DiagnosticRef(*this);
  }

private:
  /// Return active diagnostic.
  Diagnostic &getActiveDiag() { return *ActiveDiag; }

  /// Flushes active diagnostic
  void flushActiveDiag();

  /// Emits given diagnostic to all registered consumers.
  void emitDiagnostic(const Diagnostic &Diag);
};

} // namespace dusk

#endif /* DUSK_DIAGNOSTICS_H */
