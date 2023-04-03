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
std::vector<int> steps = {32,33,992,993,1055,2015};
return steps; 
}

void gy_kernel(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
seal::Ciphertext ciphertext11;
evaluator.rotate_rows(ciphertext0,33, galois_keys,ciphertext11);
seal::Ciphertext ciphertext9;
evaluator.rotate_rows(ciphertext0,1055, galois_keys,ciphertext9);
seal::Ciphertext ciphertext2;
evaluator.rotate_rows(ciphertext0,32, galois_keys,ciphertext2);
std::vector<std::int64_t> scalar1_clear(8192,-2);
seal::Plaintext scalar1;
encoder.encode(scalar1_clear,scalar1);
seal::Ciphertext ciphertext8;
evaluator.multiply_plain(ciphertext2,scalar1,ciphertext8);
seal::Ciphertext ciphertext10;
evaluator.sub(ciphertext8,ciphertext9,ciphertext10);
seal::Ciphertext ciphertext12;
evaluator.sub(ciphertext10,ciphertext11,ciphertext12);
seal::Ciphertext ciphertext6;
evaluator.rotate_rows(ciphertext0,993, galois_keys,ciphertext6);
seal::Ciphertext ciphertext4;
evaluator.rotate_rows(ciphertext0,2015, galois_keys,ciphertext4);
seal::Ciphertext ciphertext1;
evaluator.rotate_rows(ciphertext0,992, galois_keys,ciphertext1);
std::vector<std::int64_t> scalar0_clear(8192,2);
seal::Plaintext scalar0;
encoder.encode(scalar0_clear,scalar0);
seal::Ciphertext ciphertext3;
evaluator.multiply_plain(ciphertext1,scalar0,ciphertext3);
seal::Ciphertext ciphertext5;
evaluator.add(ciphertext3,ciphertext4,ciphertext5);
seal::Ciphertext ciphertext7;
evaluator.add(ciphertext5,ciphertext6,ciphertext7);
seal::Ciphertext ciphertext13;
evaluator.add(ciphertext7,ciphertext12,ciphertext13);
encrypted_outputs.insert({"result",ciphertext13});
}
