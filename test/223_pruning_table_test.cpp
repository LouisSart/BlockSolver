#include "pruning_table.hpp"
#include "sys/types.h"
#include "sys/sysinfo.h"

using Table = BlockMoveTable<2, 5>;

int main(){

    // Table table; THIS LINE FAILS BECAUSE OF STACK OVERFLOW
    struct sysinfo memInfo;

    sysinfo (&memInfo);
    long long freeRAM = memInfo.freeram;
    //Add other values in next statement to avoid int overflow on right hand side...
    freeRAM *= memInfo.mem_unit;
    std::cout << freeRAM << std::endl;
    return 0.;
}