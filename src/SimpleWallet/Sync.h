

#pragma once

#include <WalletInfo.h>

#include <Utilities/ColouredMsg.h>

void listTransfers(bool incoming, bool outgoing,
                   CryptoNote::WalletGreen &wallet, CryptoNote::INode &node);

void printOutgoingTransfer(CryptoNote::WalletTransaction t,
                           CryptoNote::INode &node);

void printIncomingTransfer(CryptoNote::WalletTransaction t,
                           CryptoNote::INode &node);

void saveCSV(CryptoNote::WalletGreen &wallet, CryptoNote::INode &node);

void syncWallet(CryptoNote::INode &node,
                std::shared_ptr<WalletInfo> &walletInfo);

void checkForNewTransactions(std::shared_ptr<WalletInfo> &walletInfo);

std::string getBlockTimestamp(CryptoNote::BlockDetails b);

CryptoNote::BlockDetails getBlock(uint32_t blockHeight,
                                  CryptoNote::INode &node);

std::string getPrompt(std::shared_ptr<WalletInfo> &walletInfo);
