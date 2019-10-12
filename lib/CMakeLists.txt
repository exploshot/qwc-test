# Generate version.h header
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/Global/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)
message(STATUS "Generated version.h header: ${CMAKE_CURRENT_BINARY_DIR}/version.h")

set(QwertycoinFramework_INCLUDE_DIRS
    "${Boost_INCLUDE_DIRS}"
    "${MINIUPNP_INCLUDE_DIRS}"
    "${RAPIDJSON_INCLUDE_DIRS}"
    "${ROCKSDB_INCLUDE_DIRS}"
    "${SPARSEHASH_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../include"
)

if(MSVC)
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows")
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/msc")
elseif(APPLE)
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX")
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
elseif(ANDROID)
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Android")
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
elseif(FREEBSD)
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD")
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
else()
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux")
    list(APPEND QwertycoinFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
endif()

set(QwertycoinFramework_INCLUDE_DIRS ${QwertycoinFramework_INCLUDE_DIRS} PARENT_SCOPE)

# QwertycoinFramework

set(QwertycoinFramework_LIBS
    QwertycoinFramework_BlockchainExplorer
    QwertycoinFramework_Common
    QwertycoinFramework_Crypto
    QwertycoinFramework_CryptoNoteCore
    QwertycoinFramework_CryptoNoteProtocol
    QwertycoinFramework_Errors
    QwertycoinFramework_Global
    QwertycoinFramework_Http
    QwertycoinFramework_JsonRpcServer
    QwertycoinFramework_Logger
    QwertycoinFramework_Logging
    QwertycoinFramework_Mnemonics
    QwertycoinFramework_Nigel
    QwertycoinFramework_NodeRpcProxy
    QwertycoinFramework_P2p
    QwertycoinFramework_Rpc
    QwertycoinFramework_Serialization
    QwertycoinFramework_SubWallets
    QwertycoinFramework_System
    QwertycoinFramework_Transfers
    QwertycoinFramework_Wallet
    QwertycoinFramework_WalletBackend
    QwertycoinFramework_WalletService
    QwertycoinFramework_ZedWallet
)

add_custom_target(QwertycoinFramework)
add_dependencies(QwertycoinFramework ${QwertycoinFramework_LIBS})

# QwertycoinFramework::BlockchainExplorer

set(QwertycoinFramework_BlockchainExplorer_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorer.h"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerErrors.h"
)

set(QwertycoinFramework_BlockchainExplorer_LIBS
    QwertycoinFramework::Global
)

add_library(QwertycoinFramework_BlockchainExplorer STATIC ${QwertycoinFramework_BlockchainExplorer_SOURCES})
add_library(QwertycoinFramework::BlockchainExplorer ALIAS QwertycoinFramework_BlockchainExplorer)
target_include_directories(QwertycoinFramework_BlockchainExplorer PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_BlockchainExplorer PRIVATE ${QwertycoinFramework_BlockchainExplorer_LIBS})

# QwertycoinFramework::Common

set(QwertycoinFramework_Common_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Common/ArrayRef.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ArrayView.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Base58.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Base58.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/BlockingQueue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/BlockingQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/CheckDifficulty.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/CheckDifficulty.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/CryptoNoteTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/CryptoNoteTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/FileMappedVector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/FileMappedVector.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/FileSystemShim.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/IInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/int-util.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/IOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/JsonValue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/JsonValue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Lazy.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Math.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Math.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/MemoryInputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/MemoryInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ObserverManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/PathTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/PathTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/pod-class.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ScopeExit.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ScopeExit.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ShuffleGenerator.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/SignalHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/SignalHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdInputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdOutputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StreamTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StreamTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringBuffer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringInputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringOutputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringView.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/TransactionExtra.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/TransactionExtra.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Util.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Util.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Varint.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/VectorOutputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/VectorOutputStream.h"
)

set(QwertycoinFramework_Common_LIBS
    Boost::filesystem
    stdc++fs
    Threads::Threads
    QwertycoinFramework::Crypto
    QwertycoinFramework::Global
    QwertycoinFramework::Serialization
)

add_library(QwertycoinFramework_Common STATIC ${QwertycoinFramework_Common_SOURCES})
add_library(QwertycoinFramework::Common ALIAS QwertycoinFramework_Common)
target_include_directories(QwertycoinFramework_Common PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Common PRIVATE ${QwertycoinFramework_Common_LIBS})

if(UNIX AND NOT ANDROID)
    target_link_libraries(QwertycoinFramework_Common PUBLIC -lresolv)
endif()

# QwertycoinFramework::Crypto

set(QwertycoinFramework_Crypto_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/aesb.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/blake256.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/blake256.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/chacha8.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/chacha8.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/crypto-ops-data.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/crypto-ops.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/crypto-ops.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/crypto.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/crypto.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/groestl_tables.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/groestl.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/groestl.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash-extra-blake.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash-extra-groestl.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash-extra-jh.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash-extra-skein.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash-ops.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/hash.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/initializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/jh.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/jh.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/keccak.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/keccak.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/oaes_config.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/oaes_lib.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/oaes_lib.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/random.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/skein_port.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/skein.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/skein.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/slow-hash-arm.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/slow-hash-common.h"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/slow-hash-portable.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/slow-hash-x86.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/tree-hash.c"
    "${CMAKE_CURRENT_LIST_DIR}/Crypto/variant2_int_sqrt.h"
)

add_library(QwertycoinFramework_Crypto STATIC ${QwertycoinFramework_Crypto_SOURCES})
add_library(QwertycoinFramework::Crypto ALIAS QwertycoinFramework_Crypto)
target_include_directories(QwertycoinFramework_Crypto PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})

