#import <stdio.h>
int main() {
__block int val = 10;
void (^blk)(void) = [^{val = 1;} copy];
void (^blk2)(void) = [blk copy];
}