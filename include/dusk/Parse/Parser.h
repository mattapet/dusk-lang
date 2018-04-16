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
        
        CodeBlock *parseCodeBlock();
        ASTNode *parseCodeBlock_();
        
        VarDecl *parseVarDecl();
        
        // MARK: - Expressions
        
        Expr *parseExpr();
        ASTNode *parseExpr_();
        ASTNode *parseFuncCall();
        VariableExpr *parseExprIdentifier();
        NumberLiteralExpr *parseExprNumberLiteral();
        
        // MARK: - Statements
        
        ASTNode *parseStatement();
        FuncStmt *parseFuncStmt();
        ASTNode *parseForStmt();
        ASTNode *parseWhileStmt();
        ASTNode *parseIfStmt();
        ASTNode *parseElseStmt();
        
        ParamList *parseParamList();
        ParamDecl *parseParamList_();
        
    };
} // namespace dusk

#endif /* DUSK_FUNC_H */
