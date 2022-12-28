#include "params_selector.hpp"
#include "encryption_parameters.hpp"

namespace params_selector
{

void ParameterSelector::fix_parameters(EncryptionParameters &params)
{
  // encryption_parameters.set_polynomial_modulus_degree(program->get_dimension());
  if (
    params.plaintext_modulus == 0 && params.plaintext_modulus_bit_length == 0 &&
    program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
    throw("plaintext modulus missing");

  // now at this level we are sure that we have a plaintext modulus
  // plaintext modulus must be a prime number and congruent to 1 mod(2n) where n is number of slots

  size_t number_of_slots = params.poly_modulus_degree;

  if (program->get_encryption_scheme() == fhecompiler::Scheme::ckks)
    number_of_slots /= 2;

  if (params.plaintext_modulus > 0 && (params.plaintext_modulus % (2 * number_of_slots) != 1))
  {
    throw("batching not supported for the given plaintext modulus value");
  }

  if (params.poly_modulus_degree == 0)
  {
    // compiler must fix this parameter based on noise budget for computation and security level
    // we estimate noise budget based on a heuristics or circuit depth ...
  }

  else if ((params.poly_modulus_degree & (params.poly_modulus_degree - 1)) > 0)
  {
    throw("polynomial modulus degree must be a power of 2");
  }

  /*
    at this level we have a value for polynomial modulus degree, but we need to check whether this value is valid or
    not. to check the validity of this value we need to consider noise budget and security level
    we can instead use a value
  */

  if (params.coef_modulus.size() == 0)
  {
    if (program->get_encryption_scheme() != fhecompiler::Scheme::bfv)
    {
      throw("we cant set a default value for coefficient modulus based on the avaible parameter settings, it is not "
            "bfv");
    }
    /*
      in case encryption scheme is set to be BFV, then we can set coefficient modulus according to FHE standard using
      polynomial modulus degree and security level (128-bit by default)
    */
  }

  /*
    for now we don't have a mechanism to validate a set of fixed parameters, but this will be done either using
    heuristic bounds and/or experimental data based on the multiplicative depth of the circuit and type of scheme ...
  */
}

} // namespace params_selector