if(NOT WIN32 AND NOT ARM AND NOT PPC64LE)
    message(STATUS "[QwertycoinFramework::Crypto] AES support enabled.")
    target_compile_options(QwertycoinFramework_Crypto PRIVATE -maes)
elseif(PPC64LE)
    message(STATUS "[QwertycoinFramework::Crypto] AES support disabled.")
endif()

# QwertycoinFramework::CryptoNoteCore

set(QwertycoinFramework_CryptoNoteCore_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainCache.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainCache.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainMessages.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainMessages.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainReadBatch.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainReadBatch.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainStorage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainWriteBatch.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/BlockchainWriteBatch.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/CachedBlock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/CachedBlock.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/IBlockchainCache.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/IBlockchainCacheFactory.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/IBlockchainStorageObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/IMainChainStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MainChainStorage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MainChainStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MainChainStorageRocksdb.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MainChainStorageRocksdb.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MainChainStorageSqlite.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MainChainStorageSqlite.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MemoryBlockchainCacheFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MemoryBlockchainCacheFactory.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MemoryBlockchainStorage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/MemoryBlockchainStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/SwappedBlockchainStorage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain/SwappedBlockchainStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DatabaseBlockchainCache.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DatabaseBlockchainCache.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DatabaseBlockchainCacheFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DatabaseBlockchainCacheFactory.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DatabaseCacheData.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DatabaseCacheData.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DataBaseConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DataBaseConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DataBaseErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DataBaseErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DBUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/DBUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/RocksDBWrapper.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Database/RocksDBWrapper.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/CachedTransaction.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/CachedTransaction.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/ITransactionPool.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/ITransactionPoolCleaner.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/ITransactionValidator.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/ITxPoolObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/Transaction.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionApi.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPool.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPoolCleaner.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPoolCleaner.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPoolMessages.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPoolMessages.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionPrefixImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionValidationErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionValidationErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionValidatiorState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transactions/TransactionValidatiorState.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Account.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Account.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/AddBlockErrorCondition.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/AddBlockErrorCondition.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/AddBlockErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/AddBlockErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockValidationErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockValidationErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Checkpoints.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Checkpoints.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Core.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Core.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CoreErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CoreErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CoreStatistics.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasic.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasic.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasicImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasicImpl.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteFormatUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteFormatUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Currency.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Currency.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Difficulty.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Difficulty.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ICore.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ICoreDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ICoreInformation.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ICoreObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IKeyValueStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IMinerHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IntrusiveLinkedList.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ITimeProvider.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IUpgradeDetector.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IUpgradeManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/MessageQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Mixins.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/SwappedVector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/SwappedVector.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/UpgradeDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/UpgradeDetector.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/UpgradeManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/UpgradeManager.h"
)

