// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
// 
// Please see the included LICENSE file for more information.

#include <ctime>

#include <boost/format.hpp>

#include <CryptoNoteCore/Core.h>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>
#include <CryptoNoteCore/Currency.h>

#include <CryptoNoteProtocol/CryptoNoteProtocolHandler.h>

#include <Daemon/DaemonCommandsHandler.h>

#include <P2p/NetNode.h>

#include <Rpc/JsonRpc.h>

#include <Serialization/SerializationTools.h>

#include <Utilities/FormatTools.h>
#include <Utilities/ColouredMsg.h>

#include <version.h>

namespace {
    template<typename T>
    static bool printAsJson(const T &obj)
    {
        std::cout
            << CryptoNote::storeToJson (obj)
            << ENDL;
        return true;
    }

    std::string printTransactionShortInfo(const CryptoNote::CachedTransaction &transaction)
    {
        std::stringstream ss;

        ss
            << "id: "
            << transaction.getTransactionHash ()
            << std::endl;
        ss
            << "fee: "
            << transaction.getTransactionFee ()
            << std::endl;
        ss
            << "blobSize: "
            << transaction.getTransactionBinaryArray ().size ()
            << std::endl;

        return ss.str ();
    }

    std::string printTransactionFullInfo(const CryptoNote::CachedTransaction &transaction)
    {
        std::stringstream ss;
        ss
            << printTransactionShortInfo (transaction);
        ss
            << "JSON: \n"
            << CryptoNote::storeToJson (transaction.getTransaction ())
            << std::endl;

        return ss.str ();
    }

} // namespace

DaemonCommandsHandler::DaemonCommandsHandler(CryptoNote::Core &core,
                                             CryptoNote::NodeServer &srv,
                                             std::shared_ptr<Logging::LoggerManager> log,
                                             CryptoNote::RpcServer *prpc_server)
    : m_core (core),
      m_srv (srv),
      logger (log, "daemon"),
      m_logManager (log),
      m_prpc_server (prpc_server)
{
    m_consoleHandler.setHandler ("exit", boost::bind (&DaemonCommandsHandler::exit, this, _1), "Shutdown the daemon");
    m_consoleHandler.setHandler ("help", boost::bind (&DaemonCommandsHandler::help, this, _1), "Show this help");
    m_consoleHandler.setHandler ("print_pl",
                                 boost::bind (&DaemonCommandsHandler::printPl, this, _1),
                                 "Print peer list");
    m_consoleHandler.setHandler ("print_cn",
                                 boost::bind (&DaemonCommandsHandler::printCn, this, _1),
                                 "Print connections");
    m_consoleHandler.setHandler ("print_bc",
                                 boost::bind (&DaemonCommandsHandler::printBc, this, _1),
                                 "Print blockchain info in a given blocks range, print_bc <begin_height> [<end_height>]");
    m_consoleHandler.setHandler ("print_block",
                                 boost::bind (&DaemonCommandsHandler::printBlock, this, _1),
                                 "Print block, print_block <block_hash> | <block_height>");
    m_consoleHandler.setHandler ("print_tx",
                                 boost::bind (&DaemonCommandsHandler::printTx, this, _1),
                                 "Print transaction, print_tx <transaction_hash>");
    m_consoleHandler.setHandler ("print_pool",
                                 boost::bind (&DaemonCommandsHandler::printPool, this, _1),
                                 "Print transaction pool (long format)");
    m_consoleHandler.setHandler ("print_pool_sh",
                                 boost::bind (&DaemonCommandsHandler::printPoolSh, this, _1),
                                 "Print transaction pool (short format)");
    m_consoleHandler.setHandler ("set_log",
                                 boost::bind (&DaemonCommandsHandler::setLog, this, _1),
                                 "set_log <level> - Change current log level, <level> is a number 0-4");
    m_consoleHandler.setHandler ("status",
                                 boost::bind (&DaemonCommandsHandler::status, this, _1),
                                 "Show daemon status");
    m_consoleHandler.setHandler ("ban_ip",
                                 boost::bind (&DaemonCommandsHandler::ip_ban, this, _1),
                                 "Bans the provided ips from the p2p interface.");
    m_consoleHandler.setHandler ("unban_ip",
                                 boost::bind (&DaemonCommandsHandler::ip_unban, this, _1),
                                 "Unbans the provided ips from the p2p interface.");
    m_consoleHandler.setHandler ("unban_all_ip",
                                 boost::bind (&DaemonCommandsHandler::ip_unban_all, this, _1),
                                 "Removes all bans from the p2p interface.");
}

