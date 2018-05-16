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
#include "dusk/AST/Type.h"
#include "dusk/AST/TypeRepr.h"
#include "dusk/AST/ASTVisitor.h"
#include "dusk/Basic/TokenDefinition.h"
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

    for (auto N : D->getContents()) {
      auto E = dynamic_cast<Expr *>(N);
      if (E)
        Printer.printNewline();
      
      super::visit_(N);
      
      if (E)
        Printer.printText(";");
    }
    Printer.printNewline();
    return true;
  }

  bool visit(LetDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName();
    
    if (D->hasTypeRepr()) {
      Printer << ": ";
      super::visit_(D->getTypeRepr());
    }
    
    if (D->hasValue()) {
      Printer << " " << tok::assign << " ";
      super::visit_(D->getValue());
    }
    Printer.printDeclPost(D);
    return true;
  }

  bool visit(VarDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName();
    
    if (D->hasTypeRepr()) {
      Printer << ": ";
      super::visit_(D->getTypeRepr());
    }
    
    if (D->hasValue()) {
      Printer << " " << tok::assign << " ";
      super::visit_(D->getValue());
    }
    Printer.printDeclPost(D);
    return true;
  }

  bool visit(FuncDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName() << "(";
    super::visit_(D->getArgs());
    Printer << ")";
    
    if (D->hasTypeRepr()) {
      Printer << " -> ";
      super::visit_(D->getTypeRepr());
    }
    
    Printer.printDeclPost(D);
    return true;
  }

  bool visit(ParamDecl *D) {
    Printer.printDeclPost(D);
    Printer << D->getName() << ": ";
    super::visit_(D->getTypeRepr());
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
  
  bool visit(ArrayLiteralExpr *E) {
    Printer << "[";
    super::visit_(E->getValues());
    Printer << "]";
    return true;
  }

  bool visit(ParenExpr *E) {
    Printer << "(";
    super::visit_(E->getExpr());
    Printer << ")";
    return true;
  }

  bool visit(AssignExpr *E) {
    super::visit_(E->getDest());
    Printer << " " << tok::assign << " ";
    super::visit_(E->getSource());
    return true;
  }

  bool visit(CallExpr *E) {
    super::visit_(E->getCallee());
    Printer << "(";
    super::visit_(E->getArgs());
    Printer << ")";
    return true;
  }

  bool visit(InfixExpr *E) {
    super::visit_(E->getLHS());
    Printer << " " << E->getOp().getKind() << " ";
    super::visit_(E->getRHS());
    return true;
  }

  bool visit(PrefixExpr *E) {
    Printer << E->getOp().getKind();
    super::visit_(E->getDest());
    return true;
  }

  bool visit(SubscriptExpr *E) {
    super::visit_(E->getBase());
    super::visit_(E->getSubscript());
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
    super::visit_(S->getValue());

    Printer.printStmtPost(S);
    return true;
  }

  bool visit(SubscriptStmt *S) {
    Printer.printStmtPre(S);
    super::visit_(S->getValue());
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(RangeStmt *S) {
    Printer.printStmtPre(S);
    super::visit_(S->getStart());

    Printer << S->getOp().getKind();

    super::visit_(S->getEnd());
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

      super::visit_(N);
      if (auto E = dynamic_cast<Expr *>(N))
        Printer.printText(";");
    }
    Printer.printStmtPost(S);
    return true;
  }

  bool visit(ExternStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwExtern << " ";
    super::visit_(S->getPrototype());
    Printer.printStmtPost(S);
    return true;
  }
  
  bool visit(FuncStmt *S) {
    Printer.printStmtPre(S);
    super::visit_(S->getPrototype());
    Printer << " ";
    super::visit_(S->getBody());
    Printer.printStmtPost(S);
    return true;
  }
  
  bool visit(ForStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kwFor << " ";
    super::visit_(S->getIter());
    Printer << " " << tok::kwIn << " ";
    super::visit_(S->getRange());
    Printer << " ";
    super::visit_(S->getBody());

    Printer.printStmtPost(S);
    return true;
  }

  bool visit(IfStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwIf << " ";

    super::visit_(S->getCond());
    Printer << " ";
    super::visit_(S->getThen());

    if (S->hasElseBlock()) {
      Printer << " " << tok::kwElse << " ";
      super::visit_(S->getElse());
    }

    Printer.printStmtPost(S);
    return true;
  }

  bool visit(WhileStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwWhile << " ";

    super::visit_(S->getCond());
    Printer << " ";
    super::visit_(S->getBody());

    Printer.printStmtPost(S);
    return true;
  }

  // MARK: - Pattern nodes

  bool visit(ExprPattern *P) {
    bool isFirst = true;
    for (auto V : P->getValues()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit_(V);
    }
    return true;
  }

  bool visit(VarPattern *P) {
    bool isFirst = true;
    for (auto V : P->getVars()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit_(V);
    }
    return true;
  }
  
  // MARK: - Type representations
  
  bool visit(IdentTypeRepr *T) {
    Printer << T->getIdent();
    return true;
  }
  
  bool visit(ArrayTypeRepr *T) {
    super::visit_(T->getBaseTyRepr());
    super::visit_(T->getSize());
    return true;
  }
};

// MARK: - Pretty Printer
  
/// Implementation of an \c ASTPrinter, which is used to pretty print the AST.
class PrettyPrinter : public StreamPrinter {
public:
  // Use StreamPrinter constructor.
  using StreamPrinter::StreamPrinter;

  virtual void printDeclPre(Decl *D) override {
    tok KW;
    switch (D->getKind()) {
    case DeclKind::Let:
      if (!isAtStartOfLine())
        printNewline();
      KW = tok::kwLet;
      break;
    case DeclKind::Var:
      if (!isAtStartOfLine())
        printNewline();
      KW = tok::kwVar;
      break;
    case DeclKind::Func:
      KW = tok::kwFunc;
      break;
    default:
      return;
    }
    *this << KW << " ";
  }

  virtual void printDeclPost(Decl *D) override {
    switch (D->getKind()) {
    case DeclKind::Let:
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
    case StmtKind::Extern:
    case StmtKind::Func:
      if (!isAtStartOfLine())
        printNewline();
      printNewline();
      break;
    case StmtKind::For:
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
    case StmtKind::Extern:
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
      break;
    }
  }

  virtual void printPatternPost(Pattern *P) override {
    switch (P->getKind()) {
    case PatternKind::Variable:
    case PatternKind::Expr:
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

void Formatter::format() {
  PrettyPrinter pp(OS);
  PrintAST p(pp);
  p.ASTVisitor::visit_(Ctx.getRootModule());
}

