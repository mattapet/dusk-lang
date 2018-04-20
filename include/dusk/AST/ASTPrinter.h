//===--- ASTPrinter.h - Class for printing parsed AST -----------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_PRINTER_H
#define DUSK_AST_PRINTER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"

namespace dusk {
class Decl;
class Expr;
class Stmt;
class Pattern;
class Indentation;
class Token;
enum struct tok;

/// Describes indentation type
enum struct IndKind { Space, Tab };

/// Encapsulates current indentation context.
///
/// Contains an interface to easily access, modify and use indentation depth
/// and other identation data like current block and indentation size.
///
/// Contains implicit convertions to \c llvm::StringRef, which acts as
/// \c getIndect() call and \c unsigned, which acts as \c getDepth() call.
class Indentation {
  /// Type of indentation
  const IndKind Kind = IndKind::Space;

  /// Indentation block size
  const unsigned SpaceLen = 4;

  /// Indentation depth
  unsigned Depth = 0;

public:
  /// Maximal indentation depth
  static const unsigned MAX_INDENTATION = 64;
  /// Maximal block size
  static const unsigned MAX_SPACE_LEN = 8;

  Indentation() = default;
  Indentation(unsigned SL);
  Indentation(IndKind K, unsigned SL);

  /// Returns indentation type.
  IndKind getKind() const { return Kind; }

  /// Returns indentation depth.
  unsigned getDepth() const { return Depth; }

  /// Returns Current indentation;
  llvm::StringRef getIndent() const;

  /// Return indentation depth.
  operator unsigned() const { return Depth; }

  /// Return indentation depth.
  operator llvm::StringRef() const { return getIndent(); }

  /// Increases indentation depth.
  Indentation &operator++();

  /// Decreases indentation depth.
  Indentation &operator--();

private:
  // Util methods
  llvm::StringRef getSpaceIndent() const;
  llvm::StringRef getTabIndent() const;
  llvm::StringRef getIndentBlock() const;
};

/// A simple abstract class, which provides a basic interface for printing AST.
/// It also acts as a cursor in the output.
class ASTPrinter {
  /// Represents current indentation
  Indentation Ind;

  /// Newline character
  llvm::StringRef Newline = "\n";

  /// True is cursor at start of the line.
  bool AtStartOfLine = true;

public:
  ASTPrinter() = default;
  ASTPrinter(llvm::StringRef NL);

  bool isAtStartOfLine() const { return AtStartOfLine; }

  /// Raw output of text without any formatting.
  virtual void printText(llvm::StringRef Text) = 0;

  /// Prints current indentation and sets \c AtStartOfLine to \c false.
  virtual void printIndent();

  /// Prints a \c Newline string and sets \c AtStartOfLine to \c true.
  virtual void printNewline();

  /// Prints a single keyword.
  virtual void printKeyword(llvm::StringRef KW);

  /// Prints given separator depending on the \c IsFirst arg, which is set
  /// to \c false, if the current element is first.
  virtual void printSeparator(bool &IsFirst, llvm::StringRef S);

  ASTPrinter &operator<<(llvm::StringRef Text);

  ASTPrinter &operator<<(uint64_t N);

  ASTPrinter &operator<<(tok T);

  /// Increases indentation.
  ASTPrinter &operator++();

  /// Decreates indentation.
  ASTPrinter &operator--();

  //===------------------------------------------------------------------------===
  //
  // Callback interface.
  //
  // Callback helper methods, which are called before and after a particular
  // node is being print.
  //
  //===------------------------------------------------------------------------===

  virtual void printDeclPre(Decl *D) {}
  virtual void printDeclPost(Decl *D) {}
  virtual void printStmtPre(Stmt *S) {}
  virtual void printStmtPost(Stmt *S) {}
  virtual void printPatternPre(Pattern *P) {}
  virtual void printPatternPost(Pattern *P) {}

private:
  void print(llvm::StringRef Text);
};

/// Simple implementation of ASTPrinter, which prints text into the provided
/// \c llvm::raw_ostream.
class StreamPrinter : public ASTPrinter {
  llvm::raw_ostream &OS;

public:
  StreamPrinter(llvm::raw_ostream &OS);
  virtual void printText(llvm::StringRef Text) override;
};

} // namespace dusk

#endif /* DUSK_AST_PRINTER_H */

