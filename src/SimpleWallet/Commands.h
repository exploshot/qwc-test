

#pragma once

#include <WalletInfo.h>

#include <Utilities/ColouredMsg.h>

#include <Wallet/WalletGreen.h>

void printPrivateKeys(CryptoNote::WalletGreen &wallet, bool viewWallet);

void help(bool viewWallet);

void status(CryptoNote::INode &node);

void reset(CryptoNote::INode &node, std::shared_ptr<WalletInfo> &walletInfo);

void blockchainHeight(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet);

void balance(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet,
             bool viewWallet);

void exportKeys(std::shared_ptr<WalletInfo> &walletInfo);
