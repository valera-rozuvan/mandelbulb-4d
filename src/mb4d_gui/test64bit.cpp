#include <iostream>
#include "utils.hpp"
#include "test64bit.hpp"

unsigned char *block;
const uint64_t ARRAY_SIZE_6GB = 1024LL * 1024LL * 1024LL * 6;

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
