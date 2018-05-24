//===--- TypeCheckStmt.cpp ------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "TypeChecker.h"

#include "dusk/Strings.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/AST/Scope.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/AST/ASTVisitor.h"
#include "dusk/Sema/Sema.h"
#include "dusk/Sema/TypeResolver.h"
#include "dusk/AST/ASTWalker.h"
#include "llvm/ADT/StringSwitch.h"

using namespace dusk;
using namespace sema;

namespace {

class TypeReprChecker : public TypeReprVisitor<TypeReprChecker> {
  TypeChecker &TC;

  typedef ASTVisitor super;

  friend super;

public:
  TypeReprChecker(TypeChecker &TC) : TC(TC) {}

private:
  void visitIdentTypeRepr(IdentTypeRepr *TR) {
    auto Ty = llvm::StringSwitch<Type *>(TR->getIdent())
                  .Case(BUILTIN_TYPE_NAME_INT, TC.Ctx.getIntType())
                  .Case(BUILTIN_TYPE_NAME_VOID, TC.Ctx.getVoidType())
                  .Default(nullptr);

    if (Ty)
      TR->setType(Ty);
    else
      TC.diagnose(TR->getLocStart(), diag::unknown_type);
  }

  void visitArrayTypeRepr(ArrayTypeRepr *TR) {
    typeCheckType(TR->getBaseTyRepr());
    TC.typeCheckStmt(TR->getSize());

    auto BaseTy = TR->getBaseTyRepr()->getType();
    auto Size = dynamic_cast<NumberLiteralExpr *>(
        TR->getSize()->getSubscripStmt()->getValue());

    if (!BaseTy)
      return;

    // Array must not be of null type
    if (BaseTy->isVoidType()) {
      TC.diagnose(TR->getLocStart(), diag::type_missmatch);
      return;
    }

    // Array size must be known at compile time
    if (!Size) {
      TC.diagnose(TR->getSize()->getLocStart(), diag::variable_array_size);
      return;
    }
    auto Ty = new (TC.Ctx) ArrayType(BaseTy, Size->getValue());
    TR->setType(Ty);
  }

public:
  void typeCheckType(TypeRepr *TR) { super::visit(TR); }
};

} // anonymous namespace

void TypeChecker::typeCheckType(TypeRepr *TR) {
  TypeReprChecker(*this).typeCheckType(TR);
}
