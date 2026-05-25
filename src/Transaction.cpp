#include "Transaction.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <iostream>
#include <vector>

// Constructor to initialize a Transaction
Transaction::Transaction(std::string sender, std::string receiver, float amount, int nonce)
    : sender(sender), receiver(receiver), amount(amount), nonce(nonce), signatureLength(0) {}

// Method to sign a transaction using EVP_DigestSign
void Transaction::sign(EVP_PKEY* privateKey) {
    std::string dataToSign = sender + receiver + std::to_string(amount) + std::to_string(nonce);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx ||
        EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, privateKey) != 1 ||
        EVP_DigestSignUpdate(ctx, dataToSign.c_str(), dataToSign.size()) != 1) {
        std::cerr << "Signing initialisation failed.\n";
        EVP_MD_CTX_free(ctx);
        return;
    }

    // Determine required buffer size
    size_t sLen = 0;
    if (EVP_DigestSignFinal(ctx, nullptr, &sLen) != 1) {
        std::cerr << "Signing failed (size query).\n";
        EVP_MD_CTX_free(ctx);
        return;
    }

    std::vector<unsigned char> sig(sLen);
    if (EVP_DigestSignFinal(ctx, sig.data(), &sLen) != 1) {
        std::cerr << "Signing failed.\n";
        EVP_MD_CTX_free(ctx);
        return;
    }
    EVP_MD_CTX_free(ctx);

    signature.assign(reinterpret_cast<char*>(sig.data()), sLen);
    signatureLength = static_cast<unsigned int>(sLen);
}

// Method to verify the transaction's signature using EVP_DigestVerify
bool Transaction::verify(EVP_PKEY* publicKey) const {
    ERR_clear_error();
    std::string dataToVerify = sender + receiver + std::to_string(amount) + std::to_string(nonce);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx ||
        EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, publicKey) != 1 ||
        EVP_DigestVerifyUpdate(ctx, dataToVerify.c_str(), dataToVerify.size()) != 1) {
        std::cerr << "Verification initialisation failed.\n";
        EVP_MD_CTX_free(ctx);
        return false;
    }

    int ret = EVP_DigestVerifyFinal(
        ctx,
        reinterpret_cast<const unsigned char*>(signature.data()),
        signatureLength);
    EVP_MD_CTX_free(ctx);

    if (ret != 1) {
        unsigned long err = ERR_get_error();
        char errBuf[256];
        ERR_error_string_n(err, errBuf, sizeof(errBuf));
        std::cerr << "Signature verification failed: " << errBuf << "\n";
        return false;
    }
    return true;
}

// Check overall validity of the transaction
bool Transaction::isValid(EVP_PKEY* publicKey) const {
    if (nonce < 0 || !(amount > 0)) {
        return false;
    }
    return verify(publicKey);
}

