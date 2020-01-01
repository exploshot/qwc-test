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

#include <memory>

#include <WalletInfo.h>

#include <Common/StringTools.h>

#include <CryptoNoteCore/Account.h>

#include <Global/Constants.h>

#include <Mnemonics/Mnemonics.h>

#include <SimpleWallet/Commands.h>
#include <SimpleWallet/Fusion.h>
#include <SimpleWallet/Sync.h>
#include <SimpleWallet/Tools.h>
#include <SimpleWallet/Transfer.h>

#include <Utilities/ColouredMsg.h>

void exportKeys(std::shared_ptr<WalletInfo> &walletInfo)
{
    confirmPassword (walletInfo->walletPass);
    printPrivateKeys (walletInfo->wallet, walletInfo->viewWallet);
}

void printPrivateKeys(CryptoNote::WalletGreen &wallet, bool viewWallet)
{
    Crypto::SecretKey privateViewKey = wallet.getViewKey ().secretKey;

    if (viewWallet) {
        std::cout
            << SuccessMsg ("Private view key:")
            << std::endl
            << SuccessMsg (Common::podToHex (privateViewKey))
            << std::endl;
        return;
    }

    Crypto::SecretKey privateSpendKey = wallet.getAddressSpendKey (0).secretKey;

    Crypto::SecretKey derivedPrivateViewKey;

    Crypto::CryptoOps::generateViewFromSpend (privateSpendKey,
                                              derivedPrivateViewKey);

    bool deterministicPrivateKeys = derivedPrivateViewKey == privateViewKey;

    std::cout
        << SuccessMsg ("Private spend key:")
        << std::endl
        << SuccessMsg (Common::podToHex (privateSpendKey))
        << std::endl
        << std::endl
        << SuccessMsg ("Private view key:")
        << std::endl
        << SuccessMsg (Common::podToHex (privateViewKey))
        << std::endl;

    if (deterministicPrivateKeys) {
        std::string mnemonicSeed = Mnemonics::PrivateKeyToMnemonic (privateSpendKey);

        std::cout
            << std::endl
            << SuccessMsg ("Mnemonic seed:")
            << std::endl
            << SuccessMsg (mnemonicSeed)
            << std::endl;
    }
}

void help(bool viewWallet)
{
    std::cout
        << "Available commands:"
        << std::endl
        << SuccessMsg ("help", 25)
        << "List this help message"
        << std::endl
        << SuccessMsg ("reset", 25)
        << "Discard cached data and recheck for transactions"
        << std::endl
        << SuccessMsg ("bc_height", 25)
        << "Show the blockchain height"
        << std::endl
        << SuccessMsg ("balance", 25)
        << "Display how much "
        << WalletConfig::ticker
        << " you have"
        << std::endl
        << SuccessMsg ("export_keys", 25)
        << "Export your private keys"
        << std::endl
        << SuccessMsg ("address", 25)
        << "Display your payment address"
        << std::endl
        << SuccessMsg ("exit", 25)
        << "Exit and save your wallet"
        << std::endl
        << SuccessMsg ("save", 25)
        << "Save your wallet state"
        << std::endl
        << SuccessMsg ("status", 25)
        << "Show daemon status"
        << std::endl
        << SuccessMsg ("incoming_transfers", 25)
        << "Show incoming transfers"
        << std::endl;

    if (viewWallet) {
        std::cout
            << InformationMsg ("Please note you are using a view only "
                               "wallet, and so cannot transfer " + WalletConfig::ticker + ".")
            << std::endl;
    } else {
        std::cout
            << SuccessMsg ("outgoing_transfers", 25)
            << "Show outgoing transfers"
            << std::endl
            << SuccessMsg ("list_transfers", 25)
            << "Show all transfers"
            << std::endl
            << SuccessMsg ("save_csv", 25)
            << "Save all wallet transactions to CSV file"
            << std::endl
            << SuccessMsg ("optimize", 25)
            << "Optimize your wallet to send large amounts"
            << std::endl
            << SuccessMsg ("transfer", 25)
            << "Send "
            << WalletConfig::ticker
            << " to someone"
            << std::endl;
    }
}

void status(CryptoNote::INode &node)
{
    std::cout
        << node.getInfo ()
        << std::endl;
}

