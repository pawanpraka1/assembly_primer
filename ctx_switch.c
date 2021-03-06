#include <stdio.h>
#include <stdlib.h>

typedef size_t uintptr_t;

#ifdef __x86_64__
	#define NUM_REG 4
#elif __i386__
	#define NUM_REG 3
#else
	#define NUM_REG 0
#endif

struct stack_context {
    uintptr_t sp_reg;
    uintptr_t other_reg[NUM_REG];
}actx, bctx, pctx;

struct stack_context *cur_ctx;
#define STACK_SIZE 4096

unsigned int astack[STACK_SIZE];
unsigned int bstack[STACK_SIZE];

extern void my_yeild(struct stack_context *rctx);
extern void funa(void);
extern void funb(void);

void funa(void)
{
    int i = 0;
    my_yeild(&bctx);
    while (i++ < 20) {
        printf("a %d\n", i);
        sleep(1);
        my_yeild(&bctx);
    }
    exit(0);
}

void funb(void)
{
    int i = 0;
    my_yeild(&actx);
    while (i++ < 20) {
        printf("b %d\n", i);
        sleep(1);
        my_yeild(&actx);
    }
    exit(0);
}

int main()
{
    cur_ctx = &pctx;
    uintptr_t *sp = (uintptr_t *) ((((uintptr_t)&astack[STACK_SIZE]) & -16L) - sizeof(uintptr_t));
    *sp = (uintptr_t)funa;
    actx.sp_reg = (uintptr_t)sp;
    
    sp = (uintptr_t *) ((((uintptr_t)&bstack[STACK_SIZE]) & -16L) - sizeof(uintptr_t));
    *sp = (uintptr_t)funb;
    bctx.sp_reg = (uintptr_t) sp;
    my_yeild(&actx); 
    return 0;
}
