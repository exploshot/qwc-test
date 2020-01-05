// Please see the included LICENSE file for more information.
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2020, The Qwertycoin Project
//
// This file is part of Qwertycoin.
//
// Qwertycoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Qwertycoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Qwertycoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <WalletInfo.h>

#include <Utilities/ColouredMsg.h>

void listTransfers(bool incoming, bool outgoing,
                   CryptoNote::WalletGreen &wallet,
                   CryptoNote::INode &node);

void listMessages(CryptoNote::WalletGreen &wallet,
                  CryptoNote::INode &node);

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
