#pragma once

#include "program.hpp"
#include <memory>

namespace fheco_passes

{

// relinearization pass

class RelinPass
{
private:
  std::shared_ptr<ir::Program> program;

public:
  RelinPass() = default;
  ~RelinPass() {}
  RelinPass(const std::shared_ptr<ir::Program> &prgm) : program(prgm) {}
  void simple_relinearize();
};

} // namespace fheco_passes