set(QwertycoinFramework_CryptoNoteCore_LIBS
    Boost::filesystem
    Boost::program_options
    RocksDB::rocksdb
    sparsehash::sparsehash
    sqlite3::sqlite3
    stdc++fs
    QwertycoinFramework::BlockchainExplorer
    QwertycoinFramework::Common
    QwertycoinFramework::Crypto
    QwertycoinFramework::Global
    QwertycoinFramework::Logging
    QwertycoinFramework::Serialization
    QwertycoinFramework::Utilities
)

add_library(QwertycoinFramework_CryptoNoteCore STATIC ${QwertycoinFramework_CryptoNoteCore_SOURCES})
add_library(QwertycoinFramework::CryptoNoteCore ALIAS QwertycoinFramework_CryptoNoteCore)
target_include_directories(QwertycoinFramework_CryptoNoteCore PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_CryptoNoteCore PRIVATE ${QwertycoinFramework_CryptoNoteCore_LIBS})

# QwertycoinFramework::CryptoNoteProtocol

set(QwertycoinFramework_CryptoNoteProtocol_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolHandlerCommon.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/ICryptoNoteProtocolObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/ICryptoNoteProtocolQuery.h"
)

set(QwertycoinFramework_CryptoNoteProtocol_LIBS
    QwertycoinFramework::Global
)

add_library(QwertycoinFramework_CryptoNoteProtocol STATIC ${QwertycoinFramework_CryptoNoteProtocol_SOURCES})
add_library(QwertycoinFramework::CryptoNoteProtocol ALIAS QwertycoinFramework_CryptoNoteProtocol)
target_include_directories(QwertycoinFramework_CryptoNoteProtocol PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_CryptoNoteProtocol PRIVATE ${QwertycoinFramework_CryptoNoteProtocol_LIBS})

# QwertycoinFramework::Errors

set(QwertycoinFramework_Errors_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Errors/Errors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Errors/Errors.h"
    "${CMAKE_CURRENT_LIST_DIR}/Errors/ValidateParameters.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Errors/ValidateParameters.h"
)

set(QwertycoinFramework_Errors_LIBS
    stdc++fs
    QwertycoinFramework::Common
    QwertycoinFramework::Crypto
    QwertycoinFramework::Global
    QwertycoinFramework::SubWallets
    QwertycoinFramework::Utilities
)

add_library(QwertycoinFramework_Errors STATIC ${QwertycoinFramework_Errors_SOURCES})
add_library(QwertycoinFramework::Errors ALIAS QwertycoinFramework_Errors)
target_include_directories(QwertycoinFramework_Errors PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_CryptoNoteProtocol PRIVATE ${QwertycoinFramework_Errors_LIBS})

# QwertycoinFramework::Global

set(QwertycoinFramework_Global_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Global/Checkpoints.h"
    "${CMAKE_CURRENT_LIST_DIR}/Global/CliHeader.h"
    "${CMAKE_CURRENT_LIST_DIR}/Global/Config.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Global/Config.h"
    "${CMAKE_CURRENT_LIST_DIR}/Global/Constants.h"
    "${CMAKE_CURRENT_LIST_DIR}/Global/CryptoNoteConfig.h"
)

add_library(QwertycoinFramework_Global STATIC ${QwertycoinFramework_Global_SOURCES})
add_library(QwertycoinFramework::Global ALIAS QwertycoinFramework_Global)
target_include_directories(QwertycoinFramework_Global PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})

# QwertycoinFramework::Http

set(QwertycoinFramework_Http_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParser.h"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParserErrorCodes.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParserErrorCodes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpRequest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpRequest.h"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpResponse.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpResponse.h"
)

add_library(QwertycoinFramework_Http STATIC ${QwertycoinFramework_Http_SOURCES})
add_library(QwertycoinFramework::Http ALIAS QwertycoinFramework_Http)
target_include_directories(QwertycoinFramework_Http PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})

# QwertycoinFramework::JsonRpcServer

set(QwertycoinFramework_JsonRpcServer_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/JsonRpcServer/JsonRpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/JsonRpcServer/JsonRpcServer.h"
)

set(QwertycoinFramework_JsonRpcServer_LIBS
    QwertycoinFramework::CryptoNoteCore
)

