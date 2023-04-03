#include "seal/seal.h"
#include <vector>
#include <map>
#include <cstdint>

seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(8192);
params.set_plain_modulus(seal::PlainModulus::Batching(8192,18));
params.set_coeff_modulus(seal::CoeffModulus::Create(8192,{60, 60}));
seal::SEALContext context(params,false,seal::sec_level_type::tc128);
return context;
}

std::vector<int> get_rotations_steps(){
std::vector<int> steps = {1,33,993,1022,1023};
return steps; 
}

void gx_kernel(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
seal::Ciphertext ciphertext35;
evaluator.rotate_rows(ciphertext0,1022, galois_keys,ciphertext35);
seal::Ciphertext ciphertext36;
evaluator.sub(ciphertext35,ciphertext0,ciphertext36);
seal::Ciphertext ciphertext13;
evaluator.rotate_rows(ciphertext36,33, galois_keys,ciphertext13);
seal::Ciphertext ciphertext8;
evaluator.rotate_rows(ciphertext0,1, galois_keys,ciphertext8);
std::vector<std::int64_t> scalar1_clear(8192,2);
seal::Plaintext scalar1;
encoder.encode(scalar1_clear,scalar1);
seal::Ciphertext ciphertext9;
evaluator.multiply_plain(ciphertext8,scalar1,ciphertext9);
seal::Ciphertext ciphertext6;
evaluator.rotate_rows(ciphertext0,1023, galois_keys,ciphertext6);
seal::Ciphertext ciphertext7;
evaluator.multiply_plain(ciphertext6,scalar1,ciphertext7);
seal::Ciphertext ciphertext10;
evaluator.sub(ciphertext7,ciphertext9,ciphertext10);
seal::Ciphertext ciphertext5;
evaluator.rotate_rows(ciphertext36,993, galois_keys,ciphertext5);
seal::Ciphertext ciphertext14;
evaluator.add(ciphertext5,ciphertext10,ciphertext14);
seal::Ciphertext ciphertext15;
evaluator.add(ciphertext14,ciphertext13,ciphertext15);
encrypted_outputs.insert({"result",ciphertext15});
}
