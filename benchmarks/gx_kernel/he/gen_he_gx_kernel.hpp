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
std::vector<int> steps = {1,33,993,1023,1055,2015};
return steps; 
}

void gx_kernel(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
seal::Ciphertext ciphertext14;
evaluator.rotate_rows(ciphertext0,33, galois_keys,ciphertext14);
seal::Ciphertext ciphertext12;
evaluator.rotate_rows(ciphertext0,1055, galois_keys,ciphertext12);
seal::Ciphertext ciphertext13;
evaluator.negate(ciphertext12,ciphertext13);
seal::Ciphertext ciphertext15;
evaluator.add(ciphertext13,ciphertext14,ciphertext15);
seal::Ciphertext ciphertext9;
evaluator.rotate_rows(ciphertext0,1, galois_keys,ciphertext9);
std::vector<std::int64_t> scalar1_clear(8192,2);
seal::Plaintext scalar1;
encoder.encode(scalar1_clear,scalar1);
seal::Ciphertext ciphertext10;
evaluator.multiply_plain(ciphertext9,scalar1,ciphertext10);
seal::Ciphertext ciphertext7;
evaluator.rotate_rows(ciphertext0,1023, galois_keys,ciphertext7);
std::vector<std::int64_t> scalar0_clear(8192,-2);
seal::Plaintext scalar0;
encoder.encode(scalar0_clear,scalar0);
seal::Ciphertext ciphertext8;
evaluator.multiply_plain(ciphertext7,scalar0,ciphertext8);
seal::Ciphertext ciphertext11;
evaluator.add(ciphertext8,ciphertext10,ciphertext11);
seal::Ciphertext ciphertext5;
evaluator.rotate_rows(ciphertext0,993, galois_keys,ciphertext5);
seal::Ciphertext ciphertext3;
evaluator.rotate_rows(ciphertext0,2015, galois_keys,ciphertext3);
seal::Ciphertext ciphertext4;
evaluator.negate(ciphertext3,ciphertext4);
seal::Ciphertext ciphertext6;
evaluator.add(ciphertext4,ciphertext5,ciphertext6);
seal::Ciphertext ciphertext16;
evaluator.add(ciphertext6,ciphertext11,ciphertext16);
seal::Ciphertext ciphertext17;
evaluator.add(ciphertext16,ciphertext15,ciphertext17);
encrypted_outputs.insert({"result",ciphertext17});
}
