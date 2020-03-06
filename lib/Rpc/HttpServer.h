// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include <unordered_set>

#include <Http/HttpRequest.h>
#include <Http/HttpResponse.h>

#include <Logging/LoggerRef.h>

#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/TcpListener.h>
#include <System/TcpConnection.h>
#include <System/Event.h>

namespace CryptoNote {

    class HttpServer
    {

    public:

        HttpServer(System::Dispatcher &dispatcher, std::shared_ptr<Logging::ILogger> log);

        void start(const std::string &address, uint16_t port);
        void stop();

        virtual void processRequest(const HttpRequest &request, HttpResponse &response) = 0;

    protected:

        System::Dispatcher &m_dispatcher;

    private:

        void acceptLoop();
        void connectionHandler(System::TcpConnection &&conn);

        System::ContextGroup workingContextGroup;
        Logging::LoggerRef logger;
        System::TcpListener m_listener;
        std::unordered_set<System::TcpConnection *> m_connections;
    };

} // namespace CryptoNote
