#include "support.h"

typedef unsigned long DWORD;
#define _LEN 32
const DWORD LEN = _LEN;
DWORD tab_in[_LEN];
DWORD tab_out[_LEN];
const unsigned int body_iterations = 50;

DWORD __attribute__((noinline)) vadd_body(DWORD counter)
{
  DWORD buf1, buf2;
  asm volatile (
                "addi x0, x0, 0 \n"
                "vsetvli x0, %[LEN], e32,m1,ta,ma \n"
                "vle32.v v1, (%[tab_in]) \n"
                "vadd.vi v2, v1, 0 \n"
                "start_vadd_%=: \n"
                "vadd.vv v2, v2, v1 \n"
                "addi %[counter], %[counter], -1 \n"
                "li %[buf1], 2 \n"
                "li %[buf2], 4 \n"
                "add %[buf1], %[buf2], %[buf1] \n"
                "add %[buf1], %[buf2], %[buf1] \n"
                "addi %[buf1], %[buf1], -1 \n"
                "bne x0, %[counter], start_vadd_%= \n"
                "vse32.v v2, (%[tab_out]) \n"
                : [counter]"+r"(counter),
                  [buf1] "=&r" (buf1),
                  [buf2] "=&r" (buf2)
                : [LEN]"r"(LEN),
                  [tab_in]"r"(tab_in),
                  [tab_out]"r"(tab_out)
                : "v1", "v2", "memory");
  return 0;
}

void initialise_benchmark (void)
{
  for(unsigned int i = 0; i < LEN; i++)
  {
    tab_in[i]=i;
  }
}

void warm_caches (int __attribute__((unused)) heat)
{
  vadd_body(4);
  return;
}

int benchmark (void)
{
  return vadd_body(body_iterations);
}

int verify_benchmark (int __attribute__((unused)) r)
{
  int expected =0;
  int got = 0;
  for(unsigned int i = 0; i < LEN; i++)
  {
    got += tab_out[i];
    expected += tab_in[i]*(body_iterations+1);
  }

//  asm volatile(
//  "li t0, 0x80000004 \n"
//  "sw %[out], 0(t0) \n"
//  "li t0, 0x80000000 \n"
//  "sw a0, 0(t0) \n"
//  :
//  : [out] "r"(r)
//  : "memory");
  return expected == got;
}
