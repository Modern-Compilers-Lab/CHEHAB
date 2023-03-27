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
std::vector<int> steps = {1,32,992,1023};
return steps; 
}

void box_blur(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::Evaluator& evaluator, const seal::BatchEncoder& encoder, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
seal::Ciphertext ciphertext2;
evaluator.rotate_rows(ciphertext0,32, galois_keys,ciphertext2);
seal::Ciphertext ciphertext13;
evaluator.rotate_rows(ciphertext2,1, galois_keys,ciphertext13);
seal::Ciphertext ciphertext11;
evaluator.rotate_rows(ciphertext2,1023, galois_keys,ciphertext11);
seal::Ciphertext ciphertext12;
evaluator.add(ciphertext2,ciphertext11,ciphertext12);
seal::Ciphertext ciphertext14;
evaluator.add(ciphertext12,ciphertext13,ciphertext14);
seal::Ciphertext ciphertext9;
evaluator.rotate_rows(ciphertext0,1, galois_keys,ciphertext9);
seal::Ciphertext ciphertext7;
evaluator.rotate_rows(ciphertext0,1023, galois_keys,ciphertext7);
seal::Ciphertext ciphertext8;
evaluator.add(ciphertext0,ciphertext7,ciphertext8);
seal::Ciphertext ciphertext10;
evaluator.add(ciphertext8,ciphertext9,ciphertext10);
seal::Ciphertext ciphertext1;
evaluator.rotate_rows(ciphertext0,992, galois_keys,ciphertext1);
seal::Ciphertext ciphertext5;
evaluator.rotate_rows(ciphertext1,1, galois_keys,ciphertext5);
seal::Ciphertext ciphertext3;
evaluator.rotate_rows(ciphertext1,1023, galois_keys,ciphertext3);
seal::Ciphertext ciphertext4;
evaluator.add(ciphertext1,ciphertext3,ciphertext4);
seal::Ciphertext ciphertext6;
evaluator.add(ciphertext4,ciphertext5,ciphertext6);
seal::Ciphertext ciphertext16;
evaluator.add(ciphertext6,ciphertext10,ciphertext16);
seal::Ciphertext ciphertext17;
evaluator.add(ciphertext16,ciphertext14,ciphertext17);
seal::Ciphertext ciphertext18 = ciphertext17;
encrypted_outputs.insert({"result",ciphertext18});
}
