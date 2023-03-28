#include "seal/seal.h"
#include <vector>
#include <map>
#include <cstdint>

seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(4096);
params.set_plain_modulus(seal::PlainModulus::Batching(4096,17));
params.set_coeff_modulus(seal::CoeffModulus::Create(4096,{54, 55}));
seal::SEALContext context(params,false,seal::sec_level_type::tc128);
return context;
}

std::vector<int> get_rotations_steps(){
std::vector<int> steps = {1,2,4,5,1017};
return steps; 
}

void gx_kernel(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
seal::Ciphertext ciphertext23;
evaluator.rotate_rows(ciphertext0,2, galois_keys,ciphertext23);
seal::Ciphertext ciphertext24;
evaluator.add(ciphertext23,ciphertext0,ciphertext24);
seal::Ciphertext ciphertext26;
evaluator.add(ciphertext24,ciphertext24,ciphertext26);
seal::Ciphertext ciphertext45;
evaluator.rotate_rows(ciphertext26,1017, galois_keys,ciphertext45);
seal::Ciphertext ciphertext30;
evaluator.rotate_rows(ciphertext0,1017, galois_keys,ciphertext30);
seal::Ciphertext ciphertext31;
evaluator.add(ciphertext0,ciphertext30,ciphertext31);
seal::Ciphertext ciphertext40;
evaluator.rotate_rows(ciphertext31,5, galois_keys,ciphertext40);
seal::Ciphertext ciphertext35;
evaluator.rotate_rows(ciphertext0,4, galois_keys,ciphertext35);
seal::Ciphertext ciphertext36;
evaluator.add(ciphertext0,ciphertext35,ciphertext36);
seal::Ciphertext ciphertext41;
evaluator.add(ciphertext36,ciphertext40,ciphertext41);
seal::Ciphertext ciphertext46;
evaluator.add(ciphertext41,ciphertext45,ciphertext46);
seal::Ciphertext ciphertext13;
evaluator.rotate_rows(ciphertext46,1, galois_keys,ciphertext13);
seal::Ciphertext ciphertext14 = ciphertext13;
encrypted_outputs.insert({"c12",ciphertext14});
}
