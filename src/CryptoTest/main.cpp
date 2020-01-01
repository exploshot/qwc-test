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

#include <iostream>
#include <chrono>
#include <assert.h>

#include <cxxopts.hpp>
#include <Global/CliHeader.h>

#include <CryptoNote.h>
#include <CryptoTypes.h>
#include <Common/StringTools.h>
#include <Crypto/Crypto.h>

#define PERFORMANCE_ITERATIONS  1000
#define PERFORMANCE_ITERATIONS_LONG_MULTIPLIER 10

using namespace Crypto;
using namespace CryptoNote;

const std::string INPUT_DATA =
    "0100fb8e8ac805899323371bb790db19218afd8db8e3755d8b90f39b3d5506a9abce4fa912244500000000ee8146d49fa93ee724deb57d12cbc6c6f3b924d946127c7a97418f9348828f0f02";

const std::string CN_SLOW_HASH_V0 = "1b606a3f4a07d6489a1bcd07697bd16696b61c8ae982f61a90160f4e52828a7f";

static inline bool CompareHashes(const Hash leftHash, const std::string right)
{
    Hash rightHash = Hash ();
    if (!Common::podFromHex (right, rightHash)) {
        return false;
    }

    return (leftHash == rightHash);
}

/* Hacky way to check if we're testing a v1 hash and thus should skip data
   < 43 bytes */
bool need43BytesOfData(std::string hashFunctionName)
{
    return hashFunctionName.find ("v1") != std::string::npos;
}

/* Bit of hackery so we can get the variable name of the passed in function.
   This way we can print the test we are currently performing. */
