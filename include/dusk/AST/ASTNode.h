//===--- ASTNode.h - Dusk language AST node ---------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_NODE_H
#define DUSK_AST_NODE_H

#include "llvm/Support/SMLoc.h"

namespace dusk {
    class ASTWalker;
    
    /// \brief Building stone of the Dusk languange AST.
    ///
    /// \decription ASTNode is a an abstract base class and a basic building
    ///   block for the dusk language AST. Each class that wish to be represent
    ///   an AST node must derive from this class.
    ///
    ///  \c ASTNode also provides a unified interface which all derived classes
    ///  must implement, to be able to properly use functionality of classes
    ///  operating on AST e.g. \c ASTWalker.
    class ASTNode {
    public:
        
        /// Return text range in source file represented by the node.
        virtual llvm::SMRange getSourceRange() const = 0;
        
        /// Return start of the text range represented by the node.
        llvm::SMLoc getLocStart() const { return getSourceRange().Start; }
        
        /// Return end of the text range represented by the node.
        llvm::SMLoc getLocEnd() const { return getSourceRange().End; }
        
        /// Walks AST node.
        ///
        /// \return \c true if walked the node properly and may continue AST
        ///  traversal, \c false if should terminate.
        virtual bool walk(ASTWalker &Walker);
    };
    
} // namespace dusk

#endif /* DUSK_AST_NODE_H */
