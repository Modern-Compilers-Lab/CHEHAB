#include "params_simulation.hpp"
#include <iostream>

int main(int argc, char **argv)
{
  int initial_plain_m_size = 14;
  if (argc > 1)
    initial_plain_m_size = atoi(argv[1]);

  int xdepth = 23;
  if (argc > 2)
    xdepth = atoi(argv[2]);

  bool use_least_levels = false;
  if (argc > 3)
    use_least_levels = atoi(argv[3]);

  bfv_params_simulation(initial_plain_m_size, xdepth, xdepth + 1, use_least_levels);

  return 0;
}
