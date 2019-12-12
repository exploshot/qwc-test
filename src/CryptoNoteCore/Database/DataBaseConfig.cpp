// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
//
// Please see the included LICENSE file for more information.


#include <Common/Util.h>
#include <Common/StringTools.h>

#include <Crypto/Crypto.h>

#include <CryptoNoteCore/Database/DataBaseConfig.h>

#include <Global/CryptoNoteConfig.h>
#include <Global/Constants.h>

using namespace CryptoNote;
using namespace Constants;

DataBaseConfig::DataBaseConfig() 
    : dataDir(Tools::getDefaultDataDirectory()),
      backgroundThreadsCount(DATABASE_DEFAULT_BACKGROUND_THREADS_COUNT),
      maxOpenFiles(DATABASE_DEFAULT_MAX_OPEN_FILES),
      writeBufferSize(DATABASE_WRITE_BUFFER_MB_DEFAULT_SIZE * MEGABYTE),
      readCacheSize(DATABASE_READ_BUFFER_MB_DEFAULT_SIZE * MEGABYTE),
      testnet(false),
      configFolderDefaulted(false) 
{
}

bool DataBaseConfig::init(const std::string dataDirectory, 
                          const int backgroundThreads, 
                          const int openFiles, 
                          const int writeBufferMB, 
                          const int readCacheMB)
{
    dataDir = dataDirectory;
    backgroundThreadsCount = backgroundThreads;
    maxOpenFiles = openFiles;
    writeBufferSize = writeBufferMB * MEGABYTE;
    readCacheSize = readCacheMB * MEGABYTE;

    if (dataDir == Tools::getDefaultDataDirectory()) {
        configFolderDefaulted = true;
    }

    return true;
}

bool DataBaseConfig::isConfigFolderDefaulted() const 
{
    return configFolderDefaulted;
}

std::string DataBaseConfig::getDataDir() const 
{
    return dataDir;
}

uint16_t DataBaseConfig::getBackgroundThreadsCount() const 
{
    return backgroundThreadsCount;
}

uint32_t DataBaseConfig::getMaxOpenFiles() const 
{
    return maxOpenFiles;
}

uint64_t DataBaseConfig::getWriteBufferSize() const 
{
    return writeBufferSize;
}

uint64_t DataBaseConfig::getReadCacheSize() const 
{
    return readCacheSize;
}

bool DataBaseConfig::getTestnet() const 
{
    return testnet;
}
