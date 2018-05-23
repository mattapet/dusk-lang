//===--- TypeResolver.cpp -------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/TypeRepr.h"
#include "dusk/AST/ASTContext.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Sema/TypeResolver.h"
#include <memory>

using namespace dusk;
using namespace sema;

TypeResolver::TypeResolver(ASTContext &Ctx, NameLookup &NL, DiagnosticEngine &D)
    : Context(Ctx), NL(NL), Diag(D) {}

bool TypeResolver::preWalkDecl(Decl *D) {
  // Skip all declarations, that already were type resolved.
  return D->getType() != nullptr;
}

bool TypeResolver::postWalkDecl(Decl *D) {
  switch (D->getKind()) {
  case DeclKind::Var:
    return resolveVarDecl(static_cast<VarDecl *>(D));
  case DeclKind::Let:
    return resolveLetDecl(static_cast<LetDecl *>(D));
  case DeclKind::Func:
    return resolveFuncDecl(static_cast<FuncDecl *>(D));
  case DeclKind::Module:
    return resolveModuleDecl(static_cast<ModuleDecl *>(D));
  case DeclKind::Param:
    return resolveParamDecl(static_cast<ParamDecl *>(D));
  }
}

std::pair<bool, Expr *> TypeResolver::preWalkExpr(Expr *E) {
  // Skip all expression that do already have a type
  return {E->getType() != nullptr, E};
}

Expr *TypeResolver::postWalkExpr(Expr *E) {
  switch (E->getKind()) {
  case ExprKind::NumberLiteral:
    return resolveNumberLiteralExpr(static_cast<NumberLiteralExpr *>(E));
  case ExprKind::ArrayLiteral:
    return resolveArrayLiteralExpr(static_cast<ArrayLiteralExpr *>(E));
  case ExprKind::Identifier:
    return resolveIdentifierExpr(static_cast<IdentifierExpr *>(E));
  case ExprKind::Paren:
    return resolveParenExpr(static_cast<ParenExpr *>(E));
  case ExprKind::Assign:
    return resolveAssignExpr(static_cast<AssignExpr *>(E));
  case ExprKind::Infix:
    return resolveInfixExpr(static_cast<InfixExpr *>(E));
  case ExprKind::Prefix:
    return resolvePrefixExpr(static_cast<PrefixExpr *>(E));
  case ExprKind::Call:
    return resolveCallExpr(static_cast<CallExpr *>(E));
  case ExprKind::Subscript:
    return resolveSubscriptExpr(static_cast<SubscriptExpr *>(E));
  }
}

bool TypeResolver::preWalkTypeRepr(TypeRepr *TR) {
  // Skip all type representation that were type resolved
  return TR->getType() != nullptr;
}

bool TypeResolver::postWalkTypeRepr(TypeRepr *TR) {
  switch (TR->getKind()) {
  case TypeReprKind::Ident:
    return typeResolveIdentTypeRepr(static_cast<IdentTypeRepr *>(TR));
  case TypeReprKind::Array:
    return typeResolveArrayTypeRepr(static_cast<ArrayTypeRepr *>(TR));
  }
}

// MARK: - Declarations

bool TypeResolver::resolveLetDecl(LetDecl *D) {
  if (D->hasTypeRepr())
    D->setType(D->getTypeRepr()->getType());
  else if (D->hasValue())
    D->setType(D->getValue()->getType());
  else
    Diag.diagnose(D->getLocStart(), diag::ambigous_types);
  return true;
}

bool TypeResolver::resolveVarDecl(VarDecl *D) {
  if (D->hasTypeRepr())
    D->setType(D->getTypeRepr()->getType());
  else if (D->hasValue())
    D->setType(D->getValue()->getType());
  else
    Diag.diagnose(D->getLocStart(), diag::ambigous_types);
  return true;
}

