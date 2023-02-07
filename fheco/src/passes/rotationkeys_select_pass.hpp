#pragma once

#include "program.hpp"

namespace fheco_passes
{

class RotationKeySelctionPass
{

private:
  ir::Program *program;

  /*
    This function write a value in Non-Adjacent form (NAF)
  */
  std::vector<int> naf(int value);
  void rewrite_rotation_node_with_naf(const ir::Program::Ptr &node, const std::vector<int32_t> naf_components);

public:
  RotationKeySelctionPass(ir::Program *prgm) : program(prgm) {}
  /*
    For now this function targets SEAL backend where instead of generating all power of two keys and in order to avoid
    overhead that comes with rotation steps that are not a power of 2, the functions traverse the program and return
    unique rotation steps since each rotation is associated to a galois element that defines the galois automorphism
    applied in rotation operation preceding key switching. We assume that this targets SEAL only since at the moment we
    don't have enough knowledge on how rotation is implemented for other backends and the operation is not totally
    scheme dependent as one might guess, it may differ from one implemention to another while the core idea is still the
    same (i.e, Galois Automorphisms).
  */
  std::vector<int> get_unique_rotation_steps();
};

} // namespace fheco_passes
