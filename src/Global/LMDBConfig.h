#include <Global/Constants.h>

namespace LMDB
{
    const std::string DB_NAME = "DB";
    const std::string TESTNET_DB_NAME = "testnet_DB";
    const size_t MAX_DIRTY = 1000;
    /* min. available/empty room in the db */
    const size_t MAPSIZE_MIN_AVAIL = 64 * Constants::MEGABYTE;
    /*  Shift << n / MEGABYTE = 
        Shift 18446744071562067968   : 17592186042368 MiB.   31
        Shift 1073741824             : 1024 MiB.             30
        Shift 536870912              : 512 MiB.              29
        Shift 268435456              : 256 MiB.              28
        Shift 134217728              : 128 MiB.              27
        Shift 67108864               : 64 MiB.               26
        Shift 33554432               : 32 MiB.               25
        Shift 16777216               : 16 MiB.               24
        Shift 8388608                : 8 MiB.                23
        Shift 4194304                : 4 MiB.                22
        Shift 2097152                : 2 MiB.                21
        Shift 1048576                : 1 MiB.                20
        Shift 524288                 : 0.5 MiB.              19
        Shift 262144                 : 0.25 MiB.             18
        Shift 131072                 : 0.125 MiB.            17
        Shift 65536                  : 0.0625 MiB.           16
        Shift 32768                  : 0.03125 MiB.          15
    */
    const int SHIFTING_VAL = 25;
    
}