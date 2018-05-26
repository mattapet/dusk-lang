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
#include "dusk/Basic/TokenDefinitions.h"
#include "dusk/Frontend/Formatter.h"

using namespace dusk;

namespace llvm {
llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, tok keyword) {
  switch (keyword) {
#define KEYWORD(KW)                                                            \
  case tok::kw_##KW:                                                           \
    OS << #KW;                                                                 \
    break;
#define PUNCTUATOR(PUN, TEXT)                                                  \
  case tok::PUN:                                                               \
    OS << TEXT;                                                                \
    break;
#include "dusk/Basic/TokenDefinitions.def"
  default:
    llvm_unreachable("unexpected keyword or punctuator kind");
  }
  return OS;
}
} // namespace llvm

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

  void visitModuleDecl(ModuleDecl *D) {
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
  }

  void visitVarDecl(VarDecl *D) {
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
  }

  void visitFuncDecl(FuncDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName() << "(";
    super::visit(D->getArgs());
    Printer << ")";

    if (D->hasTypeRepr()) {
      Printer << " -> ";
      super::visit(D->getTypeRepr());
    }

    Printer.printDeclPost(D);
  }

  void visitParamDecl(ParamDecl *D) {
    Printer.printDeclPre(D);
    Printer << D->getName();
    if (D->hasTypeRepr()) {
      Printer << ": ";
      super::visit(D->getTypeRepr());
    }
    Printer.printDeclPost(D);
  }

  // MARK: - Expression nodes

  void visitNumberLiteralExpr(NumberLiteralExpr *E) {
    auto St = E->getLocStart().getPointer();
    auto En = E->getLocEnd().getPointer();
    StringRef Str = {St, (size_t)(En - St)};
    Printer << Str;
  }

  void visitArrayLiteralExpr(ArrayLiteralExpr *E) {
    Printer << "[";
    super::visit(E->getValues());
    Printer << "]";
  }

  void visitIdentifierExpr(IdentifierExpr *E) { Printer << E->getName(); }

  void visitInOutExpr(InOutExpr *E) {
    Printer << "&";
    super::visit(E->getBase());
  }
  
  void visitParenExpr(ParenExpr *E) {
    Printer << "(";
    super::visit(E->getExpr());
    Printer << ")";
  }

  void visitAssignExpr(AssignExpr *E) {
    super::visit(E->getDest());
    Printer << " " << tok::assign << " ";
    super::visit(E->getSource());
  }

  void visitCallExpr(CallExpr *E) {
    super::visit(E->getCallee());
    Printer << "(";
    super::visit(E->getArgs());
    Printer << ")";
  }

  void visitInfixExpr(InfixExpr *E) {
    super::visit(E->getLHS());
    Printer << " " << E->getOp().getKind() << " ";
    super::visit(E->getRHS());
  }

  void visitPrefixExpr(PrefixExpr *E) {
    Printer << E->getOp().getKind();
    super::visit(E->getDest());
  }

  void visitSubscriptExpr(SubscriptExpr *E) {
    super::visit(E->getBase());
    super::visit(E->getSubscript());
  }

  // MARK: - Statement nodes

  void visitBreakStmt(BreakStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kw_break;
    Printer.printStmtPost(S);
  }

  void visitReturnStmt(ReturnStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kw_return << " ";
    super::visit(S->getValue());

    Printer.printStmtPost(S);
  }

  void visitSubscriptStmt(SubscriptStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getValue());
    Printer.printStmtPost(S);
  }

  void visitRangeStmt(RangeStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getStart());

    Printer << (S->isInclusive() ? tok::elipsis_incl : tok::elipsis_excl);

    super::visit(S->getEnd());
    Printer.printStmtPost(S);
  }

  void visitBlockStmt(BlockStmt *S) {
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
  }

  void visitExternStmt(ExternStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kw_extern << " ";
    super::visit(S->getPrototype());
    Printer.printStmtPost(S);
  }

  void visitFuncStmt(FuncStmt *S) {
    Printer.printStmtPre(S);
    super::visit(S->getPrototype());
    Printer << " ";
    super::visit(S->getBody());
    Printer.printStmtPost(S);
  }

  void visitForStmt(ForStmt *S) {
    Printer.printStmtPre(S);

    Printer << tok::kw_for << " ";
    super::visit(S->getIter());
    Printer << " " << tok::kw_in << " ";
    super::visit(S->getRange());
    Printer << " ";
    super::visit(S->getBody());

    Printer.printStmtPost(S);
  }

  void visitWhileStmt(WhileStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kw_while << " ";

    super::visit(S->getCond());
    Printer << " ";
    super::visit(S->getBody());

    Printer.printStmtPost(S);
  }

  void visitIfStmt(IfStmt *S) {
    Printer.printStmtPre(S);
    Printer << tok::kw_if << " ";

    super::visit(S->getCond());
    Printer << " ";
    super::visit(S->getThen());

    if (S->hasElseBlock()) {
      Printer << " " << tok::kw_else << " ";
      super::visit(S->getElse());
    }

    Printer.printStmtPost(S);
  }

  // MARK: - Pattern nodes

  void visitExprPattern(ExprPattern *P) {
    bool isFirst = true;
    for (auto V : P->getValues()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit(V);
    }
  }

  void visitVarPattern(VarPattern *P) {
    bool isFirst = true;
    for (auto V : P->getVars()) {
      Printer.printSeparator(isFirst, ", ");
      super::visit(V);
    }
  }

  // MARK: - Type representations

  void visitIdentTypeRepr(IdentTypeRepr *T) { Printer << T->getIdent(); }

  void visitArrayTypeRepr(ArrayTypeRepr *T) {
    super::visit(T->getBaseTyRepr());
    super::visit(T->getSize());
  }
  
  void visitInOutTypeRepr(InOutTypeRepr *TR) {
    Printer << tok::kw_inout << " ";
    super::visit(TR->getBaseTyRepr());
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

    case DeclKind::Var: {
      if (!isAtStartOfLine())
        printNewline();
      KW = D->getVarDecl()->isLet() ? tok::kw_let : tok::kw_var;
      break;
    }
        
    case DeclKind::Param: {
      if (D->getParamDecl()->isInOut())
        KW = tok::kw_inout;
      else
        return;
      break;
    }

    case DeclKind::Func:
      KW = tok::kw_func;
      break;
    default:
      return;
    }
    *this << KW << " ";
  }

  virtual void printDeclPost(Decl *D) override {
    if (D->isKind(DeclKind::Var))
      return printText(";");
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
    case PatternKind::Var:
    case PatternKind::Expr:
      break;
    }
  }

  virtual void printPatternPost(Pattern *P) override {
    switch (P->getKind()) {
    case PatternKind::Var:
    case PatternKind::Expr:
      break;
    }
  }
};

} // anonymous namespace

