#pragma once
#include <string>
#include "Transaction.h"

// Abstract base class for smart contracts.
//
// Derive from this class to implement custom contract logic. Contracts are
// registered with the Blockchain and are executed against every pending
// transaction during mining.  If any registered contract returns false for a
// transaction, that transaction is dropped before the block is sealed.
//
// Example:
//   class MinAmountContract : public SmartContract {
//   public:
//       std::string getContractId() const override { return "min-amount"; }
//       bool execute(const Transaction& tx) override { return tx.amount >= 1.0f; }
//   };
//
//   blockchain.registerContract(new MinAmountContract());

class SmartContract {
public:
    virtual ~SmartContract() = default;

    // Returns a unique identifier for this contract.
    virtual std::string getContractId() const = 0;

    // Executes the contract's conditions against the given transaction.
    // Returns true if the transaction satisfies the contract; false to reject it.
    virtual bool execute(const Transaction& tx) = 0;
};
