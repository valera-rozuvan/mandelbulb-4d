#if defined(_WIN32) || defined(WIN32)
  #include "rand48.h"
#endif

bool isPowerOfTwo(uint64_t x)
{
  return ((x != 0) && ((x & (~x + 1)) == x));
}

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

void test64bit(void)
{
  const int64_t ARRAY_SIZE_6GB = 1024LL * 1024LL * 1024LL * 6;

  int64_t numLoops = 0;
  int64_t counter = 0;

  srand48(time(NULL));

  int64_t i1 = 0x0000444400004443LL;
  int64_t i2 = -100;

  block = (unsigned char *)calloc(sizeof(unsigned char) * ARRAY_SIZE_6GB, sizeof(unsigned char));

  if (block) {
    std::cout << "Allocated 6 Gig block." << std::endl;
  } else {
    std::cout << "Unable to allocate 6 Gig block." << std::endl;

    exit(1);
  }

  block[235LL] = 235;

  numLoops = unsigned_uniform_random(3000000, 3100000);

  for (counter = 0; counter < numLoops; counter += 1) {
    i2 = -100;

    while (i2 <= 0 || i2 > ARRAY_SIZE_6GB) {
      i1 = unsigned_uniform_random(1, ARRAY_SIZE_6GB - 1);
      i2 = 1024LL * 1024LL * 1024LL * 6 - i1;
    }

    block[i2] = (unsigned char)unsigned_uniform_random(0, 255);
  }
}

void test64bit_freeMem(void) {
  free(block);
}
