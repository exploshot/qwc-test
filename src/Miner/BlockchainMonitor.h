// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
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

#include <CryptoTypes.h>

#include "httplib.h"

#include <optional>

#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>

class BlockchainMonitor
{
public:
    BlockchainMonitor(
        System::Dispatcher &dispatcher,
        const size_t pollingInterval,
        const std::shared_ptr<httplib::Client> httpClient);

    void waitBlockchainUpdate();
    void stop();

private:
    System::Dispatcher &m_dispatcher;
    size_t m_pollingInterval;
    bool m_stopped;
    System::ContextGroup m_sleepingContext;

    std::optional<Crypto::Hash> requestLastBlockHash();

    std::shared_ptr<httplib::Client> m_httpClient = nullptr;
};
