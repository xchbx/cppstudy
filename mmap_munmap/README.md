# mmap_munmap
mmap()与munmap()的小例子
mmap() 和 munmap() - Unix的系统函数
```cpp
//-----------------------------------------------------------------------------------------------------
void* mmap(void* addr,size_t size,int prot,int flags,int fd, off_t offset)
//-----------------------------------------------------------------------------------------------------
```
```C++
/*<
参数addr可以指定映射的首地址，一般为0 交给内核指定。
size 就是分配内存的大小，映射时以 页 为单位。
prot是分配内存的权限，一般用PROT_READ|PROT_WRITE   
  flags是标识，通常包括以下三个：
         MAP_SHARED   MAP_PRIVATE ： 二选一 ，指明映射的内存是否共享，MAP_SHARED只对 映射文件有效。
         MAP_ANONYMOUS ： 映射物理内存，默认映射文件。
fd是文件描述符，在映射文件时有用。
offset是文件的偏移量，指定 映射文件时从哪里开始。
映射物理内存时，fd和offset 给 0 即可。
返回  成功返回首地址，失败返回 MAP_FAILED==(void*)-1
>*/
```
-----------------------------------------------------------------------------------------------------

```c++
//mmap.c
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

int main()
{
   /*映射一片内存*/
   void* p = mmap(0,//让系统指定映射内存的首地址(虚拟地址)
        4,//映射4bytes的空间，其实会映射一页内存
        PROT_READ|PROT_WRITE,//权限，有读写权限
        MAP_PRIVATE|MAP_ANONYMOUS,/*MAP_ANONYMOUS:匿名映射，当映射到内存时，只能使用匿名映射。MAP_SHARED和MAP_PRIVATE必选其一。*/ 
        0,//文件描述符，用于映射到文件，内存映射中会忽略
        0);//文件偏移量，用于文件，内存映射也会忽略
   if(p==MAP_FAILED){
      perror("映射失败");
      exit(-1);
   }
   int* pi = p+500;
   int i;
   for(i=10; i<60; i++){
      pi[i] = i;  
   }
   strcpy(p+750, "ABCDEFGHIJKLMN");
   char* pc = p;
   for(i=0; i<800; i++){
      if(i%10==0)printf("\n");
      printf("%x ", pc[i]);
   }
   printf("p=%p\n", p);
   sleep(20);
   munmap(p, 4);
  // while(1);
}
```
