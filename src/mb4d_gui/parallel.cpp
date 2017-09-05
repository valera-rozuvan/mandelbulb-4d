#include <stdio.h>
#include "parallel.hpp"

Parallel::Parallel(void)
{

}

Parallel::~Parallel(void)
{
  fprintf(stdout, "Parallel::~Parallel destructor.\n");
}
