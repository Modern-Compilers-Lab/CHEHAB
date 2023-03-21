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
  static size_t relin_instruction_id;
  const std::string inst_keyword = "relin";

public:
  RelinPass() = default;
  ~RelinPass() {}
  RelinPass(const std::shared_ptr<ir::Program> &prgm) : program(prgm) {}
  void simple_relinearize();
};

} // namespace fheco_passes
