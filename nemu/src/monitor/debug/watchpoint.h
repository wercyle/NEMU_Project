#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char exp[32];
  word_t value;

  /* TODO: Add more members if necessary */

} WP;


WP* new_wp();

WP* whereishead();
WP* whereisfree();

void free_wp(int N);





#endif