// MARK: - Indentation

Indentation::Indentation(unsigned SL) : Indentation(IndKind::Space, SL) {}

Indentation::Indentation(IndKind K, unsigned SL) : Kind(K), SpaceLen(SL) {}

std::string Indentation::getIndent() const {
  std::string Str;
  for (unsigned i = 0; i < Depth; i++)
    Str += getIndentBlock();
  return Str;
}

std::string Indentation::getSpaceIndent() const {
  assert(Kind == IndKind::Space && "Invalid indentation.");
  std::string Str;
  for (unsigned i = 0; i < SpaceLen; i++)
    Str += " ";
  return Str;
}

std::string Indentation::getTabIndent() const {
  assert(Kind == IndKind::Tab && "Invalid indentation.");
  return "\t";
}

std::string Indentation::getIndentBlock() const {
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

ASTPrinter::ASTPrinter(const Indentation &I) : Ind(I) {}

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

StreamPrinter::StreamPrinter(raw_ostream &OS, const Indentation &I)
  : ASTPrinter(I), OS(OS) {}

void StreamPrinter::printText(StringRef Text) { OS << Text; }

// MARK: - Formatter

void Formatter::format() {
  PrettyPrinter pp(OS, {IndentKind, IndentSize});
  PrintAST(pp).ASTVisitor::visit(Ctx.getRootModule());
}
