#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>

static void my_init_hook (void);
static void *my_malloc_hook (size_t, const void *);
static void my_free_hook (void*, const void *);

/* Variables to save original hooks. */
static void *(*old_malloc_hook) (size_t, const void *);
static void *(*old_free_hook) (void*, const void *);

/* Override initializing hook from the C library. */
void (*__malloc_initialize_hook) (void) = my_init_hook; 
//void (*__MALLOC_HOOK_VOLATILE __malloc_initialize_hook) (void) = my_init_hook;  <- druga opcja

int i = 0;

int main(int argc, char *argv[])
{
 int size,blocks;
 
 if(argc < 3)
 {
   printf("Za mala ilosc argumentow\n");
   return(-1);
 }
 else
 {
 size = atoi(argv[1]);
 blocks = atoi(argv[2]);
 
 char* alloc_blocks[blocks];
 
 for( i = 0; i < blocks; i=i+1)
 {
   alloc_blocks[i] = malloc( size*1024*sizeof(char));
 }
 
 for( i = 0; i<blocks; i=i+1)
 {
   free(alloc_blocks[i]);
 }
 
 //char** alloc_blocks = malloc(blocks*sizeof(char*));
 
 return 0;
 }
}

static void my_init_hook (void)
{
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    __malloc_hook = my_malloc_hook;
    __free_hook = my_free_hook;
}

static void * my_malloc_hook (size_t size, const void *caller)
{
    void *result;
    struct mallinfo info = mallinfo();
    int old_uordblks = info.uordblks;
    int old_hblkhd = info.hblkhd;
    
    
    /* Restore all old hooks */
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
    /* Call recursively */
    result = malloc (size);
    /* Save underlying hooks */
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    /* printf might call malloc, so protect it too. */
    //printf ("malloc (%u) returns %p\n", (unsigned int) size, result);
    /* Restore our own hooks */
    __malloc_hook = my_malloc_hook;
    __free_hook = my_free_hook;
    info = mallinfo();
    
    
    printf("uordblks:\n %d -> %d\n",old_uordblks,info.uordblks);
    printf("hblkhd:\n %d -> %d\n",old_hblkhd,info.hblkhd);
    
    return result;
}

static void my_free_hook (void *ptr, const void *caller)
{
    /* Restore all old hooks */
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
    /* Call recursively */
    free (ptr);
    /* Save underlying hooks */
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    /* printf might call free, so protect it too. */
   // printf ("freed pointer %p\n", ptr);
    /* Restore our own hooks */
    __malloc_hook = my_malloc_hook;
    __free_hook = my_free_hook;
}
