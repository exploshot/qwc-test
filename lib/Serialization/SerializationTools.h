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

#include <list>
#include <vector>

#include <Common/MemoryInputStream.h>
#include <Common/StringOutputStream.h>
#include <Common/VectorOutputStream.h>

#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/CryptoNoteSerialization.h>
#include <Serialization/JsonInputStreamSerializer.h>
#include <Serialization/JsonOutputStreamSerializer.h>
#include <Serialization/KVBinaryInputStreamSerializer.h>
#include <Serialization/KVBinaryOutputStreamSerializer.h>

#include <WalletInfo.h>

namespace Common {

    template<typename T>
    T getValueAs(const JsonValue &js)
    {
        return js;
        //cdstatic_assert(false, "undefined conversion");
    }

    template<>
    inline std::string getValueAs<std::string>(const JsonValue &js)
    {
        return js.getString ();
    }

    template<>
    inline uint64_t getValueAs<uint64_t>(const JsonValue &js)
    {
        return static_cast<uint64_t>(js.getInteger ());
    }

} // namespace Common

namespace CryptoNote {

    template<typename T>
    Common::JsonValue storeToJsonValue(const T &v)
    {
        JsonOutputStreamSerializer s;
        serialize (const_cast<T &>(v), s);
        return s.getValue ();
    }

    template<typename T>
    Common::JsonValue storeContainerToJsonValue(const T &cont)
    {
        Common::JsonValue js (Common::JsonValue::ARRAY);
        for (const auto &item : cont) {
            js.pushBack (item);
        }
        return js;
    }

    template<>
    inline Common::JsonValue storeContainerToJsonValue(const std::vector<AddressBookEntry> &cont)
    {
        Common::JsonValue js (Common::JsonValue::ARRAY);
        for (const auto &item : cont) {
            js.pushBack (storeToJsonValue (item));
        }
        return js;
    }

    template<typename T>
    Common::JsonValue storeToJsonValue(const std::vector<T> &v)
    {
        return storeContainerToJsonValue (v);
    }

    template<typename T>
    Common::JsonValue storeToJsonValue(const std::list<T> &v)
    {
        return storeContainerToJsonValue (v);
    }

    template<>
    inline Common::JsonValue storeToJsonValue(const std::string &v)
    {
        return Common::JsonValue (v);
    }

    template<typename T>
    void loadFromJsonValue(T &v, const Common::JsonValue &js)
    {
        JsonInputValueSerializer s (js);
        serialize (v, s);
    }

    template<typename T>
    void loadFromJsonValue(std::vector<T> &v, const Common::JsonValue &js)
    {
        for (uint64_t i = 0; i < js.size (); ++i) {
            v.push_back (Common::getValueAs<T> (js[i]));
        }
    }

    template<>
    inline void loadFromJsonValue(AddressBook &v, const Common::JsonValue &js)
    {
        for (uint64_t i = 0; i < js.size (); ++i) {
            AddressBookEntry type;
            loadFromJsonValue (type, js[i]);
            v.push_back (type);
        }
    }

    template<typename T>
    void loadFromJsonValue(std::list<T> &v, const Common::JsonValue &js)
    {
        for (uint64_t i = 0; i < js.size (); ++i) {
            v.push_back (Common::getValueAs<T> (js[i]));
        }
    }

    template<typename T>
    std::string storeToJson(const T &v)
    {
        return storeToJsonValue (v).toString ();
    }

    template<typename T>
    bool loadFromJson(T &v, const std::string &buf)
    {
        try {
            if (buf.empty ()) {
                return true;
            }
            auto js = Common::JsonValue::fromString (buf);
            loadFromJsonValue (v, js);
        } catch (std::exception &) {
            return false;
        }
        return true;
    }

    template<typename T>
    std::string storeToBinaryKeyValue(const T &v)
    {
        KVBinaryOutputStreamSerializer s;
        serialize (const_cast<T &>(v), s);

        std::string result;
        Common::StringOutputStream stream (result);
        s.dump (stream);
        return result;
    }

    template<typename T>
    bool loadFromBinaryKeyValue(T &v, const std::string &buf)
    {
        try {
            Common::MemoryInputStream stream (buf.data (), buf.size ());
            KVBinaryInputStreamSerializer s (stream);
            serialize (v, s);
            return true;
        } catch (std::exception &) {
            return false;
        }
    }

    /*!
     * noexcept
     */
    template<class T>
    bool toBinaryArray(const T &object, BinaryArray &binaryArray)
    {
        // std::cout << "toBinaryArray L162" << std::endl;
        // std::cout << "toBinaryArray L162 => Classname: " << typeid(T).name() << std::endl;
        try {
            // std::cout << "toBinaryArray L162 => Try" << std::endl;
            ::Common::VectorOutputStream stream (binaryArray);
            BinaryOutputStreamSerializer serializer (stream);
            serialize (const_cast<T &>(object), serializer);
        } catch (std::exception &e) {
            std::cout
                << "toBinaryArray L162 => Exception: "
                << e.what ()
                << std::endl;
            return false;
        }

        return true;
    }

    template<>
    inline bool toBinaryArray(const BinaryArray &object, BinaryArray &binaryArray)
    {
        try {
            ::Common::VectorOutputStream stream (binaryArray);
            BinaryOutputStreamSerializer serializer (stream);
            std::string oldBlob = Common::asString (object);
            serializer (oldBlob, "");
        } catch (std::exception &e) {
            /*!
             * TODO: add logger
             */
            return false;
        }

        return true;
    }

    /*!
     * throws exception if serialization failed
     */
    template<class T>
    BinaryArray toBinaryArray(const T &object)
    {
        BinaryArray ba;
        toBinaryArray (object, ba);

        return ba;
    }

    template<class T>
    T fromBinaryArray(const std::vector<uint8_t> &binaryArray)
    {
        T object;
        Common::MemoryInputStream stream (binaryArray.data (), binaryArray.size ());
        BinaryInputStreamSerializer serializer (stream);
        serialize (object, serializer);
        /*!
         * check that all data was consumed
         */
        if (!stream.endOfStream ()) {
            throw std::runtime_error ("failed to unpack type");
        }

        return object;
    }

    template<class T>
    bool fromBinaryArray(T &object, const std::vector<uint8_t> &binaryArray)
    {
        try {
            object = fromBinaryArray<T> (binaryArray);
        } catch (std::exception &) {
            return false;
        }

        return true;
    }

} // namespace CryptoNote
