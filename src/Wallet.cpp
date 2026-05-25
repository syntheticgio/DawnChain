#include "Wallet.h"
#include <iostream>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

// Constructor to initialize a Wallet with given ID
Wallet::Wallet(std::string id)
    : id(id), balance(0.0f), publicKey(nullptr), privateKey(nullptr), txNonce(0) {
    generateKeys();
}

// Destructor to free EVP key pairs
Wallet::~Wallet() {
    EVP_PKEY_free(privateKey);
    privateKey = nullptr;
    EVP_PKEY_free(publicKey);
    publicKey = nullptr;
}

// Method to generate a 2048-bit RSA key pair using the EVP API
void Wallet::generateKeys() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);

    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_keygen(ctx, &pkey);
    EVP_PKEY_CTX_free(ctx);

    privateKey = pkey;

    // Extract the public-only key by round-tripping through a BIO
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, pkey);
    publicKey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free_all(bio);
}

// Method to send funds to another wallet
Transaction Wallet::sendFunds(Wallet& receiver, float amount) {
    if (balance < amount) {
        throw std::runtime_error("Insufficient balance in wallet '" + id + "'");
    }

    int nonce = ++txNonce;  // Unique, ever-increasing nonce per wallet
    Transaction tx(id, receiver.id, amount, nonce);
    tx.sign(privateKey);
    return tx;
}

// Method to update balance based on the blockchain transactions
void Wallet::updateBalance(const std::vector<Transaction>& transactions) {
    for (const auto& tx : transactions) {
        if (tx.sender == id) {
            balance -= tx.amount;
        }
        if (tx.receiver == id) {
            balance += tx.amount;
        }
    }
}

// Method to print wallet details
void Wallet::printWalletData() const {
    std::cout << "Wallet ID: " << id << "\n";
    std::cout << "Balance:   " << balance << "\n";

    // Print the public key in PEM format
    BIO* bio = BIO_new(BIO_s_mem());
    if (bio && publicKey) {
        PEM_write_bio_PUBKEY(bio, publicKey);
        size_t keylen = BIO_pending(bio);
        std::string keyStr(keylen, '\0');
        BIO_read(bio, &keyStr[0], static_cast<int>(keylen));
        BIO_free_all(bio);
        std::cout << "Public Key:\n" << keyStr << "\n";
    }
}



