#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <openssl/evp.h>

class Transaction {
public:
    std::string sender;          // Sender's wallet ID
    std::string receiver;        // Receiver's wallet ID
    float amount;                // Amount of money transferred
    std::string signature;       // Digital signature for this transaction
    int nonce;                   // Per-wallet transaction counter (prevents replay attacks)
    unsigned int signatureLength; // Length of the digital signature

    // Constructor to initialize the transaction
    Transaction(std::string sender, std::string receiver, float amount, int nonce);

    // Method to sign the transaction with the sender's private key
    void sign(EVP_PKEY* privateKey);

    // Method to verify the transaction using the sender's public key
    bool verify(EVP_PKEY* publicKey) const;

    // Method to check overall validity of the transaction
    bool isValid(EVP_PKEY* publicKey) const;
};

#endif // TRANSACTION_H
