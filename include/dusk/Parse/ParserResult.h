//===--- ParserResult.h - Result parsing a file -----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_PARSER_RESULT_H
#define DUSK_PARSER_RESULT_H

#include "dusk/AST/Diagnostics.h"
#include <vector>
#include <memory>

namespace dusk {
class ASTNode;
class ModuleDecl;

/// Represents a parser result.
///
/// Holds and owns all of the parsed nodes. Also indicates, if the parser
/// ended correctly - if it contains any \c diag::Diag nodes in the parsed AST.
class ParserResult {
    /// AST nodes
    std::vector<std::unique_ptr<ASTNode>> Nodes;
    
    /// Restul state indicator.
    bool IsError = false;
    
    /// Pointer to the root of the parsed AST.
    ModuleDecl *Root;
    
public:
    /// Retuns a pointer to the root of the AST.
    ModuleDecl *getRoot() const { return Root; }
    
    /// Sets the root of the restulting AST.
    void setRoot(ModuleDecl *R) { Root = R; }
    
    /// Returns \c true if \c setError has been called upon the result,
    /// \c false otherwise.
    bool isError() const { return IsError; }
    
    /// Sets parser to error state.
    void setError() { IsError = true; }
    
    /// Returns nodes owned by the \c ParserResult instance.
    ///
    /// Calling this method passes ownership of the nodes from called instance
    /// to resulting receiver of this call. Therefore this method invalidates
    /// this \c ParserResult instance.
    std::vector<std::unique_ptr<ASTNode>> &&takeNodes() {
        return std::move(Nodes);
    }
    
    /// Takes a single AST node and takes it's ownership.
    ///
    /// \c return A raw pointer of the node. \b Beware: The pointer's ownership
    ///  still belongs to the \c ParserResult instance, therefore the pointer
    ///  is guaranteed to live as long as the \c ParserResult instance and must
    ///  not be deallocated before that instance passes the ownership of the
    ///  nodes explicetelly by calling \c takeNodes() method.
    template <typename T>
    T *push(std::unique_ptr<T> &&N) {
        Nodes.push_back(std::move(N));
        return static_cast<T *>(Nodes.back().get());
    }
};
    
} // namespace dusk

#endif /* DUSK_PARSER_RESULT_H */
