#include "pwgen.h"
#include "wordlist.h"
#include <string.h>

const static uint8_t brute_force_character_set_length = 90;
const static uint8_t* brute_force_character_set = (uint8_t*) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[{]}/?=+-_'\\\";<>,.";

const static uint64_t brute_force_offset_tables[7] = {
  90, 8190, 737190, 66347190, 5971247190, 537412247190, 48367102247190
};

void pwgen_init(pwgen_state_t *pwgen)
{
  pwgen->state = PWGEN_FIXED_LIST;
  pwgen->ctr = 0;
  pwgen->ictr = 0;
}

static void pwgen_output_internal(pwgen_state_t *pwgen, uint8_t *buf, uint8_t len)
{
  uint32_t i = 0;
  uint32_t j = 0;
  uint8_t ch;
  uint64_t x;
  
  switch(pwgen->state)
  {
  case PWGEN_FIXED_LIST:
    ch = wordlist[pwgen->ctr];
    while (i < len - 1 && ch != 0)
    {
      buf[i] = ch;
      i++;
      (pwgen->ctr)++;
      ch = wordlist[pwgen->ctr];
    }
    (pwgen->ctr)++;
    if (pwgen->ctr >= WORDLIST_LENGTH)
    {
      pwgen->state = PWGEN_ALL_CAPS;
      pwgen->ctr = 0;
      pwgen->ictr = 0;
    }
    buf[i] = 0;
    break;
  case PWGEN_ALL_CAPS:
    ch = wordlist[pwgen->ctr];
    while (i < len - 1 && ch != 0)
    {
      if (ch >= 'a' && ch <= 'z')
      {
        ch = ch - ('a' - 'A');
      }
      buf[i] = ch;
      i++;
      (pwgen->ctr)++;
      ch = wordlist[pwgen->ctr];
    }
    (pwgen->ctr)++;
    if (pwgen->ctr >= WORDLIST_LENGTH)
    {
      pwgen->state = PWGEN_ALL_LEET_SUBS;
      pwgen->ctr = 0;
      pwgen->ictr = 0;
    }
    buf[i] = 0;
    break;
  case PWGEN_ALL_LEET_SUBS:
    ch = wordlist[pwgen->ctr];
    while (i < len - 1 && ch != 0)
    {
      if (ch >= 'a' && ch <= 'z')
      {
        ch = ch - ('a' - 'A');
      }
      else if (ch == 'o')
      {
        ch = '0';
      }
      else if (ch == 'e')
      {
        ch = '3';
      }
      else if (ch == 'i')
      {
        ch = '1';
      }
      buf[i] = ch;
      i++;
      (pwgen->ctr)++;
      ch = wordlist[pwgen->ctr];
    }
    (pwgen->ctr)++;
    if (pwgen->ctr >= WORDLIST_LENGTH)
    {
      pwgen->state = PWGEN_ALL_CAPS_LEET_SUBS;
      pwgen->ctr = 0;
      pwgen->ictr = 0;
    }
    buf[i] = 0;
    break;
  case PWGEN_ALL_CAPS_LEET_SUBS:
    ch = wordlist[pwgen->ctr];
    while (i < len - 1 && ch != 0)
    {
      if (ch == 'o')
      {
        ch = '0';
      }
      else if (ch == 'e')
      {
        ch = '3';
      }
      else if (ch == 'i')
      {
        ch = '1';
      }
      buf[i] = ch;
      i++;
      (pwgen->ctr)++;
      ch = wordlist[pwgen->ctr];
    }
    (pwgen->ctr)++;
    if (pwgen->ctr >= WORDLIST_LENGTH)
    {
      pwgen->state = PWGEN_SINGLE_CAPS;
      pwgen->ctr = 0;
      pwgen->ictr = 0;
    }
    buf[i] = 0;
    break;
  case PWGEN_SINGLE_CAPS:
    j = pwgen->ctr;
    ch = wordlist[j];
    while (i < len - 1 && ch != 0)
    {
      if (i == pwgen->ictr)
      {
        if (ch >= 'a' && ch <= 'z')
        {
          ch = ch - ('a' - 'A');
        }
        else if (ch >= '2' && ch <= '9')
        {
          ch -= 1;
        }
        else if (ch == '0')
        {
          ch = 'O';
        }
        else if (ch == '1')
        {
          ch = '!';
        }
      }
      buf[i] = ch;
      i++;
      j++;
      ch = wordlist[j];
    }
    pwgen->ictr++;
    if (i == pwgen->ictr)
    {
      pwgen->ictr = 0;
      pwgen->ctr = j + 1;
    }
    if (pwgen->ctr >= WORDLIST_LENGTH)
    {
      pwgen->state = PWGEN_SINGLE_LEET_SUBS;
      pwgen->ctr = 0;
      pwgen->ictr = 0;
    }
    buf[i] = 0;
    break;
  case PWGEN_SINGLE_LEET_SUBS:
    j = pwgen->ctr;
    ch = wordlist[j];
    while (i < len - 1 && ch != 0)
    {
      if (i == pwgen->ictr)
      {
        if (ch == 'o')
        {
          ch = '0';
        }
        else if (ch == 'e')
        {
          ch = '3';
        }
        else if (ch == 'i')
        {
          ch = '1';
        }
      }
      buf[i] = ch;
      i++;
      j++;
      ch = wordlist[j];
    }
    pwgen->ictr++;
    if (i == pwgen->ictr)
    {
      pwgen->ictr = 0;
      pwgen->ctr = j + 1;
    }
    if (pwgen->ctr >= WORDLIST_LENGTH)
    {
      pwgen->state = PWGEN_BRUTE_FORCE;
      pwgen->ctr = 0;
      pwgen->ictr = 0;
    }
    buf[i] = 0;
    break;
  default:
    x = (((uint64_t) pwgen->ctr) << 32) + ((uint64_t) pwgen->ictr);
    // Find size based on offset
    for (i = 1; x > brute_force_offset_tables[i-1]; i++) {}
    x -= brute_force_offset_tables[i-1];
    for (j = 0; j < i; j++)
    {
      buf[j] = brute_force_character_set[x % brute_force_character_set_length];
      x /= brute_force_character_set_length;
    }
    buf[j] = 0;
    
    // Two-field increment
    if (++(pwgen->ictr) == 0) pwgen->ctr++;
  }
}

void pwgen_output(pwgen_state_t *pwgen, uint8_t *buf, uint8_t len)
{
  uint8_t ibuf[64];
  buf[len - 1] = 0;
  
  uint8_t l = (len > 64) ? 64 : len;
  
  while(1)
  {
    pwgen_output_internal(pwgen, ibuf, l);
    if (strncmp((char*) ibuf, (char*) buf, l))
    {
      strncpy((char*) buf, (char*) ibuf, l);
      return;
    }
  }
}

