#include "Blockchain.h"
#include "SmartContract.h"
#include "Wallet.h"
#include <iostream>

// Constructor to initialize a Blockchain with the Genesis Block
Blockchain::Blockchain() {
    std::vector<Transaction> emptyTransactions;  // Empty list for genesis block
    chain.emplace_back(emptyTransactions, "0", 2);  // Genesis Block
}

// Create a transaction and add it to the list of pending transactions
void Blockchain::createTransaction(Transaction transaction) {
    if (!isTransactionValid(transaction)) {
        std::cerr << "Transaction rejected: invalid amount or nonce.\n";
        return;
    }
    pendingTransactions.push_back(transaction);
}

// Mine pending transactions into a new block and add it to the blockchain.
// Any registered smart contracts are executed first; transactions that fail
// a contract are dropped before the block is sealed.
void Blockchain::minePendingTransactions() {
    std::vector<Transaction> validTx;
    for (const auto& tx : pendingTransactions) {
        bool passed = true;
        for (auto& kv : contracts) {
            if (!kv.second->execute(tx)) {
                std::cerr << "Transaction dropped by contract '" << kv.first << "'.\n";
                passed = false;
                break;
            }
        }
        if (passed) {
            validTx.push_back(tx);
        }
    }
    Block newBlock(validTx, chain.back().blockHash, 2);
    chain.push_back(newBlock);
    pendingTransactions.clear();
}

// Check if a block's hash is valid
bool Blockchain::isBlockHashValid(const Block& block) {
    return block.blockHash == block.generateHash();
}

// Check if a transaction is valid
bool Blockchain::isTransactionValid(const Transaction& tx) {
    return tx.amount > 0 && tx.nonce >= 0;
}

// Check the validity of the entire blockchain
bool Blockchain::isChainValid() {
    for (size_t i = 1; i < chain.size(); ++i) {
        const Block& currBlock = chain[i];
        const Block& prevBlock = chain[i - 1];

        if (!isBlockHashValid(currBlock)) {
            return false;
        }

        if (currBlock.prevHash != prevBlock.blockHash) {
            return false;
        }

        for (const auto& tx : currBlock.transactions) {
            auto it = publicKeyMap.find(tx.sender);
            if (it == publicKeyMap.end() || it->second == nullptr) {
                std::cerr << "No public key found for sender: " << tx.sender << "\n";
                return false;
            }
            if (!tx.isValid(it->second)) {
                return false;
            }
        }
    }
    return true;
}

// Display the details of the entire blockchain
void Blockchain::printChain() {
    for (const auto& block : chain) {
        std::cout << "Block Timestamp: " << block.timestamp << std::endl;
        std::cout << "Previous Hash: " << block.prevHash << std::endl;
        std::cout << "Block Hash: " << block.blockHash << std::endl;
        std::cout << "Transactions:" << std::endl;

        for (const auto& tx : block.transactions) {
            std::cout << "  Sender: " << tx.sender << " Receiver: " << tx.receiver << " Amount: " << tx.amount << std::endl;
        }

        std::cout << "Nonce: " << block.nonce << std::endl;
        std::cout << std::endl;
    }
}

// Notify wallets with updated transactions and balances
void Blockchain::notifyWallets(std::vector<Wallet*>& wallets) {
    for (auto& wallet : wallets) {
        publicKeyMap[wallet->id] = wallet->publicKey;  // Store the public key in the map
        for (auto& block : chain) {
            wallet->updateBalance(block.transactions);
        }
    }
}

// Register a smart contract with the blockchain
void Blockchain::registerContract(SmartContract* contract) {
    if (contract) {
        contracts[contract->getContractId()] = contract;
    }
}

