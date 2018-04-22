//===--- IRGenerator.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/IRGen/IRGenerator.h"
#include "dusk/IRGen/Context.h"

#include "GenExpr.h"

using namespace dusk;
using namespace irgen;

IRGenerator::IRGenerator(DiagnosticEngine &Diag)
    : Diag(Diag), Builder({Context}) {}

IRGenerator::~IRGenerator() {}
