//===--- Alignment.h - Address alignement ------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_ALIGNMENT_H
#define DUSK_IRGEN_ALIGNMENT_H

#include "llvm/Support/DataTypes.h"

namespace dusk {

namespace irgen {

/// Represents an address alignment.
class Alignment {
  uint32_t Value;

public:
  constexpr Alignment() : Value(0) {}
  constexpr explicit Alignment(uint32_t V) : Value(V) {}

  /// Returns alignment value.
  constexpr uint32_t getValue() const { return Value; }

  /// Returns \c true if alignment value is zero, \c false otherwise.
  bool isZero() const { return Value == 0; }
  /// Returns \c true if alignment value is one, \c false otherwise.
  bool isOne() const { return Value == 1; }

  /// Calculate log2 of alignemnt.
  unsigned log2() const { return llvm::Log2_64(Value); }

  /// Implicit conversion to bool.
  operator bool() const { return Value != 0; }

  // MARK: - Comparison

  bool operator< (const Alignment &Other) { return Value <  Other.Value; }
  bool operator<=(const Alignment &Other) { return Value <= Other.Value; }
  bool operator> (const Alignment &Other) { return Value >  Other.Value; }
  bool operator>=(const Alignment &Other) { return Value >= Other.Value; }
  bool operator==(const Alignment &Other) { return Value == Other.Value; }
  bool operator!=(const Alignment &Other) { return Value != Other.Value; }
};

} // namespace irgen

} // namespace dusk

#endif /* DUSK_IRGEN_ALIGNMENT_H */

