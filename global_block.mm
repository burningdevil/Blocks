#include <stdio.h>

void (^blk)(void) = ^{printf("Block\n");};
typedef int (^blk_t)(int);

int main()
{
    blk();
    for (int rate = 0; rate < 3; ++rate) {
        blk_t blk2 = ^(int count){return rate*count;};
        blk_t blk3 = ^(int count){return count;};
    }
    return 0;
}
