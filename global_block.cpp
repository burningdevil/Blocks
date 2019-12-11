struct __blk_block_impl_0 {
  struct __block_impl impl;
  struct __blk_block_desc_0* Desc;
  __blk_block_impl_0(void *fp, struct __blk_block_desc_0 *desc, int flags=0) {
    impl.isa = &_NSConcreteGlobalBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static void __blk_block_func_0(struct __blk_block_impl_0 *__cself) {
printf("Block\n");}

static struct __blk_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __blk_block_desc_0_DATA = { 0, sizeof(struct __blk_block_impl_0)};
static __blk_block_impl_0 __global_blk_block_impl_0((void *)__blk_block_func_0, &__blk_block_desc_0_DATA);
void (*blk)(void) = ((void (*)())&__global_blk_block_impl_0);
typedef int (*blk_t)(int);


struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  int rate;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int _rate, int flags=0) : rate(_rate) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static int __main_block_func_0(struct __main_block_impl_0 *__cself, int count) {
  int rate = __cself->rate; // bound by copy
return rate*count;}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

struct __main_block_impl_1 {
  struct __block_impl impl;
  struct __main_block_desc_1* Desc;
  __main_block_impl_1(void *fp, struct __main_block_desc_1 *desc, int flags=0) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static int __main_block_func_1(struct __main_block_impl_1 *__cself, int count) {
return count;}

static struct __main_block_desc_1 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_1_DATA = { 0, sizeof(struct __main_block_impl_1)};
int main()
{
    ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    for (int rate = 0; rate < 3; ++rate) {
        blk_t blk2 = ((int (*)(int))&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, rate));
        blk_t blk3 = ((int (*)(int))&__main_block_impl_1((void *)__main_block_func_1, &__main_block_desc_1_DATA));
    }
    return 0;
}
