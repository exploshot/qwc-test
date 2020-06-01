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

#include <algorithm>
#include <cstdint>
#include <ctime>

#include <Common/StringTools.h>
#include <Common/Util.h>

#include <CryptoNoteCore/Blockchain/LMDB/BlockchainDB.h>
#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>
#include <CryptoNoteCore/Currency.h>

#include <Global/CryptoNoteConfig.h>

#include <Logging/LoggerRef.h>

namespace CryptoNote {
    class UpgradeDetectorBase
    {
    public:
        enum: uint32_t
        {
            UNDEF_HEIGHT = static_cast<uint32_t>(-1),
        };
    };

    static_assert (CryptoNote::UpgradeDetectorBase::UNDEF_HEIGHT == UINT32_C(0xFFFFFFFF),
                   "UpgradeDetectorBase::UNDEF_HEIGHT has invalid value");

    template<typename BC>
    class BasicUpgradeDetector: public UpgradeDetectorBase
    {
    public:
        BasicUpgradeDetector(const Currency &currency,
                             BC &blockchain,
                             BlockchainDB &db,
                             uint8_t targetVersion,
                             std::shared_ptr<Logging::ILogger> log)
            : db(db),
              mCurrency(currency),
              mBlockchain(blockchain),
              mTargetVersion(targetVersion),
              mVotingCompleteHeight(UNDEF_HEIGHT),
              logger (log, "upgrade")
        {
        }

        bool init()
        {
            bool r = !Tools::isLmdb();

            #define HEIGHT (r ? (mBlockchain.size() + 1) : db.height())

            uint32_t upgradeHeight = mCurrency.upgradeHeight (mTargetVersion);

            if (upgradeHeight == UNDEF_HEIGHT) {
                if (mBlockchain.empty ()) {
                    mVotingCompleteHeight = UNDEF_HEIGHT;

                } else if (mTargetVersion - 1 == mBlockchain.back ().bl.majorVersion) {
                    mVotingCompleteHeight = findVotingCompleteHeight (static_cast<uint32_t>(mBlockchain.size () - 1));

                } else if (mTargetVersion <= mBlockchain.back ().bl.majorVersion) {
                    auto it = std::lower_bound (mBlockchain.begin (),
                                                mBlockchain.end (),
                                                mTargetVersion,
                                                [](const typename BC::value_type &b,
                                                   uint8_t v)
                                                {
                                                    return b.bl.majorVersion < v;
                                                });
                    if (it == mBlockchain.end () || it->bl.majorVersion != mTargetVersion) {
                        logger (Logging::ERROR, Logging::BRIGHT_RED)
                            << "Internal error: upgrade height isn't found";

                        return false;
                    }

                    upgradeHeight = static_cast<uint32_t>(it - mBlockchain.begin ());
                    mVotingCompleteHeight = findVotingCompleteHeight (upgradeHeight);

                    if (mVotingCompleteHeight == UNDEF_HEIGHT) {
                        logger (Logging::ERROR, Logging::BRIGHT_RED)
                            << "Internal error: voting complete height isn't found, upgrade height = "
                            << upgradeHeight;

                        return false;
                    }
                } else {
                    mVotingCompleteHeight = UNDEF_HEIGHT;
                }
            } else if (!mBlockchain.empty ()) {
                if (mBlockchain.size () <= upgradeHeight + 1) {
                    if (mBlockchain.back ().bl.majorVersion >= mTargetVersion) {
                        logger (Logging::ERROR, Logging::BRIGHT_RED)
                            << "Internal error: block at height "
                            << (mBlockchain.size () - 1)
                            << " has invalid version "
                            << static_cast<int>(mBlockchain.back ().bl.majorVersion)
                            << ", expected "
                            << static_cast<int>(mTargetVersion - 1)
                            << " or less";

                        return false;
                    }
                } else {
                    int blockVersionAtUpgradeHeight = mBlockchain[upgradeHeight].bl.majorVersion;

                    if (blockVersionAtUpgradeHeight != mTargetVersion - 1) {
                        logger (Logging::ERROR, Logging::BRIGHT_RED)
                            << "Internal error: block at height "
                            << upgradeHeight
                            << " has invalid version "
                            << blockVersionAtUpgradeHeight
                            << ", expected "
                            << static_cast<int>(mTargetVersion - 1);

                        return false;
                    }

                    int blockVersionAfterUpgradeHeight =
                            mBlockchain[upgradeHeight + 1].bl.majorVersion;

                    if (blockVersionAfterUpgradeHeight != mTargetVersion) {
                        logger (Logging::ERROR, Logging::BRIGHT_RED)
                            << "Internal error: block at height "
                            << (upgradeHeight + 1)
                            << " has invalid version "
                            << blockVersionAfterUpgradeHeight
                            << ", expected "
                            << static_cast<int>(mTargetVersion);

                        return false;
                    }
                }
            }

            return true;
        }

        uint8_t targetVersion() const
        {
            return mTargetVersion;
        }

        uint32_t votingCompleteHeight() const
        {
            return mVotingCompleteHeight;
        }

        uint32_t upgradeHeight() const
        {
            if (mCurrency.upgradeHeight (mTargetVersion) == UNDEF_HEIGHT) {
                return mVotingCompleteHeight == UNDEF_HEIGHT ? UNDEF_HEIGHT
                                                              : mCurrency.calculateUpgradeHeight (mVotingCompleteHeight);
            } else {
                return mCurrency.upgradeHeight (mTargetVersion);
            }
        }