std::string DaemonCommandsHandler::getCommandsStr()
{
    std::stringstream ss;
    ss
        << CryptoNote::CRYPTONOTE_NAME
        << " v"
        << PROJECT_VERSION_LONG
        << ENDL;
    ss
        << "Commands: "
        << ENDL;
    std::string usage = m_consoleHandler.getUsage ();
    boost::replace_all (usage, "\n", "\n  ");
    usage.insert (0, "  ");
    ss
        << usage
        << ENDL;
    return ss.str ();
}

bool DaemonCommandsHandler::exit(const std::vector<std::string> &args)
{
    std::cout
        << InformationMsg ("================= EXITING ==================\n"
                           "== PLEASE WAIT, THIS MAY TAKE A LONG TIME ==\n"
                           "============================================\n");

    /*!
     * Set log to max when exiting. Sometimes this takes a while, and it helps
     * to let users know the daemon is still doing stuff
     */
    m_logManager->setMaxLevel (Logging::TRACE);
    m_consoleHandler.requestStop ();
    m_srv.sendStopSignal ();
    return true;
}

bool DaemonCommandsHandler::help(const std::vector<std::string> &args)
{
    std::cout
        << getCommandsStr ()
        << ENDL;
    return true;
}

bool DaemonCommandsHandler::printPl(const std::vector<std::string> &args)
{
    m_srv.logPeerlist ();
    return true;
}

bool DaemonCommandsHandler::printCn(const std::vector<std::string> &args)
{
    m_srv.getPayloadObject ().logConnections ();
    return true;
}

bool DaemonCommandsHandler::printBc(const std::vector<std::string> &args)
{
    if (!args.size ()) {
        std::cout
            << "need block index parameter"
            << ENDL;
        return false;
    }

    uint32_t start_index = 0;
    uint32_t end_index = 0;
    uint32_t end_block_parametr = m_core.getTopBlockIndex ();

    if (!Common::fromString (args[0], start_index)) {
        std::cout
            << "wrong starter block index parameter"
            << ENDL;
        return false;
    }

    if (args.size () > 1 && !Common::fromString (args[1], end_index)) {
        std::cout
            << "wrong end block index parameter"
            << ENDL;
        return false;
    }

    if (end_index == 0) {
        end_index = start_index;
    }

    if (end_index > end_block_parametr) {
        std::cout
            << "end block index parameter shouldn't be greater than "
            << end_block_parametr
            << ENDL;
        return false;
    }

    if (end_index < start_index) {
        std::cout
            << "end block index should be greater than or equal to starter block index"
            << ENDL;
        return false;
    }

    CryptoNote::COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::request req;
    CryptoNote::COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::response res;
    CryptoNote::JsonRpc::JsonRpcError error_resp;

    req.start_height = start_index;
    req.end_height = end_index;

    // TODO: implement m_is_rpc handling like in monero?
    if (!m_prpc_server->onGetBlockHeadersRange (req, res, error_resp) || res.status != CORE_RPC_STATUS_OK) {
        // TODO res.status handling
        std::cout
            << "Response status not CORE_RPC_STATUS_OK"
            << ENDL;
        return false;
    }

    const CryptoNote::Currency &currency = m_core.getCurrency ();

    bool first = true;
    for (CryptoNote::BlockHeaderResponse &header : res.headers) {
        if (!first) {
            std::cout
                << ENDL;
            first = false;
        }

        std::cout
            << "height: "
            << header.height
            << ", timestamp: "
            << header.timestamp
            << ", difficulty: "
            << header.difficulty
            << ", size: "
            << header.block_size
            << ", transactions: "
            << header.num_txes
            << ENDL
            << "major version: "
            << unsigned (header.major_version)
            << ", minor version: "
            << unsigned (header.minor_version)
            << ENDL
            << "block id: "
            << header.hash
            << ", previous block id: "
            << header.prev_hash
            << ENDL
            << "difficulty: "
            << header.difficulty
            << ", nonce: "
            << header.nonce
            << ", reward: "
            << currency.formatAmount (header.reward)
            << ENDL;
    }

    return true;
}

bool DaemonCommandsHandler::setLog(const std::vector<std::string> &args)
{
    if (args.size () != 1) {
        std::cout
            << "use: set_log <log_level_number_0-4>"
            << ENDL;
        return true;
    }

    uint16_t l = 0;
    if (!Common::fromString (args[0], l)) {
        std::cout
            << "wrong number format, use: set_log <log_level_number_0-4>"
            << ENDL;
        return true;
    }

    ++l;

    if (l > Logging::TRACE) {
        std::cout
            << "wrong number range, use: set_log <log_level_number_0-4>"
            << ENDL;
        return true;
    }

    m_logManager->setMaxLevel (static_cast<Logging::Level>(l));
    return true;
}

