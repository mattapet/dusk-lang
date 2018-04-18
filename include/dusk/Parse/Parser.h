//===--- Parser.h - Dusk language parser interface --------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_FUNC_H
#define DUSK_FUNC_H

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Stmt.h"
#include "dusk/Parse/Token.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Frontend/InputFile.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"

namespace dusk {
    
/// The main class used for parsing a dusk-lang (.dusk) source file.
class Parser {
    llvm::SourceMgr &SourceManager;
    InputFile &SourceFile;
    Lexer *L;
    
    /// Token currently evaluated by the parser.
    Token Tok;
    
    /// Location of previous token
    llvm::SMLoc PreviousLoc;
    
public:
    Parser(llvm::SourceMgr &SM, InputFile &SF, unsigned BufferID);
    
    /// Returns next token to be parsed.
    const Token &peekToken() const;
    
    /// Consumes current token and returns it's location.
    llvm::SMLoc consumeToken();
    
    /// Consumes token, if it's of expected type
    ///
    /// \retrun \c true if a token was consumed, \c false otherwise.
    bool consumeIf(tok T) {
        if (Tok.is(T)) {
            PreviousLoc = consumeToken();
            return true;
        }
        return false;
    }
    
    /// Main parsing method.
    ///
    /// \return A pointer to the root of the AST representing parsed
    ///  source file.
    ASTNode *parse();
    
//===------------------------------------------------------------------------===
//
//                     Recursive descent parsing methods
//
//===------------------------------------------------------------------------===
private:
    ASTNode *parseGlobal();
    
    // MARK: - Global variable declarations
    
    ValDecl *parseGlobalValDecl();
    VarDecl *parseGlobalVarDecl();
    ConstDecl *parseGlobalConstDecl();
    Expr *parseGlobalDeclValue();
    
    // MARK: - Declarations
    
    FuncDecl *parseFuncDecl();
    
    BlockStmt *parseBlock();
    ASTNode *parseBlockBody();
    ParamDecl *parseParamDecl();
    
    VarDecl *parseVarDecl();
    
    
    // MARK: - Expressions
    
    Expr *parseExpr();
    Expr *parseAssignExpr();
    Expr *parseAssignExprRHS(Expr *LHS);
    
    Expr *parseLogicalExpr();
    Expr *parseLogicalExprRHS(Expr *LHS);
    
    Expr *parseArithExpr();
    Expr *parseArithExprRHS(Expr *LHS);
    
    Expr *parseMulExpr();
    Expr *parseMulExprRHS(Expr *LHS);
    
    Expr *parsePrimaryExpr();
    Expr *parsePrimaryExprRHS(IdentifierExpr *Dest);
    
    IdentifierExpr *parseIdentifierExpr();
    CallExpr *parseCallExpr(IdentifierExpr *Dest);
    SubscriptExpr *parseSubscriptExpr(IdentifierExpr *Dest);
    
    Expr *parseParenExpr();
    NumberLiteralExpr *parseNumberLiteralExpr();
    UnaryExpr *parseUnaryExpr();
    
    
    // MARK: - Statements
    
    ASTNode *parseStatement();
    
    Expr *parseExprStmt();
    
    FuncStmt *parseFuncStmt();
    
    ForStmt *parseForStmt();
    RangeStmt *parseRangeStmt();
    
    WhileStmt *parseWhileStmt();
    
    IfStmt *parseIfStmt();
    BlockStmt *parseElseStmt();
    
    
    // MARK: - Patterns
    
    ExprPattern *parseExprPattern();
    llvm::SmallVector<Expr *, 128> parseExprPatternBody();
    Expr *parseExprPatternItem();
    
    VarPattern *parseVarPattern();
    llvm::SmallVector<ParamDecl *, 128> parseVarPatternBody();
    ParamDecl *parseVarPatternItem();
    
    SubscriptPattern *parseSubscriptPattern();
};
    
} // namespace dusk

#endif /* DUSK_FUNC_H */
