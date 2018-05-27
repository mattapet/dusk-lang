//===--- RuntimeFuncs.h - Runtime function declarations ---------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_RUNTIME_FUNCS_H
#define DUSK_RUNTIME_FUNCS_H

#include "dusk/Basic/LLVM.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/NameLookup.h"
#include "dusk/AST/ASTContext.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SourceMgr.h"
#include <memory>
#include <vector>

namespace dusk {

static ASTNode *getPrintln(ASTContext &Context) {
  auto TyRepr = new (Context) IdentTypeRepr("Int");
  auto P =
      new (Context) ParamDecl(ValDecl::Specifier::Let, "val", SMLoc{}, TyRepr);
  llvm::SmallVector<Decl *, 128> Prms;
  Prms.push_back(P);
  auto Pttrn = new (Context) VarPattern(std::move(Prms), SMLoc{}, SMLoc{});
  auto Fn = new (Context) FuncDecl("println", SMLoc{}, SMLoc{}, Pttrn);
  return new (Context) ExternStmt(SMLoc{}, Fn);
}

static ASTNode *getReadln(ASTContext &Context) {
  NameLookup NL;
  llvm::SmallVector<Decl *, 128> Prms;
  auto Pttrn = new (Context) VarPattern(std::move(Prms), SMLoc{}, SMLoc{});
  auto TyRepr = new (Context) IdentTypeRepr("Int");
  auto Fn = new (Context) FuncDecl("readln", SMLoc{}, SMLoc{}, Pttrn, TyRepr);
  return new (Context) ExternStmt(SMLoc{}, Fn);
}
  
static ASTNode *get__iter_range(ASTContext &Context) {
  auto TyRepr = new (Context) IdentTypeRepr("Int");
  llvm::SmallVector<Decl *, 128> Prms;
  Prms.push_back(new (Context) ParamDecl(ValDecl::Specifier::Let, "Start",
                                         SMLoc{}, TyRepr));
  Prms.push_back(new (Context) ParamDecl(ValDecl::Specifier::Let, "End",
                                         SMLoc{}, TyRepr));

  auto Pttrn = new (Context) VarPattern(std::move(Prms), SMLoc{}, SMLoc{});
  auto Fn =
      new (Context) FuncDecl("__iter_range", SMLoc{}, SMLoc{}, Pttrn, TyRepr);
  return new (Context) ExternStmt(SMLoc{}, Fn);
}

static ASTNode *get__iter_step(ASTContext &Context) {
  auto TyRepr = new (Context) IdentTypeRepr("Int");
  llvm::SmallVector<Decl *, 128> Prms;
  Prms.push_back(new (Context) ParamDecl(ValDecl::Specifier::Let, "Start",
                                         SMLoc{}, TyRepr));
  Prms.push_back(new (Context) ParamDecl(ValDecl::Specifier::Let, "End",
                                         SMLoc{}, TyRepr));
  
  auto Pttrn = new (Context) VarPattern(std::move(Prms), SMLoc{}, SMLoc{});
  auto Fn =
  new (Context) FuncDecl("__iter_step", SMLoc{}, SMLoc{}, Pttrn, TyRepr);
  return new (Context) ExternStmt(SMLoc{}, Fn);
}
  
static void getFuncs(ASTContext &Context) {
  std::vector<ASTNode *> NF{getPrintln(Context), getReadln(Context),
                            get__iter_range(Context), get__iter_step(Context)};
  auto C = Context.getRootModule()->getContents();
  NF.insert(NF.end(), C.begin(), C.end());
  Context.getRootModule()->setContents(std::move(NF));
}

} // namespace dusk

#endif /* DUSK_RUNTIME_FUNCS_H */
