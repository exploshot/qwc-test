// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
// 
// Please see the included LICENSE file for more information.

#pragma once

#include <Common/ConsoleHandler.h>

#include <Logging/LoggerRef.h>
#include <Logging/LoggerManager.h>

#include <Rpc/RpcServer.h>
#include <Rpc/CoreRpcServerCommandsDefinitions.h>
#include <Rpc/JsonRpc.h>

namespace CryptoNote {
    class Core;
    class NodeServer;
} // namespace CryptoNote

class DaemonCommandsHandler
{
public:
    DaemonCommandsHandler(CryptoNote::Core &core,
                          CryptoNote::NodeServer &srv,
                          std::shared_ptr<Logging::LoggerManager> log,
                          CryptoNote::RpcServer *prpc_server);

    bool start_handling()
    {
        m_consoleHandler.start ();
        return true;
    }

    void stop_handling()
    {
        m_consoleHandler.stop ();
    }

    bool exit(const std::vector<std::string> &args);

private:

    Common::ConsoleHandler m_consoleHandler;
    CryptoNote::Core &m_core;
    CryptoNote::NodeServer &m_srv;
    Logging::LoggerRef logger;
    std::shared_ptr<Logging::LoggerManager> m_logManager;
    CryptoNote::RpcServer *m_prpc_server;

    std::string getCommandsStr();
    bool printBlockByHeight(uint32_t height);
    bool printBlockByHash(const std::string &arg);

    bool help(const std::vector<std::string> &args);
    bool printPl(const std::vector<std::string> &args);
    bool show_hr(const std::vector<std::string> &args);
    bool hide_hr(const std::vector<std::string> &args);
    bool print_bc_outs(const std::vector<std::string> &args);
    bool printCn(const std::vector<std::string> &args);
    bool printBc(const std::vector<std::string> &args);
    bool print_bci(const std::vector<std::string> &args);
    bool setLog(const std::vector<std::string> &args);
    bool printBlock(const std::vector<std::string> &args);
    bool printTx(const std::vector<std::string> &args);
    bool printPool(const std::vector<std::string> &args);
    bool printPoolSh(const std::vector<std::string> &args);
    bool start_mining(const std::vector<std::string> &args);
    bool stop_mining(const std::vector<std::string> &args);
    bool status(const std::vector<std::string> &args);

    /*!
     * ip banning
     */
    bool ip_ban(const std::vector<std::string> &args);
    bool ip_unban(const std::vector<std::string> &args);
    bool ip_unban_all(const std::vector<std::string> &args);
};
