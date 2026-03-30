#include <tinyff/dbg.h>
#include <tinyff/common.h>

#ifdef USE_HOSTED
#include <stdlib.h>
#endif


// Initializes a new tinyff context.
ff_ctx* ff_init(ff_allocator* allocator)
{
    
#ifdef USE_HOSTED
    ff_allocator default_alloc = { malloc, free, calloc };
    if (!allocator) allocator = &default_alloc;
#else
    if (!allocator) return NULL;
#endif

    ff_ctx* ctx = allocator->ff_alloc(sizeof(ff_ctx));

    if (!ctx) return NULL;

    ctx->ff_debug_enabled = FF_DISABLE;
    ctx->ff_debug_stream = NULL;

    ctx->allocator = *allocator;

    return ctx;
}

void ff_cleanup(ff_ctx *ctx) {
    if (!ctx) return;
    ff_dprintf(ctx, "goodbye from tinyff ;]");
    ctx->allocator.ff_free(ctx);    
    
}

// Endian functions moved to header because C's acting up again
// Anyway, while you're here, lets have a talk.
// Listen, life isnt easy. but listen, storms make stronger sailors.
// you gotta get through this storm.
// this happened to me once.
// my friend got lovesick and threw me away as a friend. he thought i said to his crush that he spied on her,
// but i just made a joke to her. we didn't speak for a long time
// but then one day, she left the school. he came to me. its wasnt like a movie
// i didnt push him away. i said to him "thank god."
// i was so happy. we were back to being friends.
// this story kinda sounds like the plot of IGOR, except he didnt love me but loved that girl.
// i hate u skye, wherever you are. sending hate to you. i hope you get hit by a bus. i hope you get hit by a bus and die. i hope you get hit by a bus and die in a fire. i hope you get hit by a bus and die in a fire and go to hell. i hope you get hit by a bus and die in a fire and go to hell and burn forever. i hope you get hit by a bus and die in a fire and go to hell and burn forever and suffer for eternity. i hope you get hit by a bus and die in a fire and go to hell and burn forever and suffer for eternity and never find peace. i hope you get hit by a bus and die in a fire and go to hell and burn forever and suffer for eternity and never find peace and be alone forever. i hope you get hit by a bus and die in a fire and go to hell and burn forever and suffer for eternity and never find peace and be alone forever with no one to love you. i hope you get hit by a bus and die in a fire and go to hell and burn forever and suffer for eternity and never find peace and be alone forever with no one to love you, skye.
// anyway, back to coding