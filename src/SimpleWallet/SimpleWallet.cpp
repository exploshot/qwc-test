// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The Qwertycoin Project
// Copyright (c) 2014-2016, XDN developers
// Copyright (c) 2014-2017, The Monero Project
// Copyright (c) 2016-2018, The Karbo developers
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <Common/SignalHandler.h>
#include <Common/StringTools.h>

#include <CryptoNoteCore/Currency.h>

#include <Global/CliHeader.h>

#include <Logging/FileLogger.h>
#include <Logging/LoggerManager.h>

#include <NodeRpcProxy/NodeRpcProxy.h>

#include <SimpleWallet/Commands.h>
#include <SimpleWallet/Fusion.h>
#include <SimpleWallet/Open.h>
#include <SimpleWallet/ParseArguments.h>
#include <SimpleWallet/SimpleWallet.h>
#include <SimpleWallet/Sync.h>
#include <SimpleWallet/Tools.h>
#include <SimpleWallet/Transfer.h>

#include <Utilities/ColouredMsg.h>

#ifdef _WIN32
/* Prevents windows.h redefining min/max which breaks compilation */
#define NOMINMAX
#include <windows.h>
#endif

#include <version.h>

int main(int argc, char **argv)
{
#ifdef _WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

    Config config = parseArguments(argc, argv);

    std::cout << InformationMsg(CryptoNote::getProjectCLIHeader()) << std::endl;

    const auto logManager = std::make_shared<Logging::LoggerManager>();

    if (config.debug) {
        logManager->setMaxLevel(Logging::DEBUGGING);

        Logging::FileLogger fileLogger;

        fileLogger.init(WalletConfig::walletName + ".log");
        logManager->addLogger(fileLogger);
    }

    const CryptoNote::Currency currency = CryptoNote::CurrencyBuilder(logManager).currency();

    System::Dispatcher localDispatcher;
    System::Dispatcher *dispatcher = &localDispatcher;

    /*!
     * Connection to QDaemon
     */
    std::unique_ptr<CryptoNote::INode> node(
        new CryptoNote::NodeRpcProxy(config.host, config.port, 1, logManager)
    );

    std::promise<std::error_code> errorPromise;

    auto callback = [&errorPromise](std::error_code e)
    {
        errorPromise.set_value(e);
    };

    auto initNode = errorPromise.get_future();

    node->init(callback);

    if (initNode.wait_for(std::chrono::seconds(5)) != std::future_status::ready) {
        if (config.host != "127.0.0.1") {
            std::cout << WarningMsg(
                "Unable to connect to remote node, "
                "connection timed out."
            )
                      << std::endl
                      << WarningMsg(
                          "Confirm the remote node is functioning, "
                          "or try a different remote node."
                      )
                      << std::endl << std::endl;
        }
        else {
            std::cout << WarningMsg(
                "Unable to connect to node, "
                "connection timed out."
            )
                      << std::endl << std::endl;
        }
    }

    if (node->feeAmount() != 0 && !node->feeAddress().empty()) {
        std::stringstream feeMsg;

        feeMsg << std::endl << "You have connected to a node that charges " <<
               "a fee to send transactions." << std::endl << std::endl
               << "The fee for sending transactions is: " <<
               formatAmount(node->feeAmount()) <<
               " per transaction." << std::endl << std::endl <<
               "If you don't want to pay the node fee, please " <<
               "relaunch " << WalletConfig::walletName <<
               " and specify a different node or run your own." <<
               std::endl;

        std::cout << WarningMsg(feeMsg.str()) << std::endl;
    }

    CryptoNote::WalletGreen wallet(*dispatcher, currency, *node, logManager);

    run(wallet, *node, config);
}