void balance(CryptoNote::INode &node,
             CryptoNote::WalletGreen &wallet,
             bool viewWallet)
{
    uint64_t unconfirmedBalance = wallet.getPendingBalance ();
    uint64_t confirmedBalance = wallet.getActualBalance ();
    uint64_t totalBalance = unconfirmedBalance + confirmedBalance;

    uint32_t localHeight = node.getLastLocalBlockHeight ();
    uint32_t remoteHeight = node.getLastKnownBlockHeight ();
    uint32_t walletHeight = wallet.getBlockCount ();

    std::cout
        << "Available balance: "
        << SuccessMsg (formatAmount (confirmedBalance))
        << std::endl
        << "Locked (unconfirmed) balance: "
        << WarningMsg (formatAmount (unconfirmedBalance))
        << std::endl
        << "Total balance: "
        << InformationMsg (formatAmount (totalBalance))
        << std::endl;

    if (viewWallet) {
        std::cout
            << std::endl
            << InformationMsg ("Please note that view only wallets "
                               "can only track incoming transactions, "
                               "and so your wallet balance may appear "
                               "inflated.")
            << std::endl;
    }

    if (localHeight < remoteHeight) {
        std::cout
            << std::endl
            << InformationMsg ("Your daemon is not fully synced with "
                               "the network!")
            << std::endl
            << "Your balance may be incorrect until you "
            << "are fully synced!"
            << std::endl;
    }
        /*!
         * Small buffer because wallet height doesn't update instantly like node
         * height does
         */
    else if (walletHeight + 1000 < remoteHeight) {
        std::cout
            << std::endl
            << InformationMsg ("The blockchain is still being scanned for "
                               "your transactions.")
            << std::endl
            << "Balances might be incorrect whilst this is ongoing."
            << std::endl;
    }
}

void blockchainHeight(CryptoNote::INode &node, CryptoNote::WalletGreen &wallet)
{
    uint32_t localHeight = node.getLastLocalBlockHeight ();
    uint32_t remoteHeight = node.getLastKnownBlockHeight ();
    uint32_t walletHeight = wallet.getBlockCount ();

    /*!
     * This is the height that the wallet has been scanned to. The blockchain
     * can be fully updated, but we have to walk the chain to find our
     * transactions, and this number indicates that progress.
     */
    std::cout
        << "Wallet blockchain height: ";

    /*!
     * Small buffer because wallet height doesn't update instantly like node
     * height does
     */
    if (walletHeight + 1000 > remoteHeight) {
        std::cout
            << SuccessMsg (std::to_string (walletHeight));
    } else {
        std::cout
            << WarningMsg (std::to_string (walletHeight));
    }

    std::cout
        << std::endl
        << "Local blockchain height: ";

    if (localHeight == remoteHeight) {
        std::cout
            << SuccessMsg (std::to_string (localHeight));
    } else {
        std::cout
            << WarningMsg (std::to_string (localHeight));
    }

    std::cout
        << std::endl
        << "Network blockchain height: "
        << SuccessMsg (std::to_string (remoteHeight))
        << std::endl;

    if (localHeight == 0 && remoteHeight == 0) {
        std::cout
            << WarningMsg ("Uh oh, it looks like you don't have "
                           "QDaemon open!")
            << std::endl;
    } else if (walletHeight + 1000 < remoteHeight && localHeight == remoteHeight) {
        std::cout
            << InformationMsg ("You are synced with the network, but the "
                               "blockchain is still being scanned for "
                               "your transactions.")
            << std::endl
            << "Balances might be incorrect whilst this is ongoing."
            << std::endl;
    } else if (localHeight == remoteHeight) {
        std::cout
            << SuccessMsg ("Yay! You are synced!")
            << std::endl;
    } else {
        std::cout
            << WarningMsg ("Be patient, you are still syncing with the "
                           "network!")
            << std::endl;
    }
}

void reset(CryptoNote::INode &node, std::shared_ptr<WalletInfo> &walletInfo)
{
    uint64_t scanHeight = getScanHeight ();

    std::cout
        << InformationMsg ("This process may take some time to complete.")
        << std::endl
        << InformationMsg ("You can't make any transactions during the ")
        << InformationMsg ("process.")
        << std::endl
        << std::endl;

    if (!confirm ("Are you sure?")) {
        return;
    }

    std::cout
        << InformationMsg ("Resetting wallet...")
        << std::endl;

    walletInfo->wallet.reset(scanHeight);

    syncWallet (node, walletInfo);
}
