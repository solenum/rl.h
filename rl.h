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

enum {
  RLH_ERR       = 0,
  RLH_WARN      = 1,
  RLH_NORM      = 2
} RLH_LOG_LEVEL_E;

// Public API
RLHDEC void rlh_init();

#endif // RLH
// -- END INCLUDE --
// --------------------


// --------------------
// -- BEGIN IMPLEMENTATION --
#ifdef RLH_IMPLEMENTATION

static const char *rlh_log_prefix[] = {
  "\033[0;31m[ERR]",
  "\033[0;33m[WARN]",
  "\033[0;0m[LOG]"
};

#define RLH_LOG(p, f_, ...) printf(("%s " f_), rlh_log_prefix[p], ##__VA_ARGS__)

#ifdef RLH_DEBUG
#define RLH_DBG(f_, ...) printf(("\033[0;34m[DBG] " f_), ##__VA_ARGS__)
#else
#define RLH_DBG(...)
#endif

#ifndef RLH_MALLOC
#define RLH_MALLOC(s)     malloc(s)
#define RLH_REALLOC(p,nz) realloc(p,ns)
#define RLH_FREE(p)       free(p)
#endif

#define RLH_NEW(p, n) ((p) = RLH_MALLOC((n) * sizeof *(p)))

RLHDEF void rlh_init() {
  RLH_LOG(RLH_NORM, "Initializing rl.h -- version %s\n", RLH_VER);
}

#endif // RLH_IMPLEMENTATION