        void blockPushed()
        {
            assert(!mBlockchain.empty ());

            bool r = !Tools::isLmdb();

            if (mCurrency.upgradeHeight (mTargetVersion) != UNDEF_HEIGHT) {
                if (HEIGHT <= mCurrency.upgradeHeight (mTargetVersion) + 1) {
                    assert((r ? mBlockchain.back().bl.majorVersion :
                            db.getTopBlock().majorVersion) <= mTargetVersion - 1);
                } else {
                    assert((r ? mBlockchain.back().bl.majorVersion :
                            db.getTopBlock().majorVersion) >= mTargetVersion);
                }
            } else if (mVotingCompleteHeight != UNDEF_HEIGHT) {
                assert(HEIGHT > mVotingCompleteHeight);

                if (HEIGHT <= upgradeHeight ()) {
                    assert(mBlockchain.back ().bl.majorVersion == mTargetVersion - 1);

                    if (HEIGHT % (60 * 60 / mCurrency.difficultyTarget ()) == 0) {
                        auto interval = mCurrency.difficultyTarget () *
                                        (upgradeHeight () - HEIGHT + 2);
                        time_t upgradeTimestamp = time (nullptr) + static_cast<time_t>(interval);
                        struct tm *upgradeTime = localtime (&upgradeTimestamp);;
                        char upgradeTimeStr[40];
                        strftime (upgradeTimeStr, 40, "%H:%M:%S %Y.%m.%d", upgradeTime);
                        CryptoNote::CachedBlock cachedBlock (r ? mBlockchain.back ().bl : db.getTopBlockHash());

                        logger (Logging::TRACE, Logging::BRIGHT_GREEN)
                            << "###### UPGRADE is going to happen after block index "
                            << upgradeHeight ()
                            << " at about "
                            << upgradeTimeStr
                            << " (in "
                            << Common::timeIntervalToString (interval)
                            << ")! Current last block index "
                            << (mBlockchain.size () - 1)
                            << ", hash "
                            << cachedBlock.getBlockHash ();
                    }
                } else if (HEIGHT == upgradeHeight() + 1) {
                    assert((r ? mBlockchain.back().bl.majorVersion :
                                db.getTopBlock().majorVersion) == mTargetVersion - 1);

                    logger (Logging::TRACE, Logging::BRIGHT_GREEN)
                        << "###### UPGRADE has happened! Starting from block index "
                        << (upgradeHeight () + 1)
                        << " blocks with major version below "
                        << static_cast<int>(mTargetVersion)
                        << " will be rejected!";
                } else {
                    assert(mBlockchain.back ().bl.majorVersion == mTargetVersion);
                }
            } else {
                uint32_t lastBlockHeight = HEIGHT - 1;
                if (isVotingComplete (lastBlockHeight)) {
                    mVotingCompleteHeight = lastBlockHeight;
                    logger (Logging::TRACE, Logging::BRIGHT_GREEN)
                        << "###### UPGRADE voting complete at block index "
                        << mVotingCompleteHeight
                        << "! UPGRADE is going to happen after block index "
                        << upgradeHeight ()
                        << "!";
                }
            }
        }

        void blockPopped()
        {
            bool r = !Tools::isLmdb();

            if (mVotingCompleteHeight != UNDEF_HEIGHT) {
                assert(mCurrency.upgradeHeight (mTargetVersion) == UNDEF_HEIGHT);

                if (mBlockchain.size () == mVotingCompleteHeight) {
                    logger (Logging::TRACE, Logging::BRIGHT_YELLOW)
                        << "###### UPGRADE after block index "
                        << upgradeHeight ()
                        << " has been canceled!";

                    mVotingCompleteHeight = UNDEF_HEIGHT;
                } else {
                    assert(mBlockchain.size () > mVotingCompleteHeight);
                }
            }
        }

        size_t getNumberOfVotes(uint32_t height)
        {
            bool r = !Tools::isLmdb();

            if (height < mCurrency.upgradeVotingWindow () - 1) {
                return 0;
            }

            size_t voteCounter = 0;
            for (size_t i = height + 1 - mCurrency.upgradeVotingWindow (); i <= height; ++i) {
                const auto &b = (r ? mBlockchain[i].bl : db.getBlockFromHeight(i));
                voteCounter += (b.majorVersion == mTargetVersion - 1) &&
                               (b.minorVersion == BLOCK_MINOR_VERSION_1) ? 1 : 0;
            }

            return voteCounter;
        }

    private:
        uint32_t findVotingCompleteHeight(uint32_t probableUpgradeHeight)
        {
            assert(mCurrency.upgradeHeight (mTargetVersion) == UNDEF_HEIGHT);

            uint32_t probableVotingCompleteHeight = probableUpgradeHeight > mCurrency.maxUpgradeDistance () ?
                                                    probableUpgradeHeight - mCurrency.maxUpgradeDistance () : 0;
            for (uint32_t i = probableVotingCompleteHeight; i <= probableUpgradeHeight; ++i) {
                if (isVotingComplete (i)) {
                    return i;
                }
            }

            return UNDEF_HEIGHT;
        }

        bool isVotingComplete(uint32_t height)
        {
            assert(mCurrency.upgradeHeight (mTargetVersion) == UNDEF_HEIGHT);
            assert(mCurrency.upgradeVotingWindow () > 1);
            assert(mCurrency.upgradeVotingThreshold () > 0 &&
                   mCurrency.upgradeVotingThreshold () <= 100);

            size_t voteCounter = getNumberOfVotes (height);
            return mCurrency.upgradeVotingThreshold () *
                   mCurrency.upgradeVotingWindow () <= 100 * voteCounter;
        }

    private:
        Logging::LoggerRef logger;
        const Currency &mCurrency;
        BlockchainDB &db;
        BC &mBlockchain;
        uint8_t mTargetVersion;
        uint32_t mVotingCompleteHeight;
    };
} // namespace CryptoNote
