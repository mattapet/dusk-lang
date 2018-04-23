//===--- ASTWalker.cpp ----------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTPrinter.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/ASTVisitor.h"
#include "dusk/Parse/TokenDefinition.h"
#include "dusk/Frontend/Formatter.h"

using namespace dusk;

namespace {

/// AST pretty printing.
class PrintAST : public ASTVisitor<PrintAST> {
  /// Convenience type alias
  typedef ASTVisitor<PrintAST> super;

  /// AST pretty printer.
  ASTPrinter &Printer;

public:
  PrintAST(ASTPrinter &P) : Printer(P) {}

  // MARK: - Declaration nodes

  bool visit(ModuleDecl *D) {
    Printer.printText(D->getName());
    Printer.printNewline();

    for (auto N : D->getContents())
      super::visit(N);
    Printer.printNewline();
    return true;
  }

  bool visit(ConstDecl *D) {
    Printer.printDeclPre(D);

    Printer << D->getName() << " " << tok::assign << " ";

    super::visit(D->getValue());
    Printer.printDeclPost(D);
    return true;
  }

  bool visit(VarDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName() << " " << tok::assign << " ";
    super::visit(D->getValue());
    Printer.printDeclPost(D);
    return true;
  }

  bool visit(FuncDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName();
    super::visit(D->getArgs());
    Printer.printDeclPost(D);
    return true;
  }

  bool visit(ParamDecl *D) {
    Printer.printDeclPost(D);
    Printer << D->getName();
    Printer.printDeclPost(D);
    return true;
  }

  // MARK: - Expression nodes

  bool visit(IdentifierExpr *E) {
    Printer << E->getName();
    return true;
  }

  bool visit(NumberLiteralExpr *E) {
    auto St = E->getLocStart().getPointer();
    auto En = E->getLocEnd().getPointer();
    StringRef Str = {St, (size_t)(En - St)};
    Printer << Str;
    return true;
  }

  bool visit(ParenExpr *E) {
    Printer << "(";
    super::visit(E->getExpr());
    Printer << ")";
    return true;
  }

  bool visit(AssignExpr *E) {
    super::visit(E->getDest());
    Printer << " " << tok::assign << " ";
    super::visit(E->getSource());
    return true;
  }

  bool visit(CallExpr *E) {
    super::visit(E->getCalle());
    super::visit(E->getArgs());
    return true;
  }

  bool visit(InfixExpr *E) {
    super::visit(E->getLHS());
    Printer << " " << E->getOp().getKind() << " ";
    super::visit(E->getRHS());
    return true;
  }

  bool visit(PrefixExpr *E) {
    Printer << E->getOp().getKind();
    super::visit(E->getDest());
    return true;
  }

  bool visit(SubscriptExpr *E) {
    super::visit(E->getBase());
    super::visit(E->getSubscript());
    return true;
  }

  // MARK: - Statement nodes