bool DaemonCommandsHandler::printBlockByHeight(uint32_t height)
{
    if (height - 1 > m_core.getTopBlockIndex ()) {
        std::cout
            << "block wasn't found. Current block chain height: "
            << m_core.getTopBlockIndex () + 1
            << ", requested: "
            << height
            << std::endl;
        return false;
    }

    auto hash = m_core.getBlockHashByIndex (height - 1);
    std::cout
        << "block_id: "
        << hash
        << ENDL;
    printAsJson (m_core.getBlockByIndex (height - 1));

    return true;
}

bool DaemonCommandsHandler::printBlockByHash(const std::string &arg)
{
    Crypto::Hash block_hash;
    if (!parseHash256 (arg, block_hash)) {
        return false;
    }

    if (m_core.hasBlock (block_hash)) {
        printAsJson (m_core.getBlockByHash (block_hash));
    } else {
        std::cout
            << "block wasn't found: "
            << arg
            << std::endl;
        return false;
    }

    return true;
}

bool DaemonCommandsHandler::printBlock(const std::vector<std::string> &args)
{
    if (args.empty ()) {
        std::cout
            << "expected: print_block (<block_hash> | <block_height>)"
            << std::endl;
        return true;
    }

    const std::string &arg = args.front ();
    try {
        uint32_t height = boost::lexical_cast<uint32_t> (arg);
        printBlockByHeight (height);
    } catch (boost::bad_lexical_cast &) {
        printBlockByHash (arg);
    }

    return true;
}

bool DaemonCommandsHandler::printTx(const std::vector<std::string> &args)
{
    if (args.empty ()) {
        std::cout
            << "expected: print_tx <transaction hash>"
            << std::endl;
        return true;
    }

    const std::string &str_hash = args.front ();
    Crypto::Hash tx_hash;
    if (!parseHash256 (str_hash, tx_hash)) {
        return true;
    }

    std::vector<Crypto::Hash> tx_ids;
    tx_ids.push_back (tx_hash);
    std::vector<CryptoNote::BinaryArray> txs;
    std::vector<Crypto::Hash> missed_ids;
    m_core.getTransactions (tx_ids, txs, missed_ids);

    if (1 == txs.size ()) {
        CryptoNote::CachedTransaction tx (txs.front ());
        printAsJson (tx.getTransaction ());
    } else {
        std::cout
            << "transaction wasn't found: <"
            << str_hash
            << '>'
            << std::endl;
    }

    return true;
}

bool DaemonCommandsHandler::printPool(const std::vector<std::string> &args)
{
    std::cout
        << "Pool state: \n";
    auto pool = m_core.getPoolTransactions ();

    for (const auto &tx: pool) {
        CryptoNote::CachedTransaction ctx (tx);
        std::cout
            << printTransactionFullInfo (ctx)
            << "\n";
    }

    std::cout
        << std::endl;

    return true;
}

bool DaemonCommandsHandler::printPoolSh(const std::vector<std::string> &args)
{
    std::cout
        << "Pool short state: \n";
    auto pool = m_core.getPoolTransactions ();

    for (const auto &tx: pool) {
        CryptoNote::CachedTransaction ctx (tx);
        std::cout
            << printTransactionShortInfo (ctx)
            << "\n";
    }

    std::cout
        << std::endl;

    return true;
}

bool DaemonCommandsHandler::status(const std::vector<std::string> &args)
{
    CryptoNote::COMMAND_RPC_GET_INFO::request ireq;
    CryptoNote::COMMAND_RPC_GET_INFO::response iresp;

    if (!m_prpc_server->onGetInfo (ireq, iresp) || iresp.status != CORE_RPC_STATUS_OK) {
        std::cout
            << "Problem retrieving information from RPC server."
            << std::endl;
        return false;
    }

    std::cout
        << Utilities::getStatusString (iresp)
        << std::endl;

    return true;
}

bool DaemonCommandsHandler::ip_ban(const std::vector<std::string> &args)
{
    std::vector<uint32_t> ips;
    ips.resize (args.size ());
    for (size_t i = 0; i < args.size (); ++i) {
        if (!Common::parseIpAddress (ips[i], args[i])) {
            std::cout
                << "Invalid ip address: "
                << args[i]
                << std::endl;
            return false;
        }
    }
    for (const auto ip : ips) {
        m_srv.getPayloadObject ().ban (ip);
    }
    return true;
}

bool DaemonCommandsHandler::ip_unban(const std::vector<std::string> &args)
{
    std::vector<uint32_t> ips;
    ips.resize (args.size ());
    for (size_t i = 0; i < args.size (); ++i) {
        if (!Common::parseIpAddress (ips[i], args[i])) {
            std::cout
                << "Invalid ip address: "
                << args[i]
                << std::endl;
            return false;
        }
    }
    for (const auto ip : ips) {
        m_srv.getPayloadObject ().unban (ip);
    }
    return true;
}

bool DaemonCommandsHandler::ip_unban_all(const std::vector<std::string> &args)
{
    m_srv.getPayloadObject ().unbanAll ();
    return true;
}
