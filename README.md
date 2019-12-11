# Block implementation
## 1. Block review

**Block is anonymous functions together with automatic (local) variables.**

Block Literal Syntax
```
^ return type (argument list)  {expression}
```

Simple example
```
int (^blk)(int) = ^(int count) {return count+1;};
```

As arguments of function
```
void func (int (^blk)(int)) {}
```

As return value
```
return ^(int count){return count+1;};
```

Execute block
``` 
int result = blk(10)
```

Capturing automatic variables

* Automatic variables
* Function arguments
* Static variables
* Static global variables
* Global variables


## 2. Blocks
Compiler never generates human readable source code. But clang has a functionality to generate human-readable converted source code using

> clang -rewrite-objc file_name

Example:

**source code**
```
int main()
{
    void (^blk)(void) = ^{printf("Block\n");};
    blk();
    return 0;
}
```
**converted C code**
```
//block struct
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

//block struct
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};

//block implementation
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    printf("Block\n");
}

//block desc struct
static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

int main()
{
    //block defnition
    void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA));
    //struct __main_block_impl_0 tmp =
 __main_block_impl_0(__main_block_func_0, &__main_block_desc_0_DATA);
    //struct __main_block_impl_0 *blk = &tmp; 
    //call block
    ((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
    //(*blk->impl.FuncPtr)(blk); 
    return 0;
}
```

### _NSConcerateStackBlock and isa

**isa = &_NSConcerateStackBlock;**
```
typedef struct objc_object {
    Class isa;
} *id;

typedef struct objc_class *Class; 

strcut objc_class {
    Class isa;
};
```

