#include "Block.h"
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>

// Constructor to initialize a Block with given transactions, previous hash, and difficulty level.
Block::Block(std::vector<Transaction> transactions, std::string prevHash, int difficulty) {
    this->transactions = transactions;
    this->prevHash = prevHash;
    this->timestamp = std::time(nullptr);
    this->difficulty = difficulty;
    this->nonce = 0;
    this->blockHash = mineBlock();
}

// Mining function to find a valid hash for the block based on the given difficulty
std::string Block::mineBlock() {
    std::string target(difficulty, '0');
    while (blockHash.substr(0, difficulty) != target) {
        nonce++;
        blockHash = generateHash();
    }
    return blockHash;
}

// Generate the hash of the block
std::string Block::generateHash() const {
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&timestamp), "%Y-%m-%dT%H:%M:%S");
    for (const auto& tx : transactions) {
        ss << tx.sender << tx.receiver << tx.amount;
    }
    ss << prevHash << nonce;
    return sha256(ss.str());
}

// Compute SHA-256 hash for a given string using the EVP API
std::string Block::sha256(const std::string str) const {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, str.c_str(), str.size());
    EVP_DigestFinal_ex(ctx, hash, &hashLen);
    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

