//===--- Address.h - Address representation ---------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_IRGEN_ADDRESS_H
#define DUSK_IRGEN_ADDRESS_H

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"

namespace dusk {

namespace irgen {

/// A simple convenience address wrapper.
class Address {
  /// Encapsulated address.
  llvm::Value *Addr;

public:
  /// Creates an invalid address.
  Address() : Addr(nullptr) {}
  
  /// Creates a valid address.
  Address(llvm::Value *A) : Addr(A) {
    assert(Addr != nullptr && "Creating an invalid address");
  }

  /// Convenience imlicit conversion
  operator llvm::Value *() const {
    return getAddress();
  }
  
  /// Access the address.
  llvm::Value *operator->() const {
    assert(isValid() && "Accessing an invalid address");
    return getAddress();
  }

  /// Returns \c true if address is no null, \c false otherwise.
  bool isValid() const { return Addr != nullptr; }

  /// Returns address value.
  llvm::Value *getAddress() const { return Addr; }
  
  /// Returns address type as a pointer type.
  llvm::PointerType *getType() const {
    return llvm::cast<llvm::PointerType>(Addr->getType());
  }
  
  /// Returns IR name of address.
  llvm::StringRef getName() const {
    return getAddress()->getName();
  }
};

} // namespace irgen
} // namespcae dusk

#endif /* DUSK_IRGEN_ADDRESS_H */