```
@interface MyObject: NSObject
{
    int val0;
    int val1;
}
@end

Struct MyObject {
    Class isa;
    int val0;
    int val1;
};
```
```
struct class_t {
    struct class_t* isa;
    struct class_t* superclass;
    Cache cache;
    IMP* vtable;
    uintptr_t data_NEVER_USE;
};
```
![QDMGjK.png](https://s2.ax1x.com/2019/12/10/QDMGjK.png)

This __main_block_impl_0 struct is based on the objc_object struct, and is a struct for
Objective-C class objects. And, its member variable “isa” is initialized as follows.
isa = &_NSConcreteStackBlock;
This means that _NSConcreteStackBlock is the instance of the class_t struct. 
This means that _NSConcreteStackBlock is the instance of the class_t struct. When the Block is treated as an Objective-C object, _NSConcreteStackBlock has all the information of its class. Now we know that a Block is an Objective-C object.


# Capturing variables
## Automatic variables(read only)

```
int main() {
    int dmy = 256;
    int val = 10;
    const char* fmt = "val = %d\n";
    void (^blk)(void) = ^{printf(fmt, val);};
    return 0;
}
```

transfered code
```
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  //automatic variables are captured in structe as member variables
  const char *fmt;
  int val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, const char *_fmt, int _val, int flags=0) : fmt(_fmt), val(_val) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  const char *fmt = __cself->fmt; // bound by copy
  int val = __cself->val; // bound by copy
  printf(fmt, val);
}

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

int main() {
    int dmy = 256;
    int val = 10;
    const char *fmt = "val = %d\n";
    void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, fmt, val));
    return 0;
}
```

Only the values of the automatic variables used in the Block are captured. 

## Static and Global Variables(writable)
In C language, writable variables
* Static variables
* Static global variables
* Global variables

An anonymous function part in a Block literal is simply converted to a C function. In the converted function, static global variables and global variables can be accessed, work without any problem. But static variables are different. 

```
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
```

converted code
```
int global_val = 1;
static int static_global_val = 2;

struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  int *static_val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int *_static_val, int flags=0) : static_val(_static_val) {
    impl.isa = &_NSConcreteStackBlock;
    impl.Flags = flags;
    impl.FuncPtr = fp;
    Desc = desc;
  }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  int *static_val = __cself->static_val; // bound by copy

        global_val *= 1;
        static_global_val *= 2;
        (*static_val) *= 3;
    }

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};
int main()
{
    static int static_val = 3;
    void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, &static_val));
    return 0;
}
```

The static variable “static_val” is accessed via its pointer. A pointer to the variable is passed to the constructor of _main_block_impl_0 struct, and then the constructor assigns it. This is the easiest way to use a variable beyond the variable’s scope.
You might think that accessing automatic variables could be implemented in the same way as static variables. Why not? Because a Block must be able to exist even after the scope of a captured automatic variable is left. When the scope is left, the automatic variable is destroyed. Which means the Block can’t access the automatic variable anymore. So, automatic variables can’t be implemented the same as static variables.

## __block specifier
```
int crash = 10;
__block int val = 10;
void (^blk)(void) = ^{
    //crash = 1;//compiler error
    val = 1;
};
```

transfered code
```
struct __Block_byref_val_0 { 
    void *__isa;
    __Block_byref_val_0 *__forwarding; 
    int __flags;
    int __size;
    int val;
};
struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    __Block_byref_val_0 *val;
    __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc,
            __Block_byref_val_0 *_val, int flags=0) : val(_val->__forwarding) { 
        impl.isa = &_NSConcreteStackBlock;
        impl.Flags = flags;
        impl.FuncPtr = fp;
        Desc = desc;
    }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself)
{
    __Block_byref_val_0 *val = __cself->val;
    (val->__forwarding->val) = 1;
}
static void __main_block_copy_0( struct __main_block_impl_0*dst, struct __main_block_impl_0*src)
{
    _Block_object_assign(&dst->val, src->val, BLOCK_FIELD_IS_BYREF);
}
static void __main_block_dispose_0(struct __main_block_impl_0*src) {
    _Block_object_dispose(src->val, BLOCK_FIELD_IS_BYREF);
}
static struct __main_block_desc_0 {
    unsigned long reserved;
    unsigned long Block_size;
    void (*copy)(struct __main_block_impl_0*, struct __main_block_impl_0*); void (*dispose)(struct __main_block_impl_0*);
} __main_block_desc_0_DATA = { 0,
    sizeof(struct __main_block_impl_0),
    __main_block_copy_0, __main_block_dispose_0
};
int main()
{
    __Block_byref_val_0 val = {
        0,
        &val,
        0, 
        sizeof(__Block_byref_val_0), 
        10
    };
    blk = &__main_block_impl_0(__main_block_func_0, &__main_block_desc_0_DATA, &val, 0x22000000);
    return 0;
}
```

![QDJczd.png](https://s2.ax1x.com/2019/12/10/QDJczd.png)

```
__block int val = 10;
void (^blk0)(void) = ^{val = 0;};
void (^blk1)(void) = ^{val = 1;};
```
Convereted code
```
__Block_byref_val_0 val = {0, &val, 0, sizeof(__Block_byref_val_0), 10};
blk0 = &__main_block_impl_0(
 __main_block_func_0, &__main_block_desc_0_DATA, &val, 0x22000000);
Pointer ‘val’
Pointer to itself
the value for use
_isa
_forwarding
_flags
_size
val
blk1 = &__main_block_impl_1(
 __main_block_func_1, &__main_block_desc_1_DATA, &val, 0x22000000);
```

## Memory Segments for Blocks


Name | Under the Hood
---|---
Block | An instance of a struct of the Block. On stack
__block variable | An instance of a struct for __block variable. On stack

* _NSConcreteStackBlock
* _NSConcreteGlobalBlock
* _NSConcreteMallocBlock

![QDYckT.png](https://s2.ax1x.com/2019/12/10/QDYckT.png)

### NSConcreteGlobalBlock
```
void (^blk)(void) = ^{};

int main(){}
```
Converted code
```
impl.isa = &_NSConcreteGlobalBlock;
```
 Because automatic variables can’t
exist where the global variables are declared, capturing never happens. In other words,
the member variables of the instance for the Block don’t rely on the execution context. 

Summary:


- **NSConcreteGlobalBlock** are used when
1. when there are global variables
2. when a Block literal is
inside a function and doesn’t capture any automatic variables
- **NSConcreteStackBlock**
1. Any Block created by another kind of Block literal will be an object of
the _NSConcreteStackBlock class, and be stored on the stack
- **NSConcreteMallocBlock**
1. copy block


Meanwhile, a __block variable must be accessed properly no matter where it is on the stack or the heap. The member variable “__forwarding” in the struct for a __block variable is used for that. 

Even if a __block variable has been copied to the heap, the __block variable on the stack is still accessed in some cases. Because the member variable “__forwarding” of the instance on the stack points to the instance on the heap, regardless of where the __block variable is on the stack or heap, it is accessed properly. 

![Qrk3H1.png](https://s2.ax1x.com/2019/12/11/Qrk3H1.png)


### Copying Blocks Automatically
By the way, how do Blocks offer copy functionality? To tell the truth, when ARCenabled, in many cases the compiler automatically detects and copies the Block from the stack to the heap. Let’s see the next example, which calls a function returning a Block.
```
typedef int (^blk_t)(int);
blk_t func(int rate)
{
 return ^(int count){return rate * count;};
}
```
A function returns a Block, which is stored on the stack; that is, when the control flow returns to the caller, the variable scope is left and the Block on the stack is disposed of.
It looks problematic. Let’s check how it is converted when ARC is enabled.
```
blk_t func(int rate)
{
     blk_t tmp = &__func_block_impl_0(
    __func_block_func_0, &__func_block_desc_0_DATA, rate);
    tmp = objc_retainBlock(tmp);
    return objc_autoreleaseReturnValue(tmp);
}
```
Because ARC is enabled, "blk_t tmp" is same as "blk_t __strong tmp", which means the variable is qualified with __strong.
If you’d read the source code runtime/objc-arr.mm in the objc4 runtime library, you saw that objc_retainBlock function is equivalent to _Block_copy function. So, the above source code is equivalent to:
```
tmp = _Block_copy(tmp);
return objc_autoreleaseReturnValue(tmp);
```
Let’s see what happens with comments:
```
 /*
 * a Block is assigned from a Block literal to a variable 'tmp',
 * which means, the variable has the instance of struct for the Block on the stack.
 * Block_copy function copies the Block from the stack to the heap. 
 *  * After it is copied, its address on the heap is assigned to the variable 'tmp'.
 */
tmp = _Block_copy(tmp);
 /*
 * Then, the Block on the heap is registered to an autoreleasepool as an Objective-C
object.
 * After that, the Block is returned.
 */
return objc_autoreleaseReturnValue(tmp);
```
This means when the function returns the Block, the compiler automatically copies it to
the heap. 



 Block class | Copied from | How copy works
---|---|---
NSConcreteStackBlock | stack | from stack to heap
NSConcreteGlobalBlock | data section | do nothing
NSConcreteMallocBlock | Heap | add reference count

## __block variables

When a Block is copied from the stack to the heap and it uses __block variables and the __block variables aren’t used in the other Blocks, the __block variables must be on the stack. And, at the moment, all the __block variables are also copied from the stack to the heap and the Block has ownership of the __block variables.
When the Block on the heap is copied again, the __block variables will not be affected.
![QrAkxe.png](https://s2.ax1x.com/2019/12/11/QrAkxe.png)


What will happen if __block variables are used from more than one Block? At the
beginning, all the Blocks and the __block variables are on the stack. When one of the
Blocks is copied from the stack to the heap, the __block variables are copied from the
stack to the heap. And the Block has ownership of the variables. When another Block is
copied from the stack to the heap, the copied Block has ownership of the __block
variables (Figure 5–7). In other words, the reference count of the __block variables is +1.
![QrAmVI.png](https://s2.ax1x.com/2019/12/11/QrAmVI.png)

### __forwarding

A __block variable is copied from the stack to the heap when a Block is copied as well.
In this situation, both the __block variables on the stack and on the heap might be accessed at the same time. 
```
__block int val = 0;
 void (^blk)(void) = [^{++val;} copy];
++val;
blk();
NSLog(@"%d", val); 
```

```
++(val.__forwarding->val); 
```

In the function, which is converted from the Block literal, the variable “val” is an instance of the struct for __block variable on the heap. The other variable “val” outside the Block is on the stack. When the __block variable is copied from the stack to the heap, the member variable “__forwarding” of the instance on the stack is modified to the address
of the copied instance on the heap as shown
![QrAHeA.png](https://s2.ax1x.com/2019/12/11/QrAHeA.png)

When is the Block on the stack copied to the heap?
1. When the instance method “copy” is called on the Block
1. When the Block is returned from a function
1. When the Block is assigned to a member variable of id or the Block
1. type class, with __strong qualifier
1. When the Block is passed to a method, including “using


## Circular Reference with Blocks
```
typedef void (^blk_t)(void);
@interface MyObject : NSObject
{
 blk_t blk_;
}
@end

@implementation MyObject
- (id)init
{
 self = [super init];
 blk_ = ^{NSLog(@"self = %@", self);};
 return self;
}

- (void)dealloc
{
 NSLog(@"dealloc");
}
@end

int main()
{
 id o = [[MyObject alloc] init];
 NSLog(@"%@", o);
 return 0;
} 
```
![QrEPwn.png](https://s2.ax1x.com/2019/12/11/QrEPwn.png)
![QrEioq.png](https://s2.ax1x.com/2019/12/11/QrEioq.png)

```
@interface MyObject : NSObject
{
 blk_t blk_;
 id obj_;
}
@end
@implementation MyObject
- (id)init
{
 self = [super init];
 blk_ = ^{NSLog(@"obj_ = %@", obj_);};
 //blk_ = ^{NSLog(@"obj_ = %@", self->obj_);}; 
 return self;
} 
```


```
typedef void (^blk_t)(void);
@interface MyObject : NSObject
{
 blk_t blk_;
}
@end
@implementation MyObject
- (id)init
{
 self = [super init];
 __block id tmp = self;
 blk_ = ^{
 NSLog(@"self = %@", tmp);
 tmp = nil;
 };
 return self;
}
- (void)execBlock
{
 blk_();
}
- (void)dealloc
{
 NSLog(@"dealloc");
}
@end 
```
![QrEEWT.png](https://s2.ax1x.com/2019/12/11/QrEEWT.png)
If the instance method “execBlock” isn’t called, the circular reference causes a memory leak. By calling the instance method “execBlock”, the Block is executed and nil is assigned to the __block variable “tmp”. 