#define TEST_HASH_FUNCTION(hashFunction, expectedOutput) \
   testHashFunction(hashFunction, expectedOutput, #hashFunction, -1)

#define TEST_HASH_FUNCTION_WITH_HEIGHT(hashFunction, expectedOutput, height) \
    testHashFunction(hashFunction, expectedOutput, #hashFunction, height, height)

template<typename T, typename ...Args>
void testHashFunction(
    T hashFunction,
    std::string expectedOutput,
    std::string hashFunctionName,
    int64_t height,
    Args &&... args)
{
    const BinaryArray &rawData = Common::fromHex (INPUT_DATA);

    if (need43BytesOfData (hashFunctionName) && rawData.size () < 43) {
        return;
    }

    Hash hash = Hash ();

    /* Perform the hash, with a height if given */
    hashFunction (rawData.data (), rawData.size (), hash, std::forward<Args> (args)...);

    if (height == -1) {
        std::cout
            << hashFunctionName
            << ": "
            << hash
            << std::endl;
    } else {
        std::cout
            << hashFunctionName
            << " ("
            << height
            << "): "
            << hash
            << std::endl;
    }

    /* Verify the hash is as expected */
    if (!CompareHashes (hash, expectedOutput)) {
        std::cout
            << "Hashes are not equal!\n"
            << "Expected: "
            << expectedOutput
            << "\nActual: "
            << hash
            << "\nTerminating.";

        exit (1);
    }
}

/* Bit of hackery so we can get the variable name of the passed in function.
   This way we can print the test we are currently performing. */
#define BENCHMARK(hashFunction, iterations) \
   benchmark(hashFunction, #hashFunction, iterations)

template<typename T>
void benchmark(T hashFunction, std::string hashFunctionName, uint64_t iterations)
{
    const BinaryArray &rawData = Common::fromHex (INPUT_DATA);

    if (need43BytesOfData (hashFunctionName) && rawData.size () < 43) {
        return;
    }

    Hash hash = Hash ();

    auto startTimer = std::chrono::high_resolution_clock::now ();

    for (uint64_t i = 0; i < iterations; i++) {
        hashFunction (rawData.data (), rawData.size (), hash);
    }

    auto elapsedTime = std::chrono::high_resolution_clock::now () - startTimer;

    std::cout
        << hashFunctionName
        << ": "
        << (iterations / std::chrono::duration_cast<std::chrono::seconds> (elapsedTime).count ())
        << " H/s\n";
}

void benchmarkUnderivePublicKey()
{
    Crypto::KeyDerivation derivation;

    Crypto::PublicKey txPublicKey;
    Common::podFromHex ("f235acd76ee38ec4f7d95123436200f9ed74f9eb291b1454fbc30742481be1ab", txPublicKey);

    Crypto::SecretKey privateViewKey;
    Common::podFromHex ("89df8c4d34af41a51cfae0267e8254cadd2298f9256439fa1cfa7e25ee606606", privateViewKey);

    Crypto::generateKeyDerivation (txPublicKey, privateViewKey, derivation);

    const uint64_t loopIterations = 600000;

    auto startTimer = std::chrono::high_resolution_clock::now ();

    Crypto::PublicKey spendKey;

    Crypto::PublicKey outputKey;
    Common::podFromHex ("4a078e76cd41a3d3b534b83dc6f2ea2de500b653ca82273b7bfad8045d85a400", outputKey);

    for (uint64_t i = 0; i < loopIterations; i++) {
        /* Use i as output index to prevent optimization */
        Crypto::underivePublicKey (derivation, i, outputKey, spendKey);
    }

    auto elapsedTime = std::chrono::high_resolution_clock::now () - startTimer;

    /* Need to use microseconds here then divide by 1000 - otherwise we'll just get '0' */
    const auto
        timePerDerivation =
        std::chrono::duration_cast<std::chrono::microseconds> (elapsedTime).count () / loopIterations;

    std::cout
        << "Time to perform underivePublicKey: "
        << timePerDerivation / 1000.0
        << " ms"
        << std::endl;
}

void benchmarkGenerateKeyDerivation()
{
    Crypto::KeyDerivation derivation;

    Crypto::PublicKey txPublicKey;
    Common::podFromHex ("f235acd76ee38ec4f7d95123436200f9ed74f9eb291b1454fbc30742481be1ab", txPublicKey);

    Crypto::SecretKey privateViewKey;
    Common::podFromHex ("89df8c4d34af41a51cfae0267e8254cadd2298f9256439fa1cfa7e25ee606606", privateViewKey);

    const uint64_t loopIterations = 60000;

    auto startTimer = std::chrono::high_resolution_clock::now ();

    for (uint64_t i = 0; i < loopIterations; i++) {
        Crypto::generateKeyDerivation (txPublicKey, privateViewKey, derivation);
    }

    auto elapsedTime = std::chrono::high_resolution_clock::now () - startTimer;

    const auto
        timePerDerivation =
        std::chrono::duration_cast<std::chrono::microseconds> (elapsedTime).count () / loopIterations;

    std::cout
        << "Time to perform generateKeyDerivation: "
        << timePerDerivation / 1000.0
        << " ms"
        << std::endl;
}

int main(int argc, char **argv)
{
    bool o_help, o_version, o_benchmark;
    int o_iterations;

    cxxopts::Options options (argv[0], getProjectCLIHeader ());

    options.add_options ("Core")
               ("h,help", "Display this help message", cxxopts::value<bool> (o_help)->implicit_value ("true"))
               ("v,version",
                "Output software version information",
                cxxopts::value<bool> (o_version)->default_value ("false")->implicit_value ("true"));

    options.add_options ("Performance Testing")
               ("b,benchmark",
                "Run quick performance benchmark",
                cxxopts::value<bool> (o_benchmark)->default_value ("false")->implicit_value ("true"))
               ("i,iterations",
                "The number of iterations for the benchmark test. Minimum of 1,000 iterations required.",
                cxxopts::value<int> (o_iterations)->default_value (std::to_string (PERFORMANCE_ITERATIONS)),
                "#");

    try {
        auto result = options.parse (argc, argv);
    } catch (const cxxopts::OptionException &e) {
        std::cout
            << "Error: Unable to parse command line argument options: "
            << e.what ()
            << std::endl
            << std::endl;
        std::cout
            << options.help ({})
            << std::endl;
        exit (1);
    }

    if (o_help) // Do we want to display the help message?
    {
        std::cout
            << options.help ({})
            << std::endl;
        exit (0);
    } else if (o_version) // Do we want to display the software version?
    {
        std::cout
            << getProjectCLIHeader ()
            << std::endl;
        exit (0);
    }

    if (o_iterations < 1000 && o_benchmark) {
        std::cout
            << std::endl
            << "Error: The number of --iterations should be at least 1,000 for reasonable accuracy"
            << std::endl;
        exit (1);
    }

    int o_iterations_long = o_iterations * PERFORMANCE_ITERATIONS_LONG_MULTIPLIER;

    try {
        std::cout
            << getProjectCLIHeader ()
            << std::endl;

        std::cout
            << "Input: "
            << INPUT_DATA
            << std::endl
            << std::endl;

        TEST_HASH_FUNCTION(CnSlowHashV0, CN_SLOW_HASH_V0);

        std::cout
            << std::endl;

        if (o_benchmark) {
            std::cout
                << "\nPerformance Tests: Please wait, this may take a while depending on your system...\n\n";

            benchmarkUnderivePublicKey ();
            benchmarkGenerateKeyDerivation ();

            BENCHMARK(CnSlowHashV0, o_iterations);
        }
    } catch (std::exception &e) {
        std::cout
            << "Something went terribly wrong...\n"
            << e.what ()
            << "\n\n";
    }
}
