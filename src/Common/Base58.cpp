// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include <assert.h>
#include <string>
#include <vector>

#include <Common/Base58.h>
#include <Common/IIntUtil.h>
#include <Common/Varint.h>
#include <Crypto/Hash.h>

namespace Tools
{
    namespace Base58
    {
      namespace
      {
          const char     alphabet[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
          const uint64_t alphabetSize = sizeof(alphabet) - 1;
          const uint64_t encodedBlockSizes[] = {0, 2, 3, 5, 6, 7, 9, 10, 11};
          const uint64_t fullBlockSize = sizeof(encodedBlockSizes) / sizeof(encodedBlockSizes[0]) - 1;
          const uint64_t fullEncodedBlockSize = encodedBlockSizes[fullBlockSize];
          const uint64_t addressChecksumSize = 4;

          struct ReverseAlphabet
          {
              ReverseAlphabet()
              {
                  m_data.resize(alphabet[alphabetSize - 1] - alphabet[0] + 1, -1);

                  for (uint64_t i = 0; i < alphabetSize; ++i) {
                      uint64_t idx = static_cast<uint64_t>(alphabet[i] - alphabet[0]);
                      m_data[idx] = static_cast<int8_t>(i);
                  }
              }

              int operator()(char letter) const
              {
                  uint64_t idx = static_cast<uint64_t>(letter - alphabet[0]);
                  return idx < m_data.size() ? m_data[idx] : -1;
              }

              static ReverseAlphabet instance;

          private:
              std::vector<int8_t> m_data;
          };

          ReverseAlphabet ReverseAlphabet::instance;

          struct DecodedBlockSizes
          {
              DecodedBlockSizes()
              {
                  m_data.resize(encodedBlockSizes[fullBlockSize] + 1, -1);

                  for (uint64_t i = 0; i <= fullBlockSize; ++i) {
                      m_data[encodedBlockSizes[i]] = static_cast<int>(i);
                  }
              }

              int operator()(uint64_t encodedBlockSize) const
              {
                  assert(encodedBlockSize <= fullEncodedBlockSize);
                  
                  return m_data[encodedBlockSize];
              }

              static DecodedBlockSizes instance;

          private:
              std::vector<int> m_data;
          };

          DecodedBlockSizes DecodedBlockSizes::instance;

          uint64_t uint8beTo64(const uint8_t *data, uint64_t size)
          {
              assert(1 <= size && size <= sizeof(uint64_t));

              uint64_t res = 0;
              switch (9 - size) {
              case 1:            
                  res |= *data++; 
                  /* fallthrough */
              case 2: 
                  res <<= 8; 
                  res |= *data++; 
                  /* fallthrough */
              case 3: 
                  res <<= 8; 
                  res |= *data++; 
                  /* fallthrough */
              case 4: 
                  res <<= 8; 
                  res |= *data++; 
                  /* fallthrough */
              case 5: 
                  res <<= 8; 
                  res |= *data++; 
                  /* fallthrough */
              case 6: 
                  res <<= 8; 
                  res |= *data++; 
                  /* fallthrough */
              case 7: 
                  res <<= 8; 
                  res |= *data++; 
                  /* fallthrough */
              case 8: 
                  res <<= 8; 
                  res |= *data; 
                  break;
              default: 
                  assert(false);
              }

              return res;
          }

          void uint64To8be(uint64_t num, uint64_t size, uint8_t *data)
          {
              assert(1 <= size && size <= sizeof(uint64_t));

              uint64_t num_be = SWAP64BE(num);
              memcpy(data, reinterpret_cast<uint8_t *>(&num_be) + sizeof(uint64_t) - size, size);
          }

          void encodeBlock(const char *block, uint64_t size, char *res)
          {
              assert(1 <= size && size <= fullBlockSize);

              uint64_t num = uint8beTo64(reinterpret_cast<const uint8_t *>(block), size);
              int i = static_cast<int>(encodedBlockSizes[size]) - 1;
              while (0 < num) {
                  uint64_t remainder = num % alphabetSize;
                  num /= alphabetSize;
                  res[i] = alphabet[remainder];
                  --i;
              }
          }

          bool decodeBlock(const char *block, uint64_t size, char *res)
          {
              assert(1 <= size && size <= fullEncodedBlockSize);

              int resSize = DecodedBlockSizes::instance(size);
              if (resSize <= 0)
                  return false; // Invalid block size

              uint64_t resNum = 0;
              uint64_t order = 1;
              for (uint64_t i = size - 1; i < size; --i) {
                  int digit = ReverseAlphabet::instance(block[i]);
                  if (digit < 0) {
                      return false; // Invalid symbol
                  }                      

                  uint64_t productHi;
                  uint64_t tmp = resNum + mul128(order, digit, &productHi);
                  if (tmp < resNum || 0 != productHi) {
                      return false; // Overflow
                  }                      

                  resNum = tmp;
                  order *= alphabetSize; // Never overflows, 58^10 < 2^64
              }

              if (static_cast<uint64_t>(resSize) < fullBlockSize 
                  && (UINT64_C(1) << (8 * resSize)) <= resNum) {
                  return false; // Overflow
              }
                

              uint64To8be(resNum, resSize, reinterpret_cast<uint8_t*>(res));

              return true;
          }
      } // namespace

