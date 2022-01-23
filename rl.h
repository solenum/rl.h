/* rl.h - v0.0.0 - a MIT-licensed roguelike building block
  ...tbc
*/


#ifndef RLH
#define RLH

#define RLH_VER "0.0.0"

#ifndef RLH_NO_STDIO
#include <stdio.h>
#endif // RL_NO_STDIO

#ifndef RLHDEF
#ifdef RLH_STATIC
#define RLHDEC static
#define RLHDEF static
#else
#define RLHDEC extern
#define RLHDEF
#endif
#endif

// Public API
RLHDEC void rlh_init();

#endif // RLH
// -- END INCLUDE --
// --------------------


// --------------------
// -- BEGIN IMPLEMENTATION --
#ifdef RLH_IMPLEMENTATION

// debug printing
// TODO: switch to RLH_LOGLEVEL <LOG,DBG,ERR> ?
#ifdef RLH_DEBUG
#define RLH_PDG(f_, ...) printf(("\033[0;34m[DBG] " f_), ##__VA_ARGS__)
#else
#define RLH_PDG(...)
#endif // RLH_DEBUG

// error printing
#define RLH_PER(f_, ...) printf(("\033[0;31m[ERR] " f_), ##__VA_ARGS__)

// TODO: switch to RLH_LOG(<LEVEL>, ...) ?
#define RLH_LOG(f_, ...) printf(("\033[0m[LOG] " f_), ##__VA_ARGS__)

#ifndef RLH_MALLOC
#define RLH_MALLOC(s)     malloc(s)
#define RLH_REALLOC(p,nz) realloc(p,ns)
#define RLH_FREE(p)       free(p)
#endif // RLH_MALLOC

#define RLH_NEW(p, n) ((p) = RLH_MALLOC((n) * sizeof *(p)))

RLHDEF void rlh_init() {
  RLH_LOG("Initializing rl.h -- version %s\n", RLH_VER);
}

#endif // RLH_IMPLEMENTATION