add_library(QwertycoinFramework_JsonRpcServer STATIC ${QwertycoinFramework_JsonRpcServer_SOURCES})
add_library(QwertycoinFramework::JsonRpcServer ALIAS QwertycoinFramework_JsonRpcServer)
target_include_directories(QwertycoinFramework_JsonRpcServer PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_JsonRpcServer PRIVATE ${QwertycoinFramework_JsonRpcServer_LIBS})

# QwertycoinFramework::Logger

set(QwertycoinFramework_Logger_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Logger/Logger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logger/Logger.h"
)

add_library(QwertycoinFramework_Logger STATIC ${QwertycoinFramework_Logger_SOURCES})
add_library(QwertycoinFramework::Logger ALIAS QwertycoinFramework_Logger)

# QwertycoinFramework::Logging

set(QwertycoinFramework_Logging_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Logging/CommonLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/CommonLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ConsoleLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ConsoleLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/DummyLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/FileLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/FileLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ILogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ILogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerGroup.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerGroup.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerMessage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerMessage.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerRef.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerRef.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/StreamLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/StreamLogger.h"
)

set(QwertycoinFramework_Logging_LIBS
    QwertycoinFramework::Common
)

add_library(QwertycoinFramework_Logging STATIC ${QwertycoinFramework_Logging_SOURCES})
add_library(QwertycoinFramework::Logging ALIAS QwertycoinFramework_Logging)
target_include_directories(QwertycoinFramework_Logging PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Logging PRIVATE ${QwertycoinFramework_Logging_LIBS})

# QwertycoinFramework::Mnemonics

set(QwertycoinFramework_Mnemonics_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/CRC32.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/LanguageBase.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/Mnemonics.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/Mnemonics.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/WordList.h"
)

add_library(QwertycoinFramework_Mnemonics STATIC ${QwertycoinFramework_Mnemonics_SOURCES})
add_library(QwertycoinFramework::Mnemonics ALIAS QwertycoinFramework_Mnemonics)
target_include_directories(QwertycoinFramework_Mnemonics PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})

# QwertycoinFramework::Nigel

set(QwertycoinFramework_Nigel_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Nigel/Nigel.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Nigel/Nigel.h"
)

set(QwertycoinFramework_Nigel_LIBS
    QwertycoinFramework::Errors
)

add_library(QwertycoinFramework_Nigel STATIC ${QwertycoinFramework_Nigel_SOURCES})
add_library(QwertycoinFramework::Nigel ALIAS QwertycoinFramework_Nigel)
target_include_directories(QwertycoinFramework_Nigel PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Nigel PRIVATE ${QwertycoinFramework_Nigel_LIBS})

# QwertycoinFramework::NodeRpcProxy

set(QwertycoinFramework_NodeRpcProxy_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeRpcProxy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeRpcProxy.h"
)

set(QwertycoinFramework_NodeRpcProxy_LIBS
    QwertycoinFramework::Global
    QwertycoinFramework::Http
    QwertycoinFramework::Rpc
    QwertycoinFramework::System
)

add_library(QwertycoinFramework_NodeRpcProxy STATIC ${QwertycoinFramework_NodeRpcProxy_SOURCES})
add_library(QwertycoinFramework::NodeRpcProxy ALIAS QwertycoinFramework_NodeRpcProxy)
target_include_directories(QwertycoinFramework_NodeRpcProxy PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_NodeRpcProxy PRIVATE ${QwertycoinFramework_NodeRpcProxy_LIBS})

# QwertycoinFramework::P2p

set(QwertycoinFramework_P2p_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/P2p/ConnectionContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/IP2pNodeInternal.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/LevinProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/LevinProtocol.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNode.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNodeCommon.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNodeConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNodeConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/OnceInInterval.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pConnectionProxy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pConnectionProxy.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContext.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContextOwner.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContextOwner.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pInterfaces.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNetworks.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNode.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNodeConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNodeConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pProtocolDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pProtocolTypes.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/Peerlist.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/Peerlist.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/PeerListManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/PeerListManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/PendingLiteBlock.h"
)

set(QwertycoinFramework_P2p_LIBS
    MiniUPnP::miniupnpc
    QwertycoinFramework::Common
    QwertycoinFramework::CryptoNoteProtocol
    QwertycoinFramework::Global
)

