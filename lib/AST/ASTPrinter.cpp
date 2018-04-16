//===--- ASTWalker.cpp ----------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTPrinter.h"
#include "dusk/AST/ASTVisitor.h"
#include "dusk/Parse/TokenDefinition.h"
#include "dusk/Frontend/Formatter.h"

using namespace dusk;

namespace {

/// AST pretty printing.
class PrintAST: public ASTVisitor<PrintAST> {
    /// Convenience type alias
    typedef ASTVisitor<PrintAST> super;
    
    /// AST pretty printer.
    ASTPrinter &Printer;
    
public:
    PrintAST(ASTPrinter &P): Printer(P) {}
    
    // MARK: - Declaration nodes
    
    virtual bool visit(ConstDecl *D) {
        Printer.printDeclPre(D);
        Printer << tok::kwConst << D->getName() << tok::assign;
        super::visit(D->getValue());
        Printer.printDeclPost(D);
        return true;
    }
    
    virtual bool visit(FuncDecl *D) {
        Printer.printDeclPre(D);
        Printer << tok::kwFunc << D->getName();
        super::visit(D->getArgs());
        Printer.printDeclPost(D);
        return true;
    }
    
    virtual bool visit(ParamDecl *D) {
        Printer.printDeclPost(D);
        Printer << D->getName();
        Printer.printDeclPost(D);
        return true;
    }
    
    virtual bool visit(VarDecl *D) {
        Printer.printDeclPre(D);
        Printer << tok::kwVar << D->getName() << tok::assign;
        super::visit(D->getValue());
        Printer.printDeclPost(D);
        return true;
    }
    
    
    // MARK: - Expression nodes
    
    virtual bool visit(AssignExpr *E) {
        super::visit(E->getLHS());
        Printer << tok::assign;
        super::visit(E->getRHS());
        return true;
    }
    
    virtual bool visit(FuncCall *E) {
        Printer << E->getCalle();
        super::visit(E->getArgs());
        return true;
    }
    
    virtual bool visit(InfixExpr *E) {
        super::visit(E->getLHS());
        Printer << E->getOp().getKind();
        super::visit(E->getRHS());
        return true;
    }
    
    virtual bool visit(NumberLiteralExpr *E) {
        Printer << E->getValue();
        return true;
    }
    
    virtual bool visit(VariableExpr *E) {
        Printer << E->getName();
        return true;
    }
    
    
    // MARK: - Statement nodes
    
    virtual bool visit(CodeBlock *S) {
        Printer << tok::l_brace;
        ++Printer;
        for (auto N : S->getNodes()) {
            super::visit(N);
            if (auto E = dynamic_cast<Expr *>(N))
                Printer.printText(";");
        }
        --Printer;
        Printer << tok::r_brace;
        return true;
    }
    
    virtual bool visit(ForStmt *S) {
        Printer.printStmtPre(S);
        Printer << tok::kwFor << S->getItName() << tok::kwIn;
        super::visit(S->getRange());
        super::visit(S->getBody());
        Printer.printStmtPost(S);
        return true;
    }
    
    virtual bool visit(FuncStmt *S) {
        Printer.printStmtPre(S);
        super::visit(S->getPrototype());
        super::visit(S->getBody());
        Printer.printStmtPost(S);
        return true;
    }
    
    virtual bool visit(IfStmt *S) {
        Printer.printStmtPre(S);
        Printer << tok::kwIf;
        super::visit(S->getCond());
        super::visit(S->getThen());
        if (S->hasElseBlock()) {
            Printer << tok::kwElse;
            super::visit(S->getElse());
        }
        Printer.printStmtPost(S);
        return false;
    }
    
    virtual bool visit(ParamList *S) {
        Printer.printStmtPre(S);
        bool IsFirst = true;
        for (auto P : S->getParams()) {
            Printer.printSeparator(IsFirst, ", ");
            Printer.printText(P->getName());
        }
        Printer.printStmtPost(S);
        return false;
    }
    