void run(CryptoNote::WalletGreen &wallet,
         CryptoNote::INode &node,
         Config &config)
{
    auto maybeWalletInfo = Nothing<std::shared_ptr<WalletInfo>>();
    Action action;

    do {
        std::cout
            << InformationMsg(
                WalletConfig::coinName + " v"
                    + std::string(PROJECT_VERSION)
                    + " " + WalletConfig::walletName
            )
            << std::endl;

        /*!
         * Open/import/generate the wallet
         */
        action = getAction(config);
        maybeWalletInfo = handleAction(wallet, action, config);

        /*!
         * Didn't manage to get the wallet info, returning to selection screen
         */
    }
    while (!maybeWalletInfo.isJust);

    auto walletInfo = maybeWalletInfo.x;

    bool alreadyShuttingDown = false;

    /*!
     * This will call shutdown when ctrl+c is hit. This is a lambda function,
     * & means capture all variables by reference
     */
    Tools::SignalHandler::install(
        [&]
        {
            /*!
             * If we're already shutting down let control flow continue as normal
             */
            if (shutdown(walletInfo->wallet, node, alreadyShuttingDown)) {
                exit(0);
            }
        }
    );

    while (node.getLastKnownBlockHeight() == 0) {
        std::cout
            << WarningMsg("It looks like QDaemon isn't open!")
            << std::endl
            << std::endl
            << WarningMsg(
                "Ensure QDaemon is open and has finished "
                "initializing."
            )
            << std::endl
            << WarningMsg(
                "If it's still not working, try restarting "
                "QDaemon. The daemon sometimes gets stuck."
            )
            << std::endl
            << WarningMsg(
                "Alternatively, perhaps QDaemon can't "
                "communicate with any peers."
            )
            << std::endl
            << std::endl
            << WarningMsg(
                "The wallet can't function until it can "
                "communicate with the network."
            )
            << std::endl
            << std::endl;

        bool proceed = false;

        while (true) {
            std::cout
                << "["
                << InformationMsg("T")
                << "]ry again, "
                << "["
                << InformationMsg("E")
                << "]xit, or "
                << "["
                << InformationMsg("C")
                << "]ontinue anyway?: ";

            std::string answer;
            std::getline(std::cin, answer);

            char c = std::tolower(answer[0]);

            /*!
             * Lets people spam enter in the transaction screen
             */
            if (c == 't' || c == '\0') {
                break;
            }
            else if (c == 'e' || c == std::ifstream::traits_type::eof()) {
                shutdown(walletInfo->wallet, node, alreadyShuttingDown);
                return;
            }
            else if (c == 'c') {
                proceed = true;
                break;
            }
            else {
                std::cout
                    << WarningMsg("Bad input: ")
                    << InformationMsg(answer)
                    << WarningMsg(" - please enter either T, E, or C.")
                    << std::endl;
            }
        }

        if (proceed) {
            break;
        }

        std::cout
            << std::endl;
    }

    /*!
     * Scan the chain for new transactions. In the case of an imported
     * wallet, we need to scan the whole chain to find any transactions.
     * If we opened the wallet however, we just need to scan from when we
     * last had it open. If we are generating a wallet, there is no need
     * to check for transactions as there is no way the wallet can have
     * received any money yet.
     */
    if (action != Generate) {
        syncWallet(node, walletInfo);
    }
    else {
        std::cout
            << InformationMsg(
                "Your wallet is syncing with the "
                "network in the background."
            )
            << std::endl
            << InformationMsg(
                "Until this is completed new "
                "transactions might not show up."
            )
            << std::endl
            << InformationMsg("Use bc_height to check the progress.")
            << std::endl
            << std::endl;
    }

    welcomeMsg();

    inputLoop(walletInfo, node);

    shutdown(walletInfo->wallet, node, alreadyShuttingDown);
}