      std::string encode(const std::string &data)
      {
          if (data.empty()) {
              return std::string();
          }
            

          uint64_t fullBlockCount = data.size() / fullBlockSize;
          uint64_t lastBlockSize = data.size() % fullBlockSize;
          uint64_t resSize = fullBlockCount * fullEncodedBlockSize 
                             + encodedBlockSizes[lastBlockSize];

          std::string res(resSize, alphabet[0]);
          for (uint64_t i = 0; i < fullBlockCount; ++i) {
              encodeBlock(data.data() + i * fullBlockSize, 
                          fullBlockSize, 
                          &res[i * fullEncodedBlockSize]);
          }

          if (0 < lastBlockSize) {
              encodeBlock(data.data() + fullBlockCount * fullBlockSize, 
                          lastBlockSize, 
                          &res[fullBlockCount * fullEncodedBlockSize]);
          }

          return res;
      }

      bool decode(const std::string &enc, std::string &data)
      {
          if (enc.empty()) {
              data.clear();
              return true;
          }

          uint64_t fullBlockCount = enc.size() / fullEncodedBlockSize;
          uint64_t lastBlockSize = enc.size() % fullEncodedBlockSize;
          int lastBlockDecodedSize = DecodedBlockSizes::instance(lastBlockSize);

          if (lastBlockDecodedSize < 0) {
              return false; // Invalid enc length
          }
            
          uint64_t dataSize = fullBlockCount * fullBlockSize + lastBlockDecodedSize;

          data.resize(dataSize, 0);

          for (uint64_t i = 0; i < fullBlockCount; ++i) {
              if (!decodeBlock(enc.data() + i * fullEncodedBlockSize, 
                              fullEncodedBlockSize, 
                              &data[i * fullBlockSize])) {
                  return false;
              }
          }

          if (0 < lastBlockSize) {
              if (!decodeBlock(enc.data() + fullBlockCount * fullEncodedBlockSize, 
                              lastBlockSize,
                                &data[fullBlockCount * fullBlockSize])) {
                  return false;                    
              }
          }

          return true;
      }

      std::string encodeAddress(uint64_t tag, const std::string &data)
      {
          std::string buf = getVarintData(tag);
          buf += data;
          Crypto::Hash hash = Crypto::CnFastHash(buf.data(), buf.size());
          const char *hashData = reinterpret_cast<const char *>(&hash);
          buf.append(hashData, addressChecksumSize);

          return encode(buf);
      }

      bool decodeAddress(std::string addr, uint64_t &tag, std::string &data)
      {
          std::string addrData;
          bool r = decode(addr, addrData);
          if (!r) {
              return false;
          }
          if (addrData.size() <= addressChecksumSize) {
              return false;
          }

          std::string checksum(addressChecksumSize, '\0');
          checksum = addrData.substr(addrData.size() - addressChecksumSize);

          addrData.resize(addrData.size() - addressChecksumSize);
          Crypto::Hash hash = Crypto::CnFastHash(addrData.data(), addrData.size());
          std::string expectedChecksum(reinterpret_cast<const char *>(&hash), addressChecksumSize);

          if (expectedChecksum != checksum) {
              return false;
          }

          int read = Tools::readVarint(addrData.begin(), addrData.end(), tag);
          if (read <= 0) {
              return false;
          }

          data = addrData.substr(read);

          return true;
      }
    } // namespace Base58
} // namespace Tools
