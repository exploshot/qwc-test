# RocksDB

hunter_add_package(rocksdb)
find_package(RocksDB CONFIG REQUIRED)
add_definitions(-DROCKSDB_USE_RTTI=1)