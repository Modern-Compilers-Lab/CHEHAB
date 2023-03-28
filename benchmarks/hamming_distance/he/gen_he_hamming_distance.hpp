#include "seal/seal.h"
#include <vector>
#include <map>
#include <cstdint>

seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(8192);
params.set_plain_modulus(seal::PlainModulus::Batching(8192,17));
params.set_coeff_modulus(seal::CoeffModulus::Create(8192,{55, 54, 54, 55}));
seal::SEALContext context(params,true,seal::sec_level_type::tc128);
return context;
}

std::vector<int> get_rotations_steps(){
std::vector<int> steps = {1,2,4,8,16,32,64,128,256,512};
return steps; 
}

void hamming_distance(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
std::vector<std::int64_t> scalar0_clear(8192,2);
seal::Plaintext scalar0;
encoder.encode(scalar0_clear,scalar0);
seal::Ciphertext ciphertext1 = encrypted_inputs["v2"];
seal::Ciphertext ciphertext0 = encrypted_inputs["v1"];
seal::Ciphertext ciphertext39;
evaluator.multiply(ciphertext0,ciphertext1,ciphertext39);
seal::Ciphertext ciphertext3;
evaluator.relinearize(ciphertext39,relin_keys,ciphertext3);
seal::Ciphertext ciphertext4;
evaluator.multiply_plain(ciphertext3,scalar0,ciphertext4);
seal::Ciphertext ciphertext2;
evaluator.add(ciphertext0,ciphertext1,ciphertext2);
seal::Ciphertext ciphertext5;
evaluator.sub(ciphertext2,ciphertext4,ciphertext5);
seal::Ciphertext ciphertext5_level1;
evaluator.mod_switch_to_next(ciphertext5,ciphertext5_level1);
seal::Ciphertext ciphertext7;
evaluator.rotate_rows(ciphertext5,512, galois_keys,ciphertext7);
seal::Ciphertext ciphertext7_level1;
evaluator.mod_switch_to_next(ciphertext7,ciphertext7_level1);
seal::Ciphertext ciphertext8;
evaluator.add(ciphertext7_level1,ciphertext5_level1,ciphertext8);
seal::Ciphertext ciphertext9;
evaluator.rotate_rows(ciphertext8,256, galois_keys,ciphertext9);
seal::Ciphertext ciphertext10;
evaluator.add(ciphertext8,ciphertext9,ciphertext10);
seal::Ciphertext ciphertext11;
evaluator.rotate_rows(ciphertext10,128, galois_keys,ciphertext11);
seal::Ciphertext ciphertext12;
evaluator.add(ciphertext10,ciphertext11,ciphertext12);
seal::Ciphertext ciphertext13;
evaluator.rotate_rows(ciphertext12,64, galois_keys,ciphertext13);
seal::Ciphertext ciphertext14;
evaluator.add(ciphertext12,ciphertext13,ciphertext14);
seal::Ciphertext ciphertext15;
evaluator.rotate_rows(ciphertext14,32, galois_keys,ciphertext15);
seal::Ciphertext ciphertext16;
evaluator.add(ciphertext14,ciphertext15,ciphertext16);
seal::Ciphertext ciphertext17;
evaluator.rotate_rows(ciphertext16,16, galois_keys,ciphertext17);
seal::Ciphertext ciphertext18;
evaluator.add(ciphertext16,ciphertext17,ciphertext18);
seal::Ciphertext ciphertext19;
evaluator.rotate_rows(ciphertext18,8, galois_keys,ciphertext19);
seal::Ciphertext ciphertext20;
evaluator.add(ciphertext18,ciphertext19,ciphertext20);
seal::Ciphertext ciphertext21;
evaluator.rotate_rows(ciphertext20,4, galois_keys,ciphertext21);
seal::Ciphertext ciphertext22;
evaluator.add(ciphertext20,ciphertext21,ciphertext22);
seal::Ciphertext ciphertext23;
evaluator.rotate_rows(ciphertext22,2, galois_keys,ciphertext23);
seal::Ciphertext ciphertext24;
evaluator.add(ciphertext22,ciphertext23,ciphertext24);
seal::Ciphertext ciphertext24_level0;
evaluator.mod_switch_to_next(ciphertext24,ciphertext24_level0);
seal::Ciphertext ciphertext25;
evaluator.rotate_rows(ciphertext24,1, galois_keys,ciphertext25);
seal::Ciphertext ciphertext25_level0;
evaluator.mod_switch_to_next(ciphertext25,ciphertext25_level0);
seal::Ciphertext ciphertext26;
evaluator.add(ciphertext25_level0,ciphertext24_level0,ciphertext26);
seal::Ciphertext ciphertext27 = ciphertext26;
encrypted_outputs.insert({"result",ciphertext27});
}
