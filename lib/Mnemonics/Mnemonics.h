

#include <tuple>
#include <vector>

#include <CryptoNote.h>

#include <Utilities/Errors.h>

namespace Mnemonics
{
    std::tuple<Error, Crypto::SecretKey> MnemonicToPrivateKey(const std::string words);

    std::tuple<Error, Crypto::SecretKey> MnemonicToPrivateKey(const std::vector<std::string> words);

    std::string PrivateKeyToMnemonic(const Crypto::SecretKey privateKey);

    bool HasValidChecksum(const std::vector<std::string> words);

    std::string GetChecksumWord(const std::vector<std::string> words);

    std::vector<int> GetWordIndexes(const std::vector<std::string> words);
} // namespace Mnemonics
