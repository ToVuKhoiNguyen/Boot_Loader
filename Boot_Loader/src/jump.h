#ifndef JUMP_H_
#define JUMP_H_

#include <stdint.h>

void JUMP_ToApplication(uint32_t vector_addr) __attribute__((noreturn));

#endif
