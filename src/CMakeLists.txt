set(QwertycoinTools_INCLUDE_DIRS
    ${QwertycoinFramework_INCLUDE_DIRS}
    "${CMAKE_CURRENT_LIST_DIR}"
    )

set(QwertycoinTools_INCLUDE_DIRS ${QwertycoinTools_INCLUDE_DIRS} PARENT_SCOPE)

find_package(RapidJSON CONFIG REQUIRED)

# QwertycoinTools

add_custom_target(QwertycoinTools)
add_dependencies(QwertycoinTools
                 QwertycoinTools::CryptoTest
                 )

# QwertycoinTools::BinaryInfo # NOTE: Ignore this. It's not a target.


# QwertycoinTools::CryptoTest

set(QwertycoinTools_CryptoTest_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTest/main.cpp"
    )

if (WIN32)
    list(APPEND QwertycoinTools_CryptoTest_SOURCES
         "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/cryptotest.rc"
         )
endif ()

set(QwertycoinTools_CryptoTest_LIBS
    QwertycoinFramework::Common
    QwertycoinFramework::Crypto
    )

if (WIN32)
    list(APPEND QwertycoinTools_CryptoTest_LIBS ws2_32)
endif ()

add_executable(QwertycoinTools_CryptoTest ${QwertycoinTools_CryptoTest_SOURCES})
add_executable(QwertycoinTools::CryptoTest ALIAS QwertycoinTools_CryptoTest)
target_include_directories(QwertycoinTools_CryptoTest PRIVATE ${QwertycoinTools_INCLUDE_DIRS})
target_link_libraries(QwertycoinTools_CryptoTest ${QwertycoinTools_CryptoTest_LIBS})
set_target_properties(QwertycoinTools_CryptoTest PROPERTIES OUTPUT_NAME "CryptoTest")

# QwertycoinTools::Daemon

set(QwertycoinTools_Daemon_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/Daemon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonCommandsHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonCommandsHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonConfiguration.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonConfiguration.h"
    )

if (WIN32)
    list(APPEND QwertycoinTools_Daemon_SOURCES
         "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/daemon.rc"
         )
endif ()

set(QwertycoinTools_Daemon_LIBS
    Boost::filesystem
    RapidJSON::rapidjson
    sqlite3::sqlite3
    #    QwertycoinFramework::Breakpad
    QwertycoinFramework::Common
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Global
    QwertycoinFramework::Logging
    QwertycoinFramework::P2p
    QwertycoinFramework::Rpc
    QwertycoinFramework::Serialization
    QwertycoinFramework::System
    QwertycoinFramework::Utilities
    )

add_executable(QwertycoinTools_Daemon ${QwertycoinTools_Daemon_SOURCES})
add_executable(QwertycoinTools::Daemon ALIAS QwertycoinTools_Daemon)
target_include_directories(QwertycoinTools_Daemon PRIVATE ${QwertycoinTools_INCLUDE_DIRS})
target_link_libraries(QwertycoinTools_Daemon PRIVATE ${QwertycoinTools_Daemon_LIBS})
set_target_properties(QwertycoinTools_Daemon PROPERTIES OUTPUT_NAME "QDaemon")


# QwertycoinTools::Miner

# QwertycoinTools::SimpleWallet

set(QwertycoinTools_SimpleWallet_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Commands.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Commands.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Fusion.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Fusion.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Open.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Open.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/ParseArguments.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/ParseArguments.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/PasswordContainer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/PasswordContainer.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/SimpleWallet.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/SimpleWallet.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Sync.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Sync.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Tools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Tools.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Transfer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/Transfer.h"
    )

if (WIN32)
    list(APPEND QwertycoinTools_SimpleWallet_SOURCES
         "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/simplewallet.rc"
         )
endif ()

set(QwertycoinTools_SimpleWallet_LIBS
    Boost::program_options
    Boost::thread
    #    QwertycoinFramework::Breakpad
    QwertycoinFramework::Common
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Global
    QwertycoinFramework::Logging
    QwertycoinFramework::NodeRpcProxy
    QwertycoinFramework::Rpc
    QwertycoinFramework::Serialization
    QwertycoinFramework::Utilities
    QwertycoinFramework::Wallet
    )

if (WIN32)
    list(APPEND QwertycoinTools_SimpleWallet_LIBS ws2_32)
endif ()

add_executable(QwertycoinTools_SimpleWallet ${QwertycoinTools_SimpleWallet_SOURCES})
add_executable(QwertycoinTools::SimpleWallet ALIAS QwertycoinTools_SimpleWallet)
target_include_directories(QwertycoinTools_SimpleWallet PRIVATE ${QwertycoinTools_INCLUDE_DIRS})
target_link_libraries(QwertycoinTools_SimpleWallet ${QwertycoinTools_SimpleWallet_LIBS})
set_target_properties(QwertycoinTools_SimpleWallet PROPERTIES OUTPUT_NAME "SimpleWallet")

# QwertycoinTools::WalletApi


# QwertycoinTools::WalletService

# Installation

install(TARGETS
        QwertycoinTools_CryptoTest
        QwertycoinTools_Daemon
        QwertycoinTools_SimpleWallet
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )