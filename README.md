# Block
## 1. Block review

**Block is anonymous functions together with automatic variables.**

Block Literal Syntax
```
^ return type (argument list)  {expression}
```

Example
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

* Automatic variables(local variables)
* Function arguments
* Static variables
* Static global variables
* Global variables


## 2. Block as objc object
Compiler never generates human readable source code. But clang has a functionality to generate human-readable converted source code using

> clang -rewrite-objc file_name

Example:

[source code](block.mm)

[converted C code](block.cpp)

**implementation of block**
```
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
  printf("Block\n");
}
```
__cself is equal to this in C++ and self in OC.

**block structure**
```
//block impl structure
struct __block_impl {
  void *isa;
  int Flags;
  int Reserved;
  void *FuncPtr;
};

//block structure
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

static struct __main_block_desc_0 {
  size_t reserved;
  size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};
```

**init a block**
```
void (*blk)(void) = ((void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA));
    //struct __main_block_impl_0 tmp = __main_block_impl_0(__main_block_func_0, &__main_block_desc_0_DATA);
    //struct __main_block_impl_0 *blk = &tmp;
```

**call a block**
```
((void (*)(__block_impl *))((__block_impl *)blk)->FuncPtr)((__block_impl *)blk);
//(*blk->impl.FuncPtr)(blk); 
```

### Block Data Structure
```
struct Block_descriptor_1 {
    uintptr_t reserved;
    uintptr_t size;
};
 
struct Block_layout {
    void *isa;
    volatile int32_t flags; // contains ref count
    int32_t reserved; 
    void (*invoke)(void *, ...);
    struct Block_descriptor_1 *descriptor;
    // imported variables
};
```
![image](https://upload-images.jianshu.io/upload_images/1727123-b2963eb80edb4d78.png?imageMogr2/auto-orient/strip|imageView2/2/w/510)

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

 This __main_block_impl_0 struct is based on the objc_object struct, and is a struct for Objective-C class objects. And, its member variable “isa” is initialized as follows.

>isa = &_NSConcreteStackBlock;

This means that _NSConcreteStackBlock is the instance of the class_t struct. When the Block is treated as an Objective-C object, _NSConcreteStackBlock has all the information of its class. Now we know that a Block is an Objective-C object.

# Capturing variables

External variables:
* Automatic variables(local variables)
* Static variables
* Static global variables
* Global variables

## Automatic variables(read only)

[source code](block_automatic_variables.mm)

[transfered C code](block_automatic_variables.cpp)

difference compared to previous code piece:
```
struct __main_block_impl_0 {
  struct __block_impl impl;
  struct __main_block_desc_0* Desc;
  const char *fmt;
  int val;
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, const char *_fmt, int _val, int flags=0) : fmt(_fmt), val(_val) {
  }
```
> Automatic variables is captured in block struct as member variables.

> Only the values of the automatic variables used in the Block are captured.

## Static and Global Variables(writable)
In C language, writable variables
* static variables
* static global variables
* global variables
[source code](Block_Static_Global.mm)

[transfered C code](Block_Static_Global.cpp)

****
```
static void __main_block_func_0(struct __main_block_impl_0 *__cself) { 
    int *static_val = __cself->static_val;
    global_val *= 1; 
    static_global_val *= 2; 
    (*static_val) *= 3;
}
```

In the converted function, static global variables and global variables can be accessed, work without any problem.  

But static variables are different. The static variable “static_val” is accessed via its pointer. A pointer to the variable is passed to the constructor of _main_block_impl_0 struct, and then the constructor assigns it. This is the easiest way to use a variable beyond the variable’s scope.
A Block must be able to exist even after the scope of a captured automatic variable is left. When the scope is left, the automatic variable is destroyed. Which means the Block can’t access the automatic variable anymore. So, automatic variables can’t be implemented the same as static variables.

## __block variable(writable)

[source code](Block_Block.mm)

[transfered C code](Block_Block.cpp)

**__block structure**
```
struct __Block_byref_val_0 { void *__isa;
    __Block_byref_val_0 *__forwarding; 
    int __flags;
    int __size;
    int val;
};

__Block_byref_val_0 val = { 
    0,
    &val, 
    0,
    sizeof(__Block_byref_val_0),
    10 
};
```
**assign to __block variable**
```
static void __main_block_func_0(struct __main_block_impl_0 *__cself)
{
    __Block_byref_val_0 *val = __cself->val;
    (val->__forwarding->val) = 1;
}
```

![QDJczd.png](https://s2.ax1x.com/2019/12/10/QDJczd.png)

The __block variables is an automatic variable of __Block_byref_val_0 struct. 
__forwarding points to itself in this case.

### multiple blocks using same __block

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
blk1 = &__main_block_impl_1(
 __main_block_func_1, &__main_block_desc_1_DATA, &val, 0x22000000);
```

Name | Under the Hood
---|---
Block | An instance of a struct of the Block. On stack
__block variable | An instance of a struct for __block variable. On stack

# Memory Segments for Blocks

More of NSConcrete*Blocks:
* _NSConcreteStackBlock
* _NSConcreteGlobalBlock
* _NSConcreteMallocBlock

![QDYckT.png](https://s2.ax1x.com/2019/12/10/QDYckT.png)

### NSConcreteGlobalBlock

[source file](global_block.mm)

[convereted C code](global_block.cpp)
```
impl.isa = &_NSConcreteGlobalBlock;
```
Because automatic variables can’t exist where the global variables are declared, capturing never happens. In other words, the member variables of the instance for the Block don’t rely on the execution context. 

**two cases**
```
typedef int (^blk_t)(int);
for (int rate = 0; rate < 10; ++rate) {
    blk_t blk = ^(int count){return rate * count;}; 
}
```
```
typedef int (^blk_t)(int);
for (int rate = 0; rate < 10; ++rate) {
    blk_t blk = ^(int count){return count;};
}
```
- When a Block literal is written where there are global variables
- When the syntax in a Block literal doesn’t use any automatic variables to be captured

In these cases, the Block will be a _NSConcreteGlobalBlock class object and is stored in the data section. Any Block created by another kind of Block literal will be an object of the _NSConcreteStackBlock class, and be stored on the stack.

[test](block_global_test.mm)

### NSConcreteMallocBlock

By copying a stack block, the copied block on the heap can exists after the scope is left.

Meanwhile, a __block variable must be accessed properly no matter where it is on the stack or the heap. The member variable “__forwarding” in the struct for a __block variable is assigned to the one in heap. 

Even if a __block variable has been copied to the heap, the __block variable on the stack is still accessed in some cases. Because the member variable “__forwarding” of the instance on the stack points to the instance on the heap, regardless of where the __block variable is on the stack or heap, it is accessed properly. 

![Qrk3H1.png](https://s2.ax1x.com/2019/12/11/Qrk3H1.png)

### Summary
- **NSConcreteGlobalBlock** are used when
1. when there are global variables
2. when a Block literal is inside a function and doesn’t capture any automatic variables
- **NSConcreteStackBlock**
1. Any Block created by another kind of Block literal will be an object of
the _NSConcreteStackBlock class, and be stored on the stack
- **NSConcreteMallocBlock**
1. copy block from stack

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
```
 /*
 * a Block is assigned from a Block literal to a variable 'tmp',
 * which means, the variable has the instance of struct for the Block on the stack.
 * Block_copy function copies the Block from the stack to the heap. 
 *  * After it is copied, its address on the heap is assigned to the variable 'tmp'.
 */
tmp = _Block_copy(tmp);
 /*
 * Then, the Block on the heap is registered to an autoreleasepool as an Objective-C object.
 * After that, the Block is returned.
 */
return objc_autoreleaseReturnValue(tmp);
```
This means when the function returns the Block, the compiler automatically copies it to the heap. 

### Copying Blocks Manually

** Using copy() function.
When do ARC can not detect it?
- When a Block is passed as an argument for methods or functions.
    -  Except: Cocoa Framework methods, the name of which includes "usingBlock" or GCD API

 Block class | Copied from | How copy works
---|---|---
NSConcreteStackBlock | stack | from stack to heap
NSConcreteGlobalBlock | data section | do nothing
NSConcreteMallocBlock | Heap | add reference count



## Memeory Segments for __block variables

When a Block is copied from the stack to the heap and it uses __block variables and the __block variables aren’t used in the other Blocks, the __block variables must be on the stack. And, at the moment, all the __block variables are also copied from the stack to the heap and the Block has ownership of the __block variables.
When the Block on the heap is copied again, the __block variables will not be affected.
![QrAkxe.png](https://s2.ax1x.com/2019/12/11/QrAkxe.png)


What will happen if __block variables are used from more than one Block? At the
beginning, all the Blocks and the __block variables are on the stack. When one of the Blocks is copied from the stack to the heap, the __block variables are copied from the stack to the heap. And the Block has ownership of the variables. When another Block is copied from the stack to the heap, the copied Block has ownership of the __block. In other words, the reference count of the __block variables is +1.
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
both __block variables access is using
```
++(val.__forwarding->val); 
```

In the function, which is converted from the Block literal, the variable “val” is an instance of the struct for __block variable on the heap. The other variable “val” outside the Block is on the stack. When the __block variable is copied from the stack to the heap, the member variable “__forwarding” of the instance on the stack is modified to the address of the copied instance on the heap as shown
![QrAHeA.png](https://s2.ax1x.com/2019/12/11/QrAHeA.png)

## Circular Reference with Blocks

### Example of momory leak
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

###  How to fix
![QrEioq.png](https://s2.ax1x.com/2019/12/11/QrEioq.png)

### Another example
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
### Another way to fix
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

**What's wrong with this soultion**

If the instance method “execBlock” isn’t called, the circular reference causes a memory leak. By calling the instance method “execBlock”, the Block is executed and nil is assigned to the __block variable “tmp”. 

# Summary
- How automatic variables are captured
- How __block variables are achieved
- How a circular reference problem occurs due to captured objects and how to solve it