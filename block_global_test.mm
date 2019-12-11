#import <Foundation/Foundation.h>
int glo_a = 1;
static int sglo_b =2;
int main(int argc, const char * argv[]) {
    void (^glo_blk1)(void) = ^{//没有使用任何外部变量
        NSLog(@"glo_blk1");
    };
    glo_blk1();
    NSLog(@"glo_blk1 : %@",[glo_blk1 class]);
    //glo_blk_1 : __NSGlobalBlock__
    
    static int c = 3;
    void(^glo_blk2)(void) = ^() {//只用到了静态变量、全局变量、静态全局变量
        NSLog(@"glo_a = %d,sglo_b = %d,c = %d",glo_a,sglo_b,c);
    };
    glo_blk2();
    NSLog(@"glo_blk2 : %@",[glo_blk2 class]);
    //glo_blk2 : __NSGlobalBlock__
}