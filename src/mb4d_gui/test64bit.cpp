#include <iostream>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.hpp"
#include "test64bit.hpp"

#if defined(_WIN32) || defined(WIN32)
#include "rand48.h"
#endif

unsigned char *block;
const uint64_t ARRAY_SIZE_6GB = 1024LL * 1024LL * 1024LL * 6;

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

uint64_t uint64UniformRandom(uint64_t low, uint64_t high)
{
  static const uint64_t M = ~(uint64_t)0;

  uint64_t range = high - low;
  uint64_t toExclude = isPowerOfTwo(range) ? 0 : M % range + 1;
  uint64_t res;

  // Eliminate `toExclude` possible values from consideration.
  while ((res = urand64()) < toExclude) {}

  return low + res % range;
}

void test64bitAllocMem(void)
{
  std::cout << "Trying to allocate 6 GB block of memory." << std::endl;
  block = (unsigned char *)calloc(sizeof(unsigned char) * ARRAY_SIZE_6GB, sizeof(unsigned char));

  if (block) {
    std::cout << "Allocated 6 GB." << std::endl;
  } else {
    std::cout << "Unable to allocate 6 GB. Exiting..." << std::endl;

    exit(1);
  }
}

void test64bitRunTest(void)
{
  unsigned int numLoops;
  unsigned int counter;

  int64_t i1;
  int64_t i2;

  std::cout << "Testing pre-defined memory access." << std::endl;
  block[235LL] = 235;

  std::cout << "Testing random memory access." << std::endl;

  srand48(time(NULL));
  numLoops = rndFromRange(3000, 5000);

  i1 = 0x0000444400004443LL;
  i2 = -0x0000444400004443LL;

  for (counter = 0; counter < numLoops; counter += 1) {
    i2 = -100;

    while (i2 < 0 || i2 >= ARRAY_SIZE_6GB) {
      i1 = uint64UniformRandom(1, ARRAY_SIZE_6GB);
      i2 = ARRAY_SIZE_6GB - i1;
    }

    block[i2] = (unsigned char)rndFromRange(0, 255);
  }

  std::cout << "64 bit memory tests complete." << std::endl;
}

void test64bitFreeMem(void)
{
  std::cout << "Trying to de-allocate 6 GB of memory..." << std::endl;
  free(block);
  std::cout << "Memory de-allocated successfully." << std::endl;
}
