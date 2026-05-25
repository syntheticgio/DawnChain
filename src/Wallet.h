#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <openssl/evp.h>
#include "Transaction.h"

class Wallet {
public:
    Wallet(std::string id); // Constructor with wallet ID
    ~Wallet();              // Destructor to free EVP keys

    // Method to send funds to another wallet.
    // Throws std::runtime_error if the wallet has insufficient balance.
    Transaction sendFunds(Wallet& receiver, float amount);

    // Method to update the balance of this wallet based on blockchain transactions
    void updateBalance(const std::vector<Transaction>& transactions);

    // Method to print wallet details
    void printWalletData() const;

    std::string id;       // Wallet ID
    float balance;        // Wallet balance
    EVP_PKEY* publicKey;  // Public key of the wallet for verification

private:
    EVP_PKEY* privateKey; // Private key for signing transactions
    int txNonce;          // Per-wallet transaction counter used to produce unique nonces
    void generateKeys();  // Method to generate RSA key pair
};

#endif // WALLET_H
