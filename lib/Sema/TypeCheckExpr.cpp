//===--- TypeChecker.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "TypeChecker.h"

#include "dusk/AST/Diagnostics.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/Sema/Sema.h"
#include "dusk/AST/ASTVisitor.h"
#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/AST/ASTWalker.h"

using namespace dusk;
using namespace sema;

namespace {

/// Perform expression solution.
///
/// Operates on a single expression tree and tries to simplify is as much
/// as it can. Most of simplification operations include value substitutions
/// (e.g. substitute indexed element in array with the element itself) and/or
/// an identifier with a value it refers to.
///
/// \note Only constants can be substituted.
class ExprSolver : public ASTWalker {
  TypeChecker &TC;
  
public:
  ExprSolver(TypeChecker &TC) : TC(TC) {}
  
  std::pair<bool, Expr *> preWalkExpr(Expr *E) {
    // Skip all solved expressions
    return {!E->getSolved(), E};
  }
  
  Expr *postWalkExpr(Expr *E) {
    switch (E->getKind()) {
    // Literals can't be solved anymore
    case ExprKind::NumberLiteral:
    case ExprKind::ArrayLiteral:
        
    // Assignements and calls must be executed during runtime
    case ExprKind::Assign:
    case ExprKind::Call:
      break;
      
    case ExprKind::Identifier:
      E = solveIdentifierExpr(static_cast<IdentifierExpr *>(E));
      break;
      
    case ExprKind::Paren:
      // Simply extract paranthesized expression.
      E = static_cast<ParenExpr *>(E)->getExpr();
      break;
      
    case ExprKind::Infix:
      E = solveInfixExpr(static_cast<InfixExpr *>(E));
      break;
      
    case ExprKind::Prefix:
      E = solvePrefixExpr(static_cast<PrefixExpr *>(E));
      break;
      
    case ExprKind::Subscript:
      E = solveSubscriptExpr(static_cast<SubscriptExpr *>(E));
      break;
    }
    // Set each expression to solved at the end of traversal.
    E->setSolved(true);
    return E;
  }
  
private:
  
  // MARK: - Solvers
  
  Expr *solveIdentifierExpr(IdentifierExpr *E) {
    // Check if we have a variable with the name.
    if (auto D = dynamic_cast<ValDecl *>(TC.Lookup.getVal(E->getName()))) {
      // We can only substitute immutable values.
      if (!D->isLet())
        return E;
      
      else if (auto Val = dynamic_cast<NumberLiteralExpr *>(D->getValue()))
        return Val;
      else if (auto Val = dynamic_cast<ArrayLiteralExpr *>(D->getValue()))
        return Val;
    }
    return E;
  }
  
  Expr *solveInfixExpr(InfixExpr *E) {
    auto L = dynamic_cast<NumberLiteralExpr *>(E);
    auto R = dynamic_cast<NumberLiteralExpr *>(E);
    // Check if both sides are number literals
    //
    // NOTE: we can perform arithmetics only with a signle type being integer.
    // In future this resolution will need fix for other types, that might
    // also be applied to these operands.
    if (!L || !R)
      return E;
    
    int64_t Value = 0;
    switch (E->getOp().getKind()) {
    case tok::land:
      Value = L->getValue() && R->getValue();
      break;
      
    case tok::lor:
      Value = L->getValue() || R->getValue();
      break;
    
    case tok::plus:
      Value = L->getValue() + R->getValue();
      break;
    
    case tok::minus:
      Value = L->getValue() - R->getValue();
      break;
      
    case tok::multipy:
      Value = L->getValue() * R->getValue();
      break;
      
    case tok::divide:
      Value = L->getValue() / R->getValue();
      break;
      
    case tok::mod:
      Value = L->getValue() % R->getValue();
      break;
      
    default:
      llvm_unreachable("Unexpected infix operator.");
    }
    return new (TC.Ctx) NumberLiteralExpr(Value, SMRange{});
  }
  
  Expr *solvePrefixExpr(PrefixExpr *E) {
    // Check if destination value is number literal
    //
    // NOTE: we can perform arithmetics only with a signle type being integer.
    // In future this resolution will need fix for other types, that might
    // also be applied to these operands.
    auto Dest = dynamic_cast<NumberLiteralExpr *>(E);
    if (!Dest)
      return E;
    
    int64_t Value = 0;
    switch (E->getOp().getKind()) {
    case tok::lnot:
      Value = !Dest->getValue();
      break;
      
    case tok::minus:
      Value = -Dest->getValue();
      break;
      
    default:
      llvm_unreachable("Unexpectd prefixoperator.");
    }
    return new (TC.Ctx) NumberLiteralExpr(Value, SMRange{});
  }
  
  Expr *solveSubscriptExpr(SubscriptExpr *E) {
    // Check if both destination is an array and index a number literal.
    //
    // NOTE: we can perform subscript only with a arrays. In future this will
    // need update to support subscript operand on other types as well.
    auto Arr = dynamic_cast<ArrayLiteralExpr *>(E->getBase());
    auto Idx = dynamic_cast<NumberLiteralExpr *>(
        E->getSubscript()->getSubscripStmt()->getValue());
    if (!Arr || !Idx)
      return E;
    
    auto Values = Arr->getValues()->getExprPattern();
    if (Values->count() < Idx->getValue() || Idx->getValue() < 0) {
      TC.diagnose(Idx->getLocStart(), diag::array_index_out_of_bounds);
      return E;
    }
    
    return Values->getValues()[Idx->getValue()];
  }
};
  
/// Performs type checking and resolution of expressions.
class ExprChecker : public ExprVisitor<ExprChecker, Expr *> {
  TypeChecker &TC;
  
