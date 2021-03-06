About
-----

This is a module for GP2X 2.4 based Linux kernel, created for developers to use in their
programs.

Normally the upper 32MB is uncached. This means that reads/writes on the memory
are always done via the physical memory modules rather than the much faster
memory built into the processor (called 'cache'). Access to the upper 32MB can
be sped up by Squidge's MMU hack. The easiest way to use the MMU hack is to add
and load the MMU hack kernel module into your program.

Note: Building this module requries a GP2X 'kernel' toolchain (i.e. GCC 2.95.*
for the GP2X stock, 3.* for Open2X).

You can't build this module with the GCC 4 based application toolchains.

Operation
---------

When loaded into kernel, this module creates /dev/mmuhack device. Whenever
a program opens that device using open() call, the module traverses all
memory, which was allocated in 0x02000000-0x03ffffff range by the program via
using mmap() system call. While doing that, it marks all encountered memory
as bufferable and cacheable.

The most common use of this is to remove the framebuffer access bottleneck.
Note that, however, by making the framebuffer cacheable you can cause display
artifacts. This can happen because parts of your framebuffer may stay in CPU
cache and not to be written back to the physical memory. The display
controller only fetches data from the physical memory, so you get incomplete
image (the memory will most likely contain data from previous frame, so these
artifacts are better visible during fade effects). The easy way to fix this
is by using a special ARM Linux system call, which flushes the cache (forces
the CPU to write data in cache to the physical memory (see section "Flushing
the cache")).

Using this module affects the whole upper memory area. But in some situations
this may be not desirable, for example when using ARM940 core in your program
(ether using 940 libraries like ogg940 and gpu940, or using your custom code,
which needs uncacheable memory for communication and such). If you need part
of your upper memory to be cached, and other part not, you should mmap() that
memory (which you want to be uncached) _after_ doing open("/dev/mmuhack").
Another way is to modify mmuhack.c to suit your needs and rebuild the module.


Usage
-----

The very first thing to do is to load the kernel module (mmuhack.o) into the
running kernel. But before that you should try to unload mmuhack module,
because other program might have left a different version loaded with
different memory configuration, which may not suit your program.

system("/sbin/rmmod mmuhack");
system("/sbin/insmod mmuhack.o");

Now you can assume the module is loaded into kernel and open /dev/mmuhack
device. You don't need to worry about previous calls failing, because in that
case open() will simply fail and nothing bad will happen.

IMPORTANT: you _must_ do the open() call _after_ you initialize your graphics
library or allocate your memory, because it can only work with memory which is
already allocated, it won't affect memory you or your lib allocates after the
open() call.

int mmufd = open("/dev/mmuhack", O_RDWR);
if(mmufd < 0)
{
  printf("MMU hack failed");
}
else
{
  printf("MMU hack loaded");
  close(mmufd);
}

If the above call succeeded, you are all done.
I recommend to unload the module when your program exits, because the other
program may want to load a different mmuhack.o and may fail, because you left
your mmuhack.o loaded (it does not get unloaded automatically on exit).

system("/sbin/rmmod mmuhack");


Flushing the cache
------------------

If using mmuhack.o causes your program to display artifacts (see "Operation"
section for explanation), you will need to flush the CPU cache. This should
be done after finishing every frame and just before flipping your display
buffer/surface. You will need to add flush_uppermem_cache.s file to your
Makefile/project and add a call to flush_uppermem_cache() just before final
framebuffer flip or blit.

flush_uppermem_cache() has 3 parameters. First param is the start address,
second param is the end address, third one should always be 0. The addresses
should be virtual ones (most often pointers to the start/end of your
framebuffer). Example:

flush_uppermem_cache(screen_surface->pixels, screen_surface->pixels + 320*240, 0);


Credits
-------

Original idea/implementation: Squidge (this whole thing is also known as squidgehack)
Kernel module: NK
Documentation: notaz

