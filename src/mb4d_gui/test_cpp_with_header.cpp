#include "test_cpp_with_header.hpp"

bool showBrn = false;

void testFunc(void) {
  int tenVar = 0;
  int twoVar = 1;

  if (tenVar == 0) {
    twoVar = 5;
  } else {
    twoVar = 10;
  }

  if (twoVar == 10) {
    tenVar = 5;
  }
}