  bool visit(BreakStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwBreak;
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(ReturnStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kwReturn << " ";
    super::visit(S->getValue());

    Printer.printStmtPost(S);
    return true;
  }

  bool visit(SubscriptStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getValue());
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(RangeStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getStart());

    Printer << S->getOp().getKind();

    super::visit(S->getEnd());
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(BlockStmt *S) {
    Printer.printStmtPre(S);
    bool IsFirst = true;
    for (auto N : S->getNodes()) {
      if (IsFirst)
        IsFirst = false;
      else
        Printer.printNewline();

      super::visit(N);
      if (auto E = dynamic_cast<Expr *>(N))
        Printer.printText(";");
    }
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(ForStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kwFor << " ";
    super::visit(S->getVar());
    Printer << " " << tok::kwIn << " ";
    super::visit(S->getRange());
    Printer << " ";
    super::visit(S->getBody());

    Printer.printStmtPost(S);
    return true;
  }

  bool visit(FuncStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getPrototype());
    Printer << " ";
    super::visit(S->getBody());
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(IfStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwIf << " ";

    super::visit(S->getCond());
    Printer << " ";
    super::visit(S->getThen());

    if (S->hasElseBlock()) {
      Printer << " " << tok::kwElse << " ";
      super::visit(S->getElse());
    }

    Printer.printStmtPost(S);
    return true;
  }

  bool visit(WhileStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwWhile << " ";

    super::visit(S->getCond());
    Printer << " ";
    super::visit(S->getBody());

    Printer.printStmtPost(S);
    return true;
  }

  // MARK: - Pattern nodes

  bool visit(ExprPattern *P) {
    Printer.printPatternPre(P);
    bool isFirst = true;
    for (auto V : P->getValues()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit(V);
    }
    Printer.printPatternPost(P);
    return true;
  }

  bool visit(VarPattern *P) {
    Printer.printPatternPre(P);
    bool isFirst = true;
    for (auto V : P->getVars()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit(V);
    }
    Printer.printPatternPost(P);
    return true;
  }
};

/// Implementation of an \c ASTPrinter, which is used to pretty print the AST.
class PrettyPrinter : public StreamPrinter {
public:
  // Use StreamPrinter constructor.
  using StreamPrinter::StreamPrinter;

  virtual void printDeclPre(Decl *D) override {
    tok KW;
    switch (D->getKind()) {
    case DeclKind::Const:
      KW = tok::kwConst;
      break;
    case DeclKind::Var:
      KW = tok::kwVar;
      break;
    case DeclKind::Func:
      KW = tok::kwFunc;
      printNewline();
      break;
    default:
      return;
    }

    if (!isAtStartOfLine())
      printNewline();
    *this << KW << " ";
  }

  virtual void printDeclPost(Decl *D) override {
    switch (D->getKind()) {
    case DeclKind::Const:
    case DeclKind::Var:
      return printText(";");
    default:
      return;
    }
  }

  virtual void printStmtPre(Stmt *S) override {
    switch (S->getKind()) {
    case StmtKind::Block:
      *this << tok::l_brace;
      ++(*this);
      break;
    case StmtKind::For:
    case StmtKind::Func:
    case StmtKind::If:
    case StmtKind::While:
      if (!isAtStartOfLine())
        printNewline();
      return;
    case StmtKind::Subscript:
      *this << "[";
      break;
    default:
      return;
    }
  }

  virtual void printStmtPost(Stmt *S) override {
    switch (S->getKind()) {
    case StmtKind::Block:
      --(*this);
      *this << tok::r_brace;
      break;
    case StmtKind::Break:
    case StmtKind::Return:
      *this << ";";
      break;
    case StmtKind::Subscript:
      *this << "]";
      break;
    default:
      return;
    }
  }

  virtual void printPatternPre(Pattern *P) override {
    switch (P->getKind()) {
    case PatternKind::Variable:
    case PatternKind::Expr:
      *this << "(";
      break;
    }
  }

  virtual void printPatternPost(Pattern *P) override {
    switch (P->getKind()) {
    case PatternKind::Variable:
    case PatternKind::Expr:
      *this << ")";
      break;
    }
  }
};

} // anonymous namespace

// MARK: - Indentation

Indentation::Indentation(unsigned SL) : Indentation(IndKind::Space, SL) {}

Indentation::Indentation(IndKind K, unsigned SL) : Kind(K), SpaceLen(SL) {}

StringRef Indentation::getIndent() const {
  SmallString<MAX_SPACE_LEN * MAX_INDENTATION> Str;
  for (unsigned i = 0; i < Depth; i++)
    Str += getIndentBlock();
  return Str;
}

StringRef Indentation::getSpaceIndent() const {
  assert(Kind == IndKind::Space && "Invalid indentation.");
  SmallString<MAX_SPACE_LEN> Str;
  for (unsigned i = 0; i < SpaceLen; i++)
    Str += " ";
  return Str;
}

StringRef Indentation::getTabIndent() const {
  assert(Kind == IndKind::Space && "Invalid indentation.");
  return "\t";
}

StringRef Indentation::getIndentBlock() const {
  if (Kind == IndKind::Space)
    return getSpaceIndent();
  else
    return getTabIndent();
}

Indentation &Indentation::operator++() {
  assert(Depth < MAX_INDENTATION - 1 && "Max indentation exceeded.");
  Depth++;
  return *this;
}

Indentation &Indentation::operator--() {
  assert(Depth != 0 && "Unexpected zero indentation.");
  Depth--;
  return *this;
}

// MARK: - AST Printer

void ASTPrinter::printIndent() {
  AtStartOfLine = false;
  printText(Ind);
}

void ASTPrinter::printNewline() {
  AtStartOfLine = true;
  printText(Newline);
}

void ASTPrinter::printSeparator(bool &IsFirst, StringRef S) {
  if (IsFirst)
    IsFirst = false;
  else
    printText(S);
}

ASTPrinter &ASTPrinter::operator<<(StringRef Text) {
  print(Text);
  return *this;
}

ASTPrinter &ASTPrinter::operator<<(uint64_t N) {
  SmallString<32> Str;
  llvm::raw_svector_ostream OS(Str);
  OS << N;
  print(OS.str());
  return *this;
}

ASTPrinter &ASTPrinter::operator<<(tok T) {
  SmallString<16> Buffer;
  llvm::raw_svector_ostream OS(Buffer);
  OS << T;
  print(OS.str());
  return *this;
}

ASTPrinter &ASTPrinter::operator++() {
  ++Ind;
  printNewline();
  return *this;
}

ASTPrinter &ASTPrinter::operator--() {
  --Ind;
  printNewline();
  return *this;
}

void ASTPrinter::printKeyword(StringRef KW) { *this << KW; }

void ASTPrinter::print(StringRef Text) {
  if (isAtStartOfLine())
    printIndent();
  printText(Text);
}

// MARK: - StreamPrinter

StreamPrinter::StreamPrinter(raw_ostream &OS) : OS(OS) {}

void StreamPrinter::printText(StringRef Text) { OS << Text; }

// MARK: - Formatter

void Formatter::format(ASTNode *N, raw_ostream &OS) {
  PrettyPrinter pp(OS);
  PrintAST p(pp);
  p.ASTVisitor::visit(N);
}