if(WIN32 AND MSVC)
    list(APPEND QwertycoinFramework_P2p_LIBS Bcrypt)
endif()

add_library(QwertycoinFramework_P2p STATIC ${QwertycoinFramework_P2p_SOURCES})
add_library(QwertycoinFramework::P2p ALIAS QwertycoinFramework_P2p)
target_include_directories(QwertycoinFramework_P2p PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_P2p PRIVATE ${QwertycoinFramework_P2p_LIBS})

# QwertycoinFramework::Platform # TODO: Merge this target into QwertycoinFramework::System

# QwertycoinFramework::Rpc

set(QwertycoinFramework_Rpc_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/CoreRpcServerCommandsDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/CoreRpcServerErrorCodes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpClient.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpClient.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/JsonRpc.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/JsonRpc.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/RpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/RpcServer.h"
)

set(QwertycoinFramework_Rpc_LIBS
    QwertycoinFramework::BlockchainExplorer
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Global
    QwertycoinFramework::P2p
    QwertycoinFramework::Serialization
    QwertycoinFramework::System
    QwertycoinFramework::Http
)

add_library(QwertycoinFramework_Rpc STATIC ${QwertycoinFramework_Rpc_SOURCES})
add_library(QwertycoinFramework::Rpc ALIAS QwertycoinFramework_Rpc)
target_include_directories(QwertycoinFramework_Rpc PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Rpc PRIVATE ${QwertycoinFramework_Rpc_LIBS})

# QwertycoinFramework::Serialization

set(QwertycoinFramework_Serialization_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryInputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryInputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryOutputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryOutputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinarySerializationTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BlockchainExplorerDataSerialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BlockchainExplorerDataSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/CryptoNoteSerialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/CryptoNoteSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/ISerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/IStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputValueSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputValueSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonOutputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonOutputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryCommon.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryInputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryInputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryOutputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryOutputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/MemoryStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/SerializationOverloads.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/SerializationOverloads.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/SerializationTools.h"
)

set (QwertycoinFramework_Serialization_LIBS
    stdc++fs
    QwertycoinFramework::Common
)

add_library(QwertycoinFramework_Serialization STATIC ${QwertycoinFramework_Serialization_SOURCES})
add_library(QwertycoinFramework::Serialization ALIAS QwertycoinFramework_Serialization)
target_include_directories(QwertycoinFramework_Serialization PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Serialization PRIVATE ${QwertycoinFramework_Serialization_LIBS})

# QwertycoinFramework::SubWallets

set(QwertycoinFramework_SubWallets_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SubWallets/SubWallet.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SubWallets/SubWallet.h"
    "${CMAKE_CURRENT_LIST_DIR}/SubWallets/SubWallets.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SubWallets/SubWallets.h"
)

set(QwertycoinFramework_SubWallets_LIBS
    Boost::filesystem
    RapidJSON::rapidjson
    QwertycoinFramework::Common
    QwertycoinFramework::Crypto
    QwertycoinFramework::Errors
    QwertycoinFramework::Global
    QwertycoinFramework::Logger
    QwertycoinFramework::Utilities
    QwertycoinFramework::WalletBackend
)

add_library(QwertycoinFramework_SubWallets STATIC ${QwertycoinFramework_SubWallets_SOURCES})
add_library(QwertycoinFramework::SubWallets ALIAS QwertycoinFramework_SubWallets)
target_include_directories(QwertycoinFramework_SubWallets PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_SubWallets PRIVATE ${QwertycoinFramework_SubWallets_LIBS})

# QwertycoinFramework::System

set(QwertycoinFramework_System_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/System/Context.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroup.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroup.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroupTimeout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroupTimeout.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/Event.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/Event.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/EventLock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/EventLock.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/InterruptedException.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/InterruptedException.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/Ipv4Address.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/Ipv4Address.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/OperationTimeout.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/RemoteContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/RemoteEventLock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/RemoteEventLock.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/TcpStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/TcpStream.h"
)

if(MSVC)
    list(APPEND QwertycoinFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/MemoryMappedFile.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Timer.h"
    )
