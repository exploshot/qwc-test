

#pragma once

#include <Wallet/WalletGreen.h>

bool fusionTx(CryptoNote::WalletGreen &wallet, CryptoNote::TransactionParameters parameters);

bool optimize(CryptoNote::WalletGreen &wallet, uint64_t threshold);

void quickOptimize(CryptoNote::WalletGreen &wallet);

void fullOptimize(CryptoNote::WalletGreen &wallet);

size_t makeFusionTransaction(CryptoNote::WalletGreen &wallet, uint64_t threshold);