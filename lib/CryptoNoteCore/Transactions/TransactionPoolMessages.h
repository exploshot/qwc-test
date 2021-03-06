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

#include <CryptoNote.h>

#include <functional>

namespace CryptoNote {

    enum class TransactionMessageType
    {
        AddTransactionType,
        DeleteTransactionType
    };

    /*!
        immutable messages
    */
    struct AddTransaction
    {
        Crypto::Hash hash;
    };

    struct DeleteTransaction
    {
        Crypto::Hash hash;
    };

    class TransactionPoolMessage
    {
    public:
        TransactionPoolMessage(const AddTransaction &at);
        TransactionPoolMessage(const DeleteTransaction &at);

        /*!
            pattern matchin API
        */
        void match(std::function<void(const AddTransaction &)> &&, std::function<void(const DeleteTransaction &)> &&);

        /*!
            API with explicit type handling
        */
        TransactionMessageType getType() const;
        AddTransaction getAddTransaction() const;
        DeleteTransaction getDeleteTransaction() const;

    private:
        const TransactionMessageType type;
        union
        {
            const AddTransaction addTransaction;
            const DeleteTransaction deleteTransaction;
        };
    };
} // namespace CryptoNote
