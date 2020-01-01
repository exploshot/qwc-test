

#pragma once

#include <string>

#include <INode.h>
#include <WalletGreenTypes.h>

void confirmPassword(std::string walletPass);

bool confirm(std::string msg);
bool confirm(std::string msg, bool defaultReturn);

std::string formatAmount(uint64_t amount);
std::string formatAmountInt(int64_t amount);
std::string formatAmountLegacy(uint64_t amount);
std::string formatDollars(uint64_t amount);
std::string formatCents(uint64_t amount);

std::string getPaymentID(std::string extra);

uint64_t getScanHeight();