  typedef ASTVisitor super;
  
  friend super;
  
public:
  ExprChecker(TypeChecker &TC) : TC(TC) {}
  
private:
  
  // MARK: - Visitors
  
  Expr *visitNumberLiteralExpr(NumberLiteralExpr *E) {
    // Number literal is always an integer type.
    E->setType(TC.Ctx.getIntType());
    return E;
  }

  Expr *visitArrayLiteralExpr(ArrayLiteralExpr *E) {
    if (!E->getValues()->count()) {
      TC.diagnose(E->getLocStart(), diag::invalid_array_size);
      return E;
    }
    
    TC.typeCheckPattern(E->getValues());
    auto Ty = E->getValues()->getType()->getPatternType();
    if (!Ty)
      return E;
    
    auto BaseTy = Ty->getItems().front();
    // All items al the array must be of the same type
    for (auto T : Ty->getItems())
      if (!TC.typeCheckEquals(BaseTy, T))
        return E;

    auto ArrTy = new (TC.Ctx) ArrayType(BaseTy, E->getValues()->count());
    E->setType(ArrTy);
    return E;
  }
  
  Expr *visitIdentifierExpr(IdentifierExpr *E) {
    if (auto D = TC.Lookup.getVal(E->getName())) {
      if (D->getType())
        E->setType(D->getType());
    
    } else if (auto Fn = TC.Lookup.getFunc(E->getName())) {
      if (Fn->getType())
        E->setType(Fn->getType());
    
    } else {
      TC.diagnose(E->getLocStart(), diag::undefined_identifier);
    }
    
    return E;
  }

  Expr *visitParenExpr(ParenExpr *E) {
    // Simplt extract paranthesizes expression.
    return E->getExpr();
  }

  Expr *visitAssignExpr(AssignExpr *E) {
    E->setDest(typeCheckExpr(E->getDest(), /* Solve */false));
    E->setSource(typeCheckExpr(E->getSource()));
    
    auto DTy = E->getDest()->getType();
    auto STy = E->getSource()->getType();
    if (!DTy || !STy)
      return E;
    
    // Check if we have valid destination node.
    if (auto Dest = dynamic_cast<IdentifierExpr *>(E->getDest())) {
      if (auto D = dynamic_cast<ValDecl *>(TC.Lookup.getVal(Dest->getName())))
        if (D->isLet()) {
          TC.diagnose(E->getLocStart(), diag::cannot_reassign_let_value);
          return E;
        }
    } else if (dynamic_cast<SubscriptExpr *>(E->getDest()) == nullptr) {
      TC.diagnose(E->getLocStart(), diag::unexpected_expresssion);
      return E;
    }
    
    if (!TC.typeCheckEquals(DTy, STy)) {
      TC.diagnose(E->getLocStart(), diag::type_missmatch);
      return E;
    }
    
    E->setType(DTy);
    return E;
  }

  Expr *visitInfixExpr(InfixExpr *E) {
    E->setLHS(typeCheckExpr(E->getLHS()));
    E->setRHS(typeCheckExpr(E->getRHS()));
    
    auto LTy = E->getLHS()->getType();
    auto RTy = E->getRHS()->getType();
    if (!LTy || !RTy)
      return E;

    // We only support arithmetics with integer types
    if (!LTy->isClassOf(TC.Ctx.getIntType()) ||
        !RTy->isClassOf(TC.Ctx.getIntType())) {
      TC.diagnose(E->getLocStart(), diag::invalid_operand_type);
      return E;
    }
    
    E->setType(LTy);
    return E;
  }

  Expr *visitPrefixExpr(PrefixExpr *E) {
    E->setDest(typeCheckExpr(E->getDest()));
    // We only support integer types with any and all prefix operations.
    if (auto Ty = dynamic_cast<IntType *>(E->getDest()->getType()))
      E->setType(Ty);
    return E;
  }

  Expr *visitCallExpr(CallExpr *E) {
    E->setCallee(typeCheckExpr(E->getCallee()));
    TC.typeCheckPattern(E->getArgs());
    
    // Calle must be function type.
    auto FnTy = dynamic_cast<FunctionType *>(E->getCallee()->getType());
    auto ArgsTy = E->getArgs()->getType();
    if (!FnTy || !ArgsTy)
      return E;
    
    if (!TC.typeCheckEquals(FnTy->getFunctionType()->getArgsType(), ArgsTy))
      return E;
    
    E->setType(FnTy->getFunctionType()->getRetType());
    return E;
  }

  Expr *visitSubscriptExpr(SubscriptExpr *E) {
    E->setBase(TC.typeCheckExpr(E->getBase()));
    TC.typeCheckStmt(E->getSubscript());
    
    // Destination must be array type.
    auto ArrTy = dynamic_cast<ArrayType *>(E->getBase()->getType());
    if (!ArrTy)
      return E;
    
    E->setType(ArrTy->getArrayType()->getBaseType());
    return E;
  }
  
public:
  Expr *typeCheckExpr(Expr *E, bool Solve = true) {
    E = super::visit(E);

    // Solve expression if type resolution succeeded.
    if (E->getType() && Solve) {
      ExprSolver Solver(TC);
      E = E->walk(Solver);
    }
    
    return E;
  }
};
  
} // anonymous namesapce

Expr *TypeChecker::typeCheckExpr(Expr *E) {
  return ExprChecker(*this).typeCheckExpr(E);
}
