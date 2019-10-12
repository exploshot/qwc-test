set(QwertycoinTools_INCLUDE_DIRS
    ${QwertycoinFramework_INCLUDE_DIRS}
    "${CMAKE_CURRENT_LIST_DIR}"
)

set(QwertycoinTools_INCLUDE_DIRS ${QwertycoinTools_INCLUDE_DIRS} PARENT_SCOPE)

add_custom_target(QwertycoinTools)
add_dependencies(QwertycoinTools
    QwertycoinTools::Daemon
#    QwertycoinTools::Miner
#    QwertycoinTools::ZedWallet
#    QwertycoinTools::ZedWallet++
)

# QwertycoinTools::BinaryInfo # NOTE: Ignore this. It's not a target.

# QwertycoinTools::Daemon

set(QwertycoinTools_Daemon_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/Daemon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonCommandsHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonCommandsHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonConfiguration.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonConfiguration.h"
)

if(WIN32)
    list(APPEND QwertycoinTools_Daemon_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/daemon.rc"
    )
endif()

set(QwertycoinTools_Daemon_LIBS
    lz4::lz4
    RocksDB::rocksdb
    sqlite3::sqlite3
    QwertycoinFramework::CryptoNoteCore
    QwertycoinFramework::Errors
    QwertycoinFramework::Global
)

add_executable(QwertycoinTools_Daemon ${QwertycoinTools_Daemon_SOURCES})
add_executable(QwertycoinTools::Daemon ALIAS QwertycoinTools_Daemon)
target_include_directories(QwertycoinTools_Daemon PRIVATE ${QwertycoinTools_INCLUDE_DIRS})
target_link_libraries(QwertycoinTools_Daemon PRIVATE ${QwertycoinTools_Daemon_LIBS})
set_target_properties(QwertycoinTools_Daemon PROPERTIES OUTPUT_NAME "qwertycoin")