Maybe<std::shared_ptr<WalletInfo>> handleAction(CryptoNote::WalletGreen &wallet,
                                                Action action, Config &config)
{
    if (action == Generate) {
        return Just<std::shared_ptr<WalletInfo>>(generateWallet(wallet));
    }
    else if (action == Open) {
        return openWallet(wallet, config);
    }
    else if (action == Import) {
        return Just<std::shared_ptr<WalletInfo>>(importWallet(wallet));
    }
    else if (action == SeedImport) {
        return Just<std::shared_ptr<WalletInfo>>(mnemonicImportWallet(wallet));
    }
    else if (action == ViewWallet) {
        return Just<std::shared_ptr<WalletInfo>>(createViewWallet(wallet));
    }
    else {
        throw std::runtime_error("Unimplemented action!");
    }
}

Action getAction(Config &config)
{
    if (config.walletGiven || config.passGiven) {
        return Open;
    }

    while (true) {
        std::cout
            << std::endl
            << "Welcome, please choose an option below:"
            << std::endl
            << std::endl

            << "\t["
            << InformationMsg("G")
            << "] - "
            << "Generate a new wallet address"
            << std::endl

            << "\t["
            << InformationMsg("O")
            << "] - "
            << "Open a wallet already on your system"
            << std::endl

            << "\t["
            << InformationMsg("S")
            << "] - "
            << "Regenerate your wallet using a seed phrase of words"
            << std::endl

            << "\t["
            << InformationMsg("I")
            << "] - "
            << "Import your wallet using a View Key and Spend Key"
            << std::endl

            << "\t["
            << InformationMsg("V")
            << "] - "
            << "Import a view only wallet (Unable to send transactions)"
            << std::endl
            << std::endl

            << "or, press CTRL_C to exit: ";

        std::string answer;
        std::getline(std::cin, answer);

        char c = answer[0];
        c = std::tolower(c);

        if (c == 'o') {
            return Open;
        }
        else if (c == 'g') {
            return Generate;
        }
        else if (c == 'i') {
            return Import;
        }
        else if (c == 's') {
            return SeedImport;
        }
        else if (c == 'v') {
            return ViewWallet;
        }
        else {
            std::cout
                << "Unknown command: "
                << WarningMsg(answer)
                << std::endl;
        }
    }
}

void welcomeMsg()
{
    std::cout
        << "Use the "
        << SuggestionMsg("help")
        << " command to see the list of available commands."
        << std::endl
        << "Use "
        << SuggestionMsg("exit")
        << " when closing to ensure your wallet file doesn't get "
        << "corrupted."
        << std::endl
        << std::endl;
}