elseif(APPLE)
    list(APPEND QwertycoinFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/asm.s"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Context.c"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Context.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
elseif(ANDROID)
    list(APPEND QwertycoinFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Context.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
elseif(FREEBSD)
    list(APPEND QwertycoinFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/asm.s"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Context.c"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Context.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
else()
    list(APPEND QwertycoinFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
endif()

add_library(QwertycoinFramework_System STATIC ${QwertycoinFramework_System_SOURCES})
add_library(QwertycoinFramework::System ALIAS QwertycoinFramework_System)
target_include_directories(QwertycoinFramework_System PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})

if(WIN32)
    target_link_libraries(QwertycoinFramework_System PRIVATE ws2_32)
endif()

# QwertycoinFramework::Transfers

set(QwertycoinFramework_Transfers_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/BlockchainSynchronizer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/BlockchainSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/CommonTypes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/IBlockchainSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/IObservableImpl.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/SynchronizationState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/SynchronizationState.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersConsumer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersConsumer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersContainer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersContainer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSubscription.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSubscription.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSynchronizer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TypeHelpers.h"
)

set(QwertycoinFramework_Transfers_LIBS
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Global
)

add_library(QwertycoinFramework_Transfers STATIC ${QwertycoinFramework_Transfers_SOURCES})
add_library(QwertycoinFramework::Transfers ALIAS QwertycoinFramework_Transfers)
target_include_directories(QwertycoinFramework_Transfers PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Transfers PRIVATE ${QwertycoinFramework_Transfers_LIBS})

# QwertycoinFramework::Utilities

set(QwertycoinFramework_Utilities_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Addresses.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Addresses.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/ColouredMsg.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Container.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/FormatTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/FormatTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Input.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Input.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/LicenseCanary.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Mixins.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Mixins.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/ParseExtra.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/ParseExtra.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/String.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/String.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/ThreadSafeDeque.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/ThreadSafePriorityQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/ThreadSafeQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Utilities.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utilities/Utilities.h"
)

set(QwertycoinFramework_Utilities_LIBS
    QwertycoinFramework::Global
    QwertycoinFramework::Rpc
)

add_library(QwertycoinFramework_Utilities STATIC ${QwertycoinFramework_Utilities_SOURCES})
add_library(QwertycoinFramework::Utilities ALIAS QwertycoinFramework_Utilities)
target_include_directories(QwertycoinFramework_Utilities PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Utilities PRIVATE ${QwertycoinFramework_Utilities_LIBS})

# QwertycoinFramework::Wallet

set(QwertycoinFramework_Wallet_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/IFusionManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletAsyncContextCounter.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletAsyncContextCounter.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletGreen.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletGreen.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletIndices.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletSerializationV2.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletSerializationV2.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletUtils.h"
)

set(QwertycoinFramework_Wallet_LIBS
    Boost::filesystem
    QwertycoinFramework::Common
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Global
    QwertycoinFramework::NodeRpcProxy
    QwertycoinFramework::Transfers
    QwertycoinFramework::WalletBackend
)

add_library(QwertycoinFramework_Wallet STATIC ${QwertycoinFramework_Wallet_SOURCES})
add_library(QwertycoinFramework::Wallet ALIAS QwertycoinFramework_Wallet)
target_include_directories(QwertycoinFramework_Wallet PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_Wallet PRIVATE ${QwertycoinFramework_Wallet_LIBS})

# QwertycoinFramework::WalletBackend

set(QwertycoinFramework_WalletBackend_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/BlockDownloader.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/BlockDownloader.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/Constants.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/EventHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/JsonSerialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/JsonSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/SynchronizationStatus.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/SynchronizationStatus.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/Transfer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/Transfer.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/WalletBackend.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/WalletBackend.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/WalletSynchronizer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/WalletSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletBackend/WalletSynchronizerRAIIWrapper.h"
)

set(QwertycoinFramework_WalletBackend_LIBS
    Boost::filesystem
    cryptopp-static
    QwertycoinFramework::Common
    QwertycoinFramework::Global
    QwertycoinFramework::Logger
    QwertycoinFramework::Mnemonics
    QwertycoinFramework::Nigel
    QwertycoinFramework::Serialization
    QwertycoinFramework::SubWallets
    QwertycoinFramework::Wallet
)

add_library(QwertycoinFramework_WalletBackend STATIC ${QwertycoinFramework_WalletBackend_SOURCES})
add_library(QwertycoinFramework::WalletBackend ALIAS QwertycoinFramework_WalletBackend)
target_include_directories(QwertycoinFramework_WalletBackend PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_WalletBackend PRIVATE ${QwertycoinFramework_WalletBackend_LIBS})

# QwertycoinFramework::WalletService

set(QwertycoinFramework_WalletService_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/ConfigurationManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/ConfigurationManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/NodeFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/NodeFactory.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/PaymentGateService.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/PaymentGateService.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/PaymentServiceJsonRpcMessages.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/PaymentServiceJsonRpcMessages.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/PaymentServiceJsonRpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/PaymentServiceJsonRpcServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/WalletService.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/WalletService.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/WalletServiceConfiguration.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/WalletServiceConfiguration.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/WalletServiceErrorCategory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletService/WalletServiceErrorCategory.h"
)

set(QwertycoinFramework_WalletService_LIBS
    RapidJSON::rapidjson
    QwertycoinFramework::Common
    QwertycoinFramework::Crypto
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Global
    QwertycoinFramework::Logging
    QwertycoinFramework::Mnemonics
    QwertycoinFramework::Rpc
    QwertycoinFramework::Serialization
    QwertycoinFramework::Utilities
    QwertycoinFramework::Wallet
)

add_library(QwertycoinFramework_WalletService STATIC ${QwertycoinFramework_WalletService_SOURCES})
add_library(QwertycoinFramework::WalletService ALIAS QwertycoinFramework_WalletService)
target_include_directories(QwertycoinFramework_WalletService PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_WalletService PRIVATE ${QwertycoinFramework_WalletService_LIBS})

# QwertycoinFramework::ZedWallet

set(QwertycoinFramework_ZedWallet_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/AddressBook.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/AddressBook.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/CommandDispatcher.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/CommandDispatcher.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/CommandImplementations.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/CommandImplementations.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Commands.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Commands.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Fusion.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Fusion.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/GetInput.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/GetInput.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Menu.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Menu.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Open.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Open.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/ParseArguments.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/ParseArguments.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/PasswordContainer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/PasswordContainer.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Sync.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Sync.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Tools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Tools.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Transfer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Transfer.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/Types.h"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/ZedWallet.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ZedWallet/ZedWallet.h"
)

set(QwertycoinFramework_ZedWallet_LIBS

)

add_library(QwertycoinFramework_ZedWallet STATIC ${QwertycoinFramework_ZedWallet_SOURCES})
add_library(QwertycoinFramework::ZedWallet ALIAS QwertycoinFramework_ZedWallet)
target_include_directories(QwertycoinFramework_ZedWallet PRIVATE ${QwertycoinFramework_INCLUDE_DIRS})
target_link_libraries(QwertycoinFramework_ZedWallet PRIVATE ${QwertycoinFramework_ZedWallet_LIBS})

if(BUILD_WITH_COVERAGE AND (${CMAKE_BUILD_TYPE} MATCHES Debug))
    message(STATUS "Included code-coverage settings.")
    message(STATUS "NOTE: Code-coverage is enabled by default when CMAKE_BUILD_TYPE=Debug.")

    # Enable code-coverage settings for each library
    foreach(lib ${QwertycoinFramework_LIBS})
        target_link_libraries(${lib} PUBLIC codecov)
    endforeach()
endif()

install(TARGETS
            QwertycoinFramework_BlockchainExplorer
            QwertycoinFramework_Common
            QwertycoinFramework_Crypto
            QwertycoinFramework_CryptoNoteCore
            QwertycoinFramework_CryptoNoteProtocol
            QwertycoinFramework_Errors
            QwertycoinFramework_Global
            QwertycoinFramework_Http
            QwertycoinFramework_JsonRpcServer
            QwertycoinFramework_Logger
            QwertycoinFramework_Logging
            QwertycoinFramework_Mnemonics
            QwertycoinFramework_Nigel
            QwertycoinFramework_NodeRpcProxy
            QwertycoinFramework_P2p
            QwertycoinFramework_Rpc
            QwertycoinFramework_Serialization
            QwertycoinFramework_SubWallets
            QwertycoinFramework_System
            QwertycoinFramework_Transfers
            QwertycoinFramework_Wallet
            QwertycoinFramework_WalletBackend
            QwertycoinFramework_WalletService
            QwertycoinFramework_ZedWallet
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
