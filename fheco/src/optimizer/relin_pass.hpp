#include "program.hpp"

namespace optimizer

{

// relinearization pass

class RelinPass
{
private:
  ir::Program *program;
  static size_t relin_instruction_id;
  const std::string inst_keyword = "relin";

public:
  RelinPass() = default;
  ~RelinPass() {}
  RelinPass(ir::Program *prgm) : program(prgm) {}
  void simple_relinearize();
};

} // namespace optimizer