std::string getInputAndDoWorkWhileIdle(std::shared_ptr<WalletInfo> &walletInfo)
{
    auto lastUpdated = std::chrono::system_clock::now();

    std::future<std::string> inputGetter = std::async(
        std::launch::async, []
        {
            std::string command;
            std::getline(std::cin, command);
            boost::algorithm::trim(command);
            return command;
        }
    );

    while (true) {
        /*!
         * Check if the user has inputted something yet (Wait for zero seconds
         * to instantly return)
         */
        std::future_status status = inputGetter.wait_for(std::chrono::seconds(0));

        /*!
         * User has inputted, get what they inputted and return it
         */
        if (status == std::future_status::ready) {
            return inputGetter.get();
        }

        auto currentTime = std::chrono::system_clock::now();

        /*!
         * Otherwise check if we need to update the wallet cache
         */
        if ((currentTime - lastUpdated) > std::chrono::seconds(5)) {
            lastUpdated = currentTime;
            checkForNewTransactions(walletInfo);
        }

        /*!
         * Sleep for enough for it to not be noticeable when the user enters
         * something, but enough that we're not starving the CPU
         */
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

bool shutdown(CryptoNote::WalletGreen &wallet, CryptoNote::INode &node,
              bool &alreadyShuttingDown)
{
    if (alreadyShuttingDown) {
        std::cout
            << "I do appreciate your patience so far, we're already shutting down!"
            << std::endl;
        return false;
    }
    else {
        alreadyShuttingDown = true;
        std::cout
            << InformationMsg(
                "Saving wallet and shutting down, please "
                "wait..."
            )
            << std::endl;
    }

    bool finishedShutdown = false;

    boost::thread timelyShutdown(
        [&finishedShutdown]
        {
            auto startTime = std::chrono::system_clock::now();

            /*!
             * Has shutdown finished?
             */
            while (!finishedShutdown) {
                auto currentTime = std::chrono::system_clock::now();

                /*!
                 * If not, wait for a max of 20 seconds then force exit.
                 */
                if ((currentTime - startTime) > std::chrono::seconds(20)) {
                    std::cout
                        << WarningMsg(
                            "Wallet took too long to save! "
                            "Force closing."
                        )
                        << std::endl
                        << "Bye."
                        << std::endl;
                    exit(0);
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    );

    wallet.save();
    wallet.shutdown();
    node.shutdown();

    finishedShutdown = true;

    /*!
     * Wait for shutdown watcher to finish
     */
    timelyShutdown.join();

    std::cout
        << "Bye."
        << std::endl;

    return true;
}

void inputLoop(std::shared_ptr<WalletInfo> &walletInfo, CryptoNote::INode &node)
{
    while (true) {
        std::cout
            << getPrompt(walletInfo);

        std::string command = getInputAndDoWorkWhileIdle(walletInfo);

        if (command == "") {
            // no-op
        }
        else if (command == "export_keys") {
            exportKeys(walletInfo);
        }
        else if (command == "help") {
            help(walletInfo->viewWallet);
        }
        else if (command == "status") {
            status(node);
        }
        else if (command == "balance") {
            balance(node, walletInfo->wallet, walletInfo->viewWallet);
        }
        else if (command == "address") {
            std::cout
                << SuccessMsg(walletInfo->walletAddress)
                << std::endl;
        }
        else if (command == "incoming_transfers") {
            listTransfers(true, false, walletInfo->wallet, node);
        }
        else if (command == "save_csv") {
            saveCSV(walletInfo->wallet, node);
        }
        else if (command == "exit") {
            return;
        }
        else if (command == "save") {
            std::cout
                << InformationMsg("Saving.")
                << std::endl;
            walletInfo->wallet.save();
            std::cout
                << InformationMsg("Saved.")
                << std::endl;
        }
        else if (command == "bc_height") {
            blockchainHeight(node, walletInfo->wallet);
        }
        else if (command == "reset") {
            reset(node, walletInfo);
        }
        else if (!walletInfo->viewWallet) {
            if (command == "outgoing_transfers") {
                listTransfers(false, true, walletInfo->wallet, node);
            }
            else if (command == "list_transfers") {
                listTransfers(true, true, walletInfo->wallet, node);
            }
            else if (command == "list_messages") {
                listMessages(walletInfo->wallet, node);
            }
            else if (command == "transfer") {
                transfer(walletInfo, node.getLastKnownBlockHeight());
            }
                /*!
                 * String starts with transfer, old transfer syntax
                 */
            else if (command.find("transfer") == 0) {
                std::cout
                    << "This transfer syntax has been removed."
                    << std::endl
                    << "Run just the "
                    << SuggestionMsg("transfer")
                    << " command for a walk through guide to "
                    << "transferring."
                    << std::endl;
            }
            else if (command == "quick_optimize") {
                quickOptimize(walletInfo->wallet);
            }
            else if (command == "full_optimize" || command == "optimize") {
                fullOptimize(walletInfo->wallet);
            }
            else {
                std::cout
                    << "Unknown command: "
                    << WarningMsg(command)
                    << ", use "
                    << SuggestionMsg("help")
                    << " command to list all possible commands."
                    << std::endl;
            }
        }
        else {
            std::cout
                << "Unknown command: "
                << WarningMsg(command)
                << ", use "
                << SuggestionMsg("help")
                << " command to list all possible commands."
                << std::endl
                << "Please note some commands such as transfer are "
                << "unavailable, as you are using a view only wallet."
                << std::endl;
        }
    }
}
