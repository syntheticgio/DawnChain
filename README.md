
# DawnChain — Simple Blockchain Implementation in C++

## Overview

DawnChain is a simple implementation of a blockchain with basic wallet functionality. The blockchain contains blocks, where each block contains multiple transactions. Wallets can perform transactions, sending and receiving funds, which are then added to the blockchain. The architecture also includes an extension point for **smart contracts** so that custom transaction rules can be layered on without rewriting core blockchain logic.

## Features

1. **Basic Blockchain**: Create a blockchain and add blocks to it. Each block has a hash, a timestamp, and a list of transactions.

2. **Mining**: Blocks have a `mineBlock()` function that solves a proof-of-work problem.

3. **Wallets**: Wallets with unique IDs can send and receive funds. Each wallet generates a 2 048-bit RSA key pair using the OpenSSL EVP API.

4. **Transaction Validation**: Transactions are only valid if the wallet has sufficient balance. `sendFunds()` throws `std::runtime_error` if the balance is too low.

5. **Cryptographic Signatures**: Every transaction is signed with the sender's private key (RSA/SHA-256 via the OpenSSL EVP API) and verified during `isChainValid()`.

6. **Blockchain Integrity Check**: `isChainValid()` verifies that every block's hash is consistent with its contents, that each block points to the correct previous hash, and that every transaction carries a valid signature.

7. **Wallet Balance Update**: After mining a block, `notifyWallets()` recalculates each wallet's balance from the confirmed transactions and registers public keys for later signature verification.

8. **Smart Contract Extension Point**: Register objects derived from `SmartContract` with `Blockchain::registerContract()`. Registered contracts are executed against every pending transaction during `minePendingTransactions()`; transactions that fail any contract are silently dropped before the block is sealed.

---

## How to Build and Run the Project

This project uses a Makefile to automate the build process. Follow these steps to build and run the project:

### Prerequisites

- Make sure you have `g++` and `make` installed on your system.
- OpenSSL library (≥ 1.1) is required for cryptographic functions. Install it if you haven't done so already:
  - **Ubuntu/Debian**: `sudo apt-get install libssl-dev pkg-config`
  - **macOS with Homebrew**: `brew install openssl pkg-config`
  - **Windows**: Download and install from [OpenSSL's website](https://www.openssl.org/source/).

### Building the Project

1. **Clone the repository** to your local machine:
    ```
    git clone https://github.com/syntheticgio/DawnChain.git
    ```

2. **Navigate to the project directory**:
    ```
    cd DawnChain
    ```

3. **Run the Makefile**:
    ```
    make
    ```
    This will compile all the necessary files and create an executable named `blockchain_app`.

    > The Makefile auto-detects OpenSSL via `pkg-config`. If `pkg-config` is not available it falls back to `/opt/homebrew/opt/openssl` (Homebrew on Apple Silicon/Intel).

### Running the Project

After a successful build, run the project with:

```
./blockchain_app
```


### Cleaning Up

To remove all compiled files and the generated executable:

```
make clean
```


## Code Structure

| File | Purpose |
|---|---|
| `src/main.cpp` | Driver program demonstrating blockchain and wallet functionality |
| `src/Blockchain.cpp` / `Blockchain.h` | `Blockchain` class — manages blocks, pending transactions, and the smart-contract registry |
| `src/Block.cpp` / `Block.h` | `Block` class — individual blocks with proof-of-work mining |
| `src/Transaction.cpp` / `Transaction.h` | `Transaction` class — RSA-signed transfers between wallets |
| `src/Wallet.cpp` / `Wallet.h` | `Wallet` class — EVP key-pair generation, fund transfers, balance tracking |
| `src/SmartContract.h` / `SmartContract.cpp` | `SmartContract` abstract base class — derive to add custom contract logic |

## Adding a Smart Contract

Derive from `SmartContract`, implement `getContractId()` and `execute()`, then register the contract with the blockchain **before** calling `minePendingTransactions()`:

```cpp
#include "SmartContract.h"

// Example: reject transactions below a minimum amount
class MinAmountContract : public SmartContract {
public:
    std::string getContractId() const override { return "min-amount"; }
    bool execute(const Transaction& tx) override {
        return tx.amount >= 10.0f;  // only allow transfers of ≥ 10
    }
};

int main() {
    Blockchain chain;
    MinAmountContract minAmount;
    chain.registerContract(&minAmount);

    // ... create wallets, call sendFunds, createTransaction, minePendingTransactions ...
}
```

## Recent Updates

- **Bug fix**: Added missing `#include <unordered_map>` in `Blockchain.h` (previously caused a compilation error).
- **Bug fix**: Replaced non-standard VLA with `std::vector` in transaction signing.
- **Bug fix**: `sendFunds()` now throws `std::runtime_error` when the sender has insufficient balance.
- **Bug fix**: `isChainValid()` no longer crashes when a sender's public key is missing from the map; it returns `false` with an error message.
- **Improvement**: Per-wallet nonce counter (was hardcoded to 12345) prevents transaction replay.
- **Improvement**: Migrated all cryptography from deprecated OpenSSL 1.x low-level APIs to the modern EVP API — zero deprecation warnings with OpenSSL 3.x.
- **Improvement**: `createTransaction()` now validates the transaction before adding it to the pending list.
- **Improvement**: `printWalletData()` prints the actual PEM-encoded public key instead of a raw pointer address.
- **New feature**: `SmartContract` abstract base class and `Blockchain::registerContract()` extension point.
- **Build fix**: Makefile uses `pkg-config` for portable OpenSSL detection instead of hard-coded Homebrew paths.

## Dependencies

- C++ Standard Library (C++11 or later)
- OpenSSL (≥ 1.1) for SHA-256 hashing and RSA key operations
