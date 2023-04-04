#include "seal/seal.h"
#include <vector>
#include <map>
#include <cstdint>

seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(4096);
params.set_plain_modulus(seal::PlainModulus::Batching(4096,18));
params.set_coeff_modulus(seal::CoeffModulus::Create(4096,{36, 36, 37}));
seal::SEALContext context(params,true,seal::sec_level_type::tc128);
return context;
}

std::vector<int> get_rotations_steps(){
std::vector<int> steps = {1,2};
return steps; 
}

void hamming_distance(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
seal::Ciphertext ciphertext1 = encrypted_inputs["c1"];
seal::Ciphertext ciphertext2;
evaluator.add(ciphertext1,ciphertext0,ciphertext2);
std::vector<std::int64_t> scalar0_clear(4096,2);
seal::Plaintext scalar0;
encoder.encode(scalar0_clear,scalar0);
seal::Ciphertext ciphertext3;
evaluator.negate(ciphertext2,ciphertext3);
seal::Ciphertext ciphertext4;
evaluator.add_plain(ciphertext3,scalar0,ciphertext4);
seal::Ciphertext ciphertext14;
evaluator.multiply(ciphertext4,ciphertext2,ciphertext14);
seal::Ciphertext ciphertext5;
evaluator.relinearize(ciphertext14,relin_keys,ciphertext5);
seal::Ciphertext ciphertext5_level0;
evaluator.mod_switch_to_next(ciphertext5,ciphertext5_level0);
seal::Ciphertext ciphertext6;
evaluator.rotate_rows(ciphertext5,2, galois_keys,ciphertext6);
seal::Ciphertext ciphertext6_level0;
evaluator.mod_switch_to_next(ciphertext6,ciphertext6_level0);
seal::Ciphertext ciphertext7;
evaluator.add(ciphertext6_level0,ciphertext5_level0,ciphertext7);
seal::Ciphertext ciphertext8;
evaluator.rotate_rows(ciphertext7,1, galois_keys,ciphertext8);
seal::Ciphertext ciphertext9;
evaluator.add(ciphertext8,ciphertext7,ciphertext9);
encrypted_outputs.insert({"c_result",ciphertext9});
}
