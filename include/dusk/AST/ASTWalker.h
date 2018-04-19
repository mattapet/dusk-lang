//===--- ASTWalker.h - Class for walking the AST ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_WALKER_H
#define DUSK_AST_WALKER_H

namespace dusk {
class Decl;
class Expr;
class Stmt;
class Pattern;

/// \brief Base class for all classes, that whish to traverse the AST.
///
/// \description \c ASTWalker provides default API for walking the AST. The API
///  consists of \c postWalk and \c preWalk methods called by traversed
///  AST nodes.
class ASTWalker {
public:
    ASTWalker() = default;
    virtual ~ASTWalker() = default;

    /// This method is called before a node is being walked.
    ///
    /// \param D A declaration node that will be walked.
    ///
    /// \return \c true if the node should be walked, \c false otherwise.
    virtual bool preWalk(Decl *D) { return true; }

    /// This method is called before a node was walked.
    ///
    /// \param D A declaration node that was walked.
    ///
    /// \return \c true if the node should terminate traversal,
    ///  \c false otherwise.
    virtual bool postWalk(Decl *D) { return true; }

    /// This method is called before a node is being walked.
    ///
    /// \param E An expression node that will be walked.
    ///
    /// \return \c true if the node should be walked, \c false otherwise.
    virtual bool preWalk(Expr *E) { return true; }

    /// This method is called before a node was walked.
    ///
    /// \param E An expression node that was walked.
    ///
    /// \return \c true if the node should terminate traversal,
    ///  \c false otherwise.
    virtual bool postWalk(Expr *E) { return true; }

    /// This method is called before a node is being walked.
    ///
    /// \param S A statement node that will be walked.
    ///
    /// \return \c true if the node should be walked, \c false otherwise.
    virtual bool preWalk(Stmt *S) { return true; }

    /// This method is called before a node was walked.
    ///
    /// \param S A statement node that was walked.
    ///
    /// \return \c true if the node should terminate traversal,
    ///  \c false otherwise.
    virtual bool postWalk(Stmt *S) { return true; }
    
    /// This method is called before a node is being walked.
    ///
    /// \param P A pattern node that will be walked.
    ///
    /// \return \c true if the node should be walked, \c false otherwise.
    virtual bool preWalk(Pattern *P) { return true; }
    
    /// This method is called before a node was walked.
    ///
    /// \param P A pattern node that was walked.
    ///
    /// \return \c true if the node should terminate traversal,
    ///  \c false otherwise.
    virtual bool postWalk(Pattern *P) { return true; }
};
    
} // namespace dusk

#endif /* DUSK_AST_WALKER_H */
