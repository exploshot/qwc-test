

#include <cctype>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include <Common/StringTools.h>

#include <CryptoNoteCore/Transactions/TransactionExtra.h>

#include <Global/Constants.h>

#include <SimpleWallet/PasswordContainer.h>
#include <SimpleWallet/Tools.h>

#include <Utilities/ColouredMsg.h>

using namespace CryptoNote;

void confirmPassword(std::string walletPass)
{
    /*!
     * Password container requires an rvalue, we don't want to wipe our current
     * pass so copy it into a tmp string and std::move that instead
     */
    std::string tmpString = walletPass;
    Tools::PasswordContainer pwdContainer (std::move (tmpString));

    while (!pwdContainer.readAndValidate ()) {
        std::cout
            << "Incorrect password! Try again."
            << std::endl;
    }
}

std::string formatAmount(uint64_t amount)
{
    uint64_t divider = pow(10, CryptoNote::parameters::CRYPTONOTE_DISPLAY_DECIMAL_POINT);

    uint64_t dollars = amount / divider;
    uint64_t cents = amount % divider;

    return formatDollars (dollars) + "." + formatCents (cents) + " " + WalletConfig::ticker;
}

std::string formatDollars(uint64_t amount)
{
    /*!
     * We want to format our number with comma separators so it's easier to
     * use. Now, we could use the nice print_money() function to do this.
     * However, whilst this initially looks pretty handy, if we have a locale
     * such as ja_JP.utf8, 1 QWC will actually be formatted as 100 QWC, which
     * is terrible, and could really screw over users.
     *
     * So, easy solution right? Just use en_US.utf8! Sure, it's not very
     * international, but it'll work! Unfortunately, no. The user has to have
     * the locale installed, and if they don't, we get a nasty error at
     * runtime.
     *
     * Annoyingly, there's no easy way to comma separate numbers outside of
     * using the locale method, without writing a pretty long boiler plate
     * function. So, instead, we define our own locale, which just returns
     * the values we want.
     *
     * It's less internationally friendly than we would potentially like
     * but that would require a ton of scrutinization which if not done could
     * land us with quite a few issues and rightfully angry users.
     * Furthermore, we'd still have to hack around cases like JP locale
     * formatting things incorrectly, and it makes reading in inputs harder
     * too.
     *
     * Thanks to https://stackoverflow.com/a/7277333/8737306 for this neat
     * workaround.
     */

    class CommaNumPunct: public std::numpunct<char>
    {
    protected:
        virtual char doThousandsSep() const
        {
            return ',';
        }

        virtual std::string doGrouping() const
        {
            return "\03";
        }
    };

    std::locale commaLocale (std::locale (), new CommaNumPunct ());
    std::stringstream stream;
    stream.imbue (commaLocale);
    stream
        << amount;
    return stream.str ();
}

/*!
 * Pad to two spaces, e.g. 5 becomes 05, 50 remains 50
 */
std::string formatCents(uint64_t amount)
{
    std::stringstream stream;
    stream
        << std::setfill ('0')
        << std::setw (2)
        << amount;

    return stream.str ();
}

bool confirm(std::string msg)
{
    return confirm (msg, true);
}

/*!
 * defaultReturn = what value we return on hitting enter, i.e. the "expected"
 * workflow
 */
bool confirm(std::string msg, bool defaultReturn)
{
    /*!
     * In unix programs, the upper case letter indicates the default, for
     * example when you hit enter
     */
    std::string prompt = " (Y/n): ";

    if (!defaultReturn) {
        prompt = " (y/N): ";
    }

    while (true) {
        std::cout
            << InformationMsg (msg + prompt);

        std::string answer;
        std::getline (std::cin, answer);

        char c = std::tolower (answer[0]);

        switch (std::tolower (answer[0])) {
            /*!
             * Lets people spam enter / choose default value
             */
            case '\0':
                return defaultReturn;
            case 'y':
                return true;
            case 'n':
                return false;
        }

        std::cout
            << WarningMsg ("Bad input: ")
            << InformationMsg (answer)
            << WarningMsg (" - please enter either Y or N.")
            << std::endl;
    }
}

std::string getPaymentID(std::string extra)
{
    std::string paymentID;

    if (extra.length () > 0) {
        BinaryArray vecExtra;

        for (auto it : extra) {
            vecExtra.push_back (static_cast<uint8_t>(it));
        }

        Crypto::Hash paymentIdHash;

        if (getPaymentIdFromTxExtra (vecExtra, paymentIdHash)) {
            return Common::podToHex (paymentIdHash);
        }
    }

    return paymentID;
}

uint64_t getScanHeight()
{
    while (true) {
        std::cout
            << "What height would you like to begin scanning "
            << "your wallet from?"
            << std::endl
            << "This can greatly speed up the initial wallet "
            << "scanning process."
            << std::endl
            << "If you do not know the exact height, "
            << std::endl
            << "err on the side of caution so transactions do not "
            << "get missed."
            << std::endl
            << "Hit enter for the default of zero: ";

        std::string stringHeight;

        std::getline (std::cin, stringHeight);

        /*!
         * Remove commas so user can enter height as e.g. 200,000
         */
        boost::erase_all (stringHeight, ",");

        if (stringHeight == "") {
            return 0;
        }

        try {
            return std::stoi (stringHeight);
        }
        catch (const std::invalid_argument &) {
            std::cout
                << WarningMsg ("Failed to parse height - input is not ")
                << WarningMsg ("a number!")
                << std::endl
                << std::endl;
        }
    }
}