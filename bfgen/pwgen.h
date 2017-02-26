#ifndef PWGEN_H
#define PWGEN_H

#include <stdint.h>

enum {
  PWGEN_FIXED_LIST,
  PWGEN_ALL_CAPS,
  PWGEN_ALL_LEET_SUBS,
  PWGEN_ALL_CAPS_LEET_SUBS,
  PWGEN_SINGLE_CAPS,
  PWGEN_SINGLE_LEET_SUBS,
  PWGEN_BRUTE_FORCE
};

typedef struct pwgen_state {
  uint8_t state;
  uint32_t ctr;
  uint32_t ictr;
} pwgen_state_t;

void pwgen_init(pwgen_state_t *pwgen);
void pwgen_output(pwgen_state_t *pwgen, uint8_t *buf, uint8_t len);

#endif
