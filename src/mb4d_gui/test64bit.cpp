#include <iostream>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include "test64bit.hpp"

#if defined(_WIN32) || defined(WIN32)
#include "rand48.h"
#endif

// Taken from:
//   http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
bool isPowerOfTwo(uint64_t x)
{
  return ((x != 0) && ((x & (~x + 1)) == x));
}

// Taken from:
//   https://stackoverflow.com/a/20177601
uint64_t urand64(void)
{
  uint64_t hi = lrand48();
  uint64_t md = lrand48();
  uint64_t lo = lrand48();

  return (hi << 42) + (md << 21) + lo;
}

uint64_t unsigned_uniform_random(uint64_t low, uint64_t high)
{
  static const uint64_t M = ~(uint64_t)0;

  uint64_t range = high - low;
  uint64_t to_exclude = isPowerOfTwo(range) ? 0 : M % range + 1;
  uint64_t res;

  // Eliminate `to_exclude` possible values from consideration.
  while ((res = urand64()) < to_exclude) {}

  return low + res % range;
}

unsigned char *block = NULL;
const uint64_t ARRAY_SIZE_6GB = 1024LL * 1024LL * 1024LL * 6;

void test64bit(void)
{
  std::cout << "Trying to allocate 6 GB block of memory." << std::endl;
  block = (unsigned char *)calloc(sizeof(unsigned char) * ARRAY_SIZE_6GB, sizeof(unsigned char));

  if (block) {
    std::cout << "Allocated 6 GB." << std::endl;
  } else {
    std::cout << "Unable to allocate 6 GB. Exiting..." << std::endl;

    exit(1);
  }

  std::cout << "Testing pre-defined memory access." << std::endl;
  block[235LL] = 235;

  std::cout << "Testing random memory access." << std::endl;
  srand48(time(NULL));
  uint64_t numLoops = unsigned_uniform_random(3000, 5000);

  int64_t i1 = 0x0000444400004443LL;
  int64_t i2 = -0x0000444400004443LL;

  for (uint64_t counter = 0; counter < numLoops; counter += 1) {
    i2 = -100;

    while (i2 < 0 || i2 >= (int64_t)ARRAY_SIZE_6GB) {
      i1 = unsigned_uniform_random(1, ARRAY_SIZE_6GB);
      i2 = ARRAY_SIZE_6GB - i1;
    }

    block[i2] = (unsigned char)unsigned_uniform_random(0, 255);
  }

  std::cout << "64 bit memory tests complete." << std::endl;
}

void test64bit_freeMem(void) {
  std::cout << "Trying to de-allocate 6 GB of memory..." << std::endl;
  free(block);
  std::cout << "Memory de-allocated successfully." << std::endl;
}
