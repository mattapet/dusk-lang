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

  friend super;
  
  /// AST pretty printer.
  ASTPrinter &Printer;

public:
  PrintAST(ASTPrinter &P) : Printer(P) {}

  // MARK: - Declaration nodes

  bool visitModuleDecl(ModuleDecl *D) {
    Printer.printText(D->getName());
    Printer.printNewline();

    for (auto N : D->getContents()) {
      auto E = dynamic_cast<Expr *>(N);
      if (E)
        Printer.printNewline();
      
      if (auto D = dynamic_cast<Decl *>(N))
        super::visit(D);
      else if (E != nullptr)
        super::visit(E);
      else if (auto S = dynamic_cast<Stmt *>(N))
        super::visit(S);
      else
        llvm_unreachable("Unexpected node");
      
      if (E)
        Printer.printText(";");
    }
    Printer.printNewline();
    return true;
  }

  bool visitLetDecl(LetDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName();
    
    if (D->hasTypeRepr()) {
      Printer << ": ";
      super::visit(D->getTypeRepr());
    }
    
    if (D->hasValue()) {
      Printer << " " << tok::assign << " ";
      super::visit(D->getValue());
    }
    Printer.printDeclPost(D);
    return true;
  }

  bool visitVarDecl(VarDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName();
    
    if (D->hasTypeRepr()) {
      Printer << ": ";
      super::visit(D->getTypeRepr());
    }
    
    if (D->hasValue()) {
      Printer << " " << tok::assign << " ";
      super::visit(D->getValue());
    }
    Printer.printDeclPost(D);
    return true;
  }

  bool visitFuncDecl(FuncDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName() << "(";
    super::visit(D->getArgs());
    Printer << ")";
    
    if (D->hasTypeRepr()) {
      Printer << " -> ";
      super::visit(D->getTypeRepr());
    }
    
    Printer.printDeclPost(D);
    return true;
  }

  bool visitParamDecl(ParamDecl *D) {
    Printer.printDeclPost(D);
    Printer << D->getName() << ": ";
    if (D->hasTypeRepr())
      super::visit(D->getTypeRepr());
    Printer.printDeclPost(D);
    return true;
  }

  // MARK: - Expression nodes

  Expr *visitNumberLiteralExpr(NumberLiteralExpr *E) {
    auto St = E->getLocStart().getPointer();
    auto En = E->getLocEnd().getPointer();
    StringRef Str = {St, (size_t)(En - St)};
    Printer << Str;
    return E;
  }
  
  Expr *visitArrayLiteralExpr(ArrayLiteralExpr *E) {
    Printer << "[";
    super::visit(E->getValues());
    Printer << "]";
    return E;
  }
  
  Expr *visitIdentifierExpr(IdentifierExpr *E) {
    Printer << E->getName();
    return E;
  }

  Expr *visitParenExpr(ParenExpr *E) {
    Printer << "(";
    super::visit(E->getExpr());
    Printer << ")";
    return E;
  }

  Expr *visitAssignExpr(AssignExpr *E) {
    super::visit(E->getDest());
    Printer << " " << tok::assign << " ";
    super::visit(E->getSource());
    return E;
  }

  Expr *visitCallExpr(CallExpr *E) {
    super::visit(E->getCallee());
    Printer << "(";
    super::visit(E->getArgs());
    Printer << ")";
    return E;
  }

  Expr *visitInfixExpr(InfixExpr *E) {
    super::visit(E->getLHS());
    Printer << " " << E->getOp().getKind() << " ";
    super::visit(E->getRHS());
    return E;
  }

  Expr *visitPrefixExpr(PrefixExpr *E) {
    Printer << E->getOp().getKind();
    super::visit(E->getDest());
    return E;
  }

  Expr *visitSubscriptExpr(SubscriptExpr *E) {
    super::visit(E->getBase());
    super::visit(E->getSubscript());
    return E;
  }

  // MARK: - Statement nodes

  bool visitBreakStmt(BreakStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwBreak;
    Printer.printStmtPost(S);
    return true;
  }

  bool visitReturnStmt(ReturnStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kwReturn << " ";
    super::visit(S->getValue());

    Printer.printStmtPost(S);
    return true;
  }

  bool visitSubscriptStmt(SubscriptStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getValue());
    Printer.printStmtPost(S);
    return true;
  }

  bool visitRangeStmt(RangeStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getStart());

    Printer << (S->isInclusive() ? tok::elipsis_incl : tok::elipsis_excl);
    
    super::visit(S->getEnd());
    Printer.printStmtPost(S);
    return true;
  }

  bool visitBlockStmt(BlockStmt *S) {
    Printer.printStmtPre(S);
    bool IsFirst = true;
    for (auto N : S->getNodes()) {
      if (IsFirst)
        IsFirst = false;
      else
        Printer.printNewline();
      
      auto E = dynamic_cast<Expr *>(N);

      if (auto D = dynamic_cast<Decl *>(N))
        super::visit(D);
      else if (E != nullptr)
        super::visit(E);
      else if (auto S = dynamic_cast<Stmt *>(N))
        super::visit(S);
      else
        llvm_unreachable("Unexpected node");
      
      if (E)
        Printer.printText(";");
    }
    Printer.printStmtPost(S);
    return true;
  }

  bool visitExternStmt(ExternStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwExtern << " ";
    super::visit(S->getPrototype());
    Printer.printStmtPost(S);
    return true;
  }
  
  bool visitFuncStmt(FuncStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getPrototype());
    Printer << " ";
    super::visit(S->getBody());
    Printer.printStmtPost(S);
    return true;
  }
  
  bool visitForStmt(ForStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kwFor << " ";
    super::visit(S->getIter());
    Printer << " " << tok::kwIn << " ";
    super::visit_(S->getRange());
    Printer << " ";
    super::visit(S->getBody());

    Printer.printStmtPost(S);
    return true;
  }

  bool visitWhileStmt(WhileStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kwWhile << " ";
    
    super::visit_(S->getCond());
    Printer << " ";
    super::visit(S->getBody());
    
    Printer.printStmtPost(S);
    return true;
  }
  
  bool visitIfStmt(IfStmt *S) {
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

  // MARK: - Pattern nodes

  bool visitExprPattern(ExprPattern *P) {
    bool isFirst = true;
    for (auto V : P->getValues()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit(V);
    }
    return true;
  }

  bool visitVarPattern(VarPattern *P) {
    bool isFirst = true;
    for (auto V : P->getVars()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit(V);
    }
    return true;
  }
  
  // MARK: - Type representations
  
  bool visitIdentTypeRepr(IdentTypeRepr *T) {
    Printer << T->getIdent();
    return true;
  }
  
  bool visitArrayTypeRepr(ArrayTypeRepr *T) {
    super::visit(T->getBaseTyRepr());
    super::visit(T->getSize());
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
  p.ASTVisitor::visit(Ctx.getRootModule());
}

