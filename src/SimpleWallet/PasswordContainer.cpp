// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2019, The Qwertycoin developers
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
#include <memory.h>
#include <stdio.h>

#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <io.h>
    #include <windows.h>

#else
#include <termios.h>
    #include <unistd.h>
#endif

#include <SimpleWallet/PasswordContainer.h>

namespace Tools
{
    namespace
    {
        bool isCinTty();
    } // namespace

    PasswordContainer::PasswordContainer()
        : m_empty(true)
    {
    }

    PasswordContainer::PasswordContainer(std::string &&password)
        : m_empty(false),
          m_password(std::move(password))
    {
    }

    PasswordContainer::PasswordContainer(Tools::PasswordContainer &&rhs)
        : m_empty(std::move(rhs.m_empty)),
          m_password(std::move(rhs.m_password))
    {
    }

    PasswordContainer::~PasswordContainer()
    {
        clear();
    }

    void PasswordContainer::clear()
    {
        if (0 < m_password.capacity()) {
            m_password.replace(0, m_password.capacity(), m_password.capacity(), '\0');
            m_password.resize(0);
        }

        m_empty = true;
    }

    bool PasswordContainer::readAndValidate()
    {
        std::string tmpPassword = m_password;

        if (!readPassword()) {
            std::cout
                << "Failed to read password!"
                << std::endl;
            return false;
        }

        bool validPass = m_password == tmpPassword;

        m_password = tmpPassword;

        return validPass;
    }

    bool PasswordContainer::readPassword()
    {
        return readPassword(false);
    }

    bool PasswordContainer::readPassword(bool verify)
    {
        clear();

        bool r;
        if (isCinTty()) {
            if (verify) {
                std::cout
                    << "Give your new wallet a password: ";
            }
            else {
                std::cout
                    << "Enter password: ";
            }

            if (verify) {
                std::string password1;
                std::string password2;

                r = readFromTty(password1);
                if (r) {
                    std::cout
                        << "Confirm your new password: ";
                    r = readFromTty(password2);
                    if (r) {
                        if (password1 == password2) {
                            m_password = std::move(password2);
                            m_empty = false;
                            return true;
                        }
                        else {
                            std::cout
                                << "Passwords do not match, try again."
                                << std::endl;
                            clear();
                            return readPassword(true);
                        }
                    }
                }
            }
            else {
                r = readFromTty(m_password);
            }
        }
        else {
            r = readFromFile();
        }

        if (r) {
            m_empty = false;
        }
        else {
            clear();
        }

        return r;
    }

    bool PasswordContainer::readFromFile()
    {
        m_password.reserve(maxPasswordSize);
        for (size_t i = 0; i < maxPasswordSize; ++i) {
            char ch = static_cast<char>(std::cin.get());
            if (std::cin.eof() || ch == '\n' || ch == '\r') {
                break;
            }
            else if (std::cin.fail()) {
                return false;
            }
            else {
                m_password.push_back(ch);
            }
        }

        return true;
    }

#if defined(_WIN32)
    namespace
    {
        bool isCinTty()
        {
            return 0 != _isatty(_fileno(stdin));
        }
    } // namespace

    bool PasswordContainer::readFromTty(std::string &password)
    {
        const char BACKSPACE = 8;

        HANDLE hCin = ::GetStdHandle(STD_INPUT_HANDLE);
        DWORD modeOld;
        ::GetConsoleMode(hCin, &modeOld);
        DWORD modeNew = modeOld & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
        ::SetConsoleMode(hCin, modeNew);

        bool r = true;
        password.reserve(maxPasswordSize);

        while (password.size() < maxPasswordSize) {
            DWORD read;
            char ch;

            r = (TRUE == ::ReadConsole(hCin, &ch, 1, &read, NULL));
            r &= (1 == read);
            if (!r) {
                break;
            }
            else if (ch == '\n' || ch == '\r') {
                std::cout
                    << std::endl;
                break;
            }
            else if (ch == BACKSPACE) {
                if (!password.empty()) {
                    password.back() = '\0';
                    password.resize(password.size() - 1);
                    std::cout
                        << "\b \b";
                }
            }
            else {
                password.push_back(ch);
                std::cout
                    << '*';
            }
        }

        ::SetConsoleMode(hCin, modeOld);

        return r;
    }
#else
    namespace {
        bool isCinTty()
        {
            return 0 != isatty (fileno(stdin));
        }

        int getch()
        {
            struct termios ttyOld;
            tcgetattr (STDIN_FILENO, &ttyOld);

            struct termios ttyNew;
            ttyNew = ttyOld;
            ttyNew.c_cflag &= ~(ICANON | ECHO);
            tcsetattr (STDIN_FILENO, TCSANOW, &ttyNew);

            int ch = getchar ();

            tcsetattr (STDIN_FILENO, TCSANOW, &ttyOld);

            return ch;
        }
    } // namespace

    bool PasswordContainer::readFromTty(std::string &password)
    {
        const char BACKSPACE = 127;

        password.reserve (maxPasswordSize);

        while (password.size () < maxPasswordSize) {
            int ch = getch ();

            if (EOF == ch) {
                return false;
            } else if (ch == '\n' || ch == '\r') {
                std::cout
                    << std::endl;
                break;
            } else if (ch == BACKSPACE) {
                if (!password.empty ()) {
                    password.back() = '\0';
                    password.resize(password.size() - 1);
                    std::cout
                        << "\b \b";
                }
            } else {
                password.push_back(ch);
                std::cout
                    << "*";
            }
        }

        return true;
    }
#endif
} // namespace Tools