bool TypeResolver::resolveFuncDecl(FuncDecl *D) {
  auto AT = D->getArgs()->getType();
  Type *RT;
  if (D->hasTypeRepr()) {
    RT = D->getTypeRepr()->getType();
  } else {
    RT = new (Context) VoidType();
  }

  D->setType(new (Context) FunctionType(AT, RT));
  return true;
}

bool TypeResolver::resolveModuleDecl(ModuleDecl *D) { return true; }

bool TypeResolver::resolveParamDecl(ParamDecl *D) {
  if (D->hasTypeRepr())
    D->setType(D->getTypeRepr()->getType());
  return true;
}

// MARK: - Expressions

Expr *TypeResolver::resolveNumberLiteralExpr(NumberLiteralExpr *E) {
  E->setType(Context.getIntType());
  return E;
}

Expr *TypeResolver::resolveArrayLiteralExpr(ArrayLiteralExpr *E) {
  auto Size = E->getValues()->count();
  auto Vals = static_cast<ExprPattern *>(E->getValues());
  auto BaseTy = Vals->getValues().front()->getType();
  E->setType(new (Context) ArrayType(BaseTy, Size));
  return E;
}

Expr *TypeResolver::resolveIdentifierExpr(IdentifierExpr *E) {
  if (auto D = NL.getVal(E->getName())) {
    E->setType(D->getType());
    return E;
  }
  Diag.diagnose(E->getLocStart(), diag::undefined_identifier);
  return E;
}

Expr *TypeResolver::resolveParenExpr(ParenExpr *E) {
  E->setType(E->getExpr()->getType());
  return E;
}

Expr *TypeResolver::resolveAssignExpr(AssignExpr *E) {
  E->setType(E->getSource()->getType());
  return E;
}

Expr *TypeResolver::resolveInfixExpr(InfixExpr *E) {
  E->setType(E->getLHS()->getType());
  return E;
}

Expr *TypeResolver::resolvePrefixExpr(PrefixExpr *E) {
  E->setType(E->getDest()->getType());
  return E;
}

Expr *TypeResolver::resolveCallExpr(CallExpr *E) {
  if (auto Ident = dynamic_cast<IdentifierExpr *>(E)) {
    if (auto Fn = NL.getFunc(Ident->getName())) {
      auto FnTy = static_cast<FunctionType *>(Fn->getType());
      if (FnTy->getRetType()->isValueType()) {
        E->setType(FnTy->getRetType());
        return E;

      } else {
        Diag.diagnose(E->getLocStart(), diag::expected_value_type_expression);
      }

    } else {
      if (NL.contains(Ident->getName())) {
        Diag.diagnose(E->getLocStart(), diag::func_call_non_func_type);
      } else {
        Diag.diagnose(E->getLocStart(), diag::undefined_identifier);
      }
    }

  } else {
    Diag.diagnose(E->getLocStart(), diag::func_call_non_func_type);
  }
  return E;
}

Expr *TypeResolver::resolveSubscriptExpr(SubscriptExpr *E) {
  if (auto ArrTy = dynamic_cast<ArrayType *>(E->getBase()->getType())) {
    E->setType(ArrTy->getBaseType());
    return E;
  }
  Diag.diagnose(E->getLocStart(), diag::subscripted_value_not_array);
  return E;
}

// MARK: - Type representations

bool TypeResolver::typeResolveIdentTypeRepr(IdentTypeRepr *TR) {
  if (TR->getIdent() == "Void") {
    TR->setType(Context.getVoidType());
    return true;

  } else if (TR->getIdent() == "Int") {
    TR->setType(Context.getVoidType());
    return true;
  }

  Diag.diagnose(TR->getLocStart(), diag::unknown_type);
  return true;
}

bool TypeResolver::typeResolveArrayTypeRepr(ArrayTypeRepr *TR) {
  auto BaseTy = TR->getBaseTyRepr()->getType();
  auto Size = 0; // TODO: perform exctraction. TR->getSize();

  TR->setType(new (Context) ArrayType(BaseTy, Size));
  return true;
}