    virtual bool visit(WhileStmt *S) {
        Printer.printStmtPre(S);
        Printer << tok::kwWhile;
        super::visit(S->getCond());
        super::visit(S->getBody());
        Printer.printStmtPost(S);
        return false;
    }
};

/// Implementation of an \c ASTPrinter, which is used to pretty print the AST.
class PrettyPrinter: public StreamPrinter {
public:
    // Use StreamPrinter constructor.
    using StreamPrinter::StreamPrinter;
    
    virtual void printDeclPre(Decl *D) override {
        switch (D->getKind()) {
        case DeclKind::Const:
        case DeclKind::Func:
        case DeclKind::Var:
            if (!isAtStartOfLine())
                printNewline();
            return;
        default: return;
        }
        if (!isAtStartOfLine())
            printNewline();
    }
    
    virtual void printDeclPost(Decl *D) override {
        switch (D->getKind()) {
        case DeclKind::Const:
        case DeclKind::Var:
            return printText(";");
            return;
        default: return;
        }
    }
    
    virtual void printStmtPre(Stmt *S) override {
        switch (S->getKind()) {
        case StmtKind::For:
        case StmtKind::Func:
        case StmtKind::If:
        case StmtKind::While:
            if (!isAtStartOfLine())
                printNewline();
            return;
        case StmtKind::ParamList:
            return printText("(");
        default: return;
        }
    }
    
    virtual void printStmtPost(Stmt *S) override {
        switch (S->getKind()) {
        case StmtKind::For:
        case StmtKind::Func:
        case StmtKind::If:
        case StmtKind::While:
            if (!isAtStartOfLine())
                printNewline();
            return;
        case StmtKind::ParamList:
            return printText(")");
        default: return;
        }
    }
};
    
} // anonymous namespace

// MARK: - Indentation

Indentation::Indentation(unsigned SL)
: Indentation(IndKind::Space, SL)
{}

Indentation::Indentation(IndKind K, unsigned SL)
: Kind(K), SpaceLen(SL)
{}

llvm::StringRef Indentation::getIndent() const {
    llvm::SmallString<MAX_SPACE_LEN * MAX_INDENTATION> Str;
    for (unsigned i = 0; i < Depth; i++)
        Str += getIndentBlock();
    return Str;
}

llvm::StringRef Indentation::getSpaceIndent() const {
    assert(Kind == IndKind::Space && "Invalid indentation.");
    llvm::SmallString<MAX_SPACE_LEN> Str;
    for (unsigned i = 0; i < SpaceLen; i++)
        Str += " ";
    return Str;
}

llvm::StringRef Indentation::getTabIndent() const {
    assert(Kind == IndKind::Space && "Invalid indentation.");
    return "\t";
}

llvm::StringRef Indentation::getIndentBlock() const {
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

void ASTPrinter::printSeparator(bool &IsFirst, llvm::StringRef S) {
    if (IsFirst)
        IsFirst = false;
    else
        printText(S);
}

ASTPrinter &ASTPrinter::operator<<(llvm::StringRef Text) {
    print(Text);
    return *this;
}

ASTPrinter &ASTPrinter::operator<<(uint64_t N) {
    llvm::SmallString<32> Str;
    llvm::raw_svector_ostream OS(Str);
    OS << N;
    print(OS.str());
    return *this;
}

ASTPrinter &ASTPrinter::operator<<(tok T) {
    llvm::SmallString<16> Buffer;
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

void ASTPrinter::printKeyword(llvm::StringRef KW) {
    *this << KW;
}

void ASTPrinter::print(llvm::StringRef Text) {
    if (isAtStartOfLine())
        printIndent();
    else
        printText(" ");
    printText(Text);
}

// MARK: - StreamPrinter

StreamPrinter::StreamPrinter(llvm::raw_ostream &OS)
: OS(OS)
{}

void StreamPrinter::printText(llvm::StringRef Text) {
    OS << Text;
}

// MARK: - Formatter

void Formatter::format(ASTNode *N, llvm::raw_ostream &OS) {
    PrettyPrinter pp(OS);
    PrintAST p(pp);
    p.ASTVisitor::visit(N);
}


