// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018-2019, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Plenteum Developers
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

#include <Crypto/Keccak.h>

#include <CryptoNoteCore/Account.h>

#include <Serialization/CryptoNoteSerialization.h>

namespace CryptoNote {
    AccountBase::AccountBase()
    {
        setNull ();
    }

    void AccountBase::setNull()
    {
        m_keys = AccountKeys ();
    }

    void AccountBase::generate()
    {

        Crypto::generateKeys (m_keys.address.spendPublicKey, m_keys.spendSecretKey);

        /*!
            We derive the view secret key by taking our spend secret key, hashing
            with keccak-256, and then using this as the seed to generate a new set
            of keys - the public and private view keys. See generateDeterministicKeys
        */

        Crypto::CryptoOps::generateViewFromSpend (m_keys.spendSecretKey,
                                                  m_keys.viewSecretKey,
                                                  m_keys.address.viewPublicKey);
        m_creation_timestamp = time (NULL);

    }

    const AccountKeys &AccountBase::getAccountKeys() const
    {
        return m_keys;
    }

    void AccountBase::serialize(ISerializer &s)
    {
        s (m_keys, "m_keys");
        s (m_creation_timestamp, "m_creation_timestamp");
    }
} // namespace CryptoNote
