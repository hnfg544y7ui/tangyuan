#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".lua_port_api.data.bss")
#pragma data_seg(".lua_port_api.data")
#pragma const_seg(".lua_port_api.text.const")
#pragma code_seg(".lua_port_api.text")
#endif
//--- support functions for Lua

#include <stdlib.h>
#include "cpu.h"

#if 1
void abort()
{
    printf("lua aborted !!!!!\n");
    while (1);
}
#endif

char *_user_strerror(
    int errnum,
    int internal,
    int *errptr)
{
    /* prevent warning about unused parameters */
    (void)errnum;
    (void)internal;
    (void)errptr;

    return 0;
}

unsigned int luaP_makeseed(void)
{
    int rand = JL_RAND->R64L;
    return rand;
}

void luaP_writeline(void)
{
    putchar('\n');
}

void luaP_writestring(const char *p, int l)
{
    for (int i = 0; i < l; i++) {
        putchar(*p++);
    }
}


#if 1
void *my_realloc(void *ptr, size_t osize, size_t nsize)
{
    /* printf("realloc(%x, %d)\n", ptr, size); */
    if (!ptr) {
        return malloc(nsize);
    }
    if (nsize == 0) {
        free(ptr);
        ptr = NULL;
        return NULL;
    }

    void *p = malloc(nsize);
    if (p) {
        if (ptr != NULL) {
            // 照较小的buffer大小拷贝，防止溢出
            size_t copy_size = (osize > nsize) ? nsize : osize;
            memcpy(p, ptr, copy_size);
            free(ptr);
        }
        return p;
    }
    return NULL;
}
#endif
