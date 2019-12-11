
int global_val = 1;
static int static_global_val = 2;
int main()
{
    static int static_val = 3; 
    void (^blk)(void) = ^{
        global_val *= 1; 
        static_global_val *= 2; 
        static_val *= 3;
    };
    return 0; 
}