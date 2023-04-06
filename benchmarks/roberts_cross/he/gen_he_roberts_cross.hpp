#include "seal/seal.h"
#include <vector>
#include <map>
#include <cstdint>

seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(8192);
params.set_plain_modulus(seal::PlainModulus::Batching(8192,22));
params.set_coeff_modulus(seal::CoeffModulus::Create(8192,{60, 60, 60}));
seal::SEALContext context(params,false,seal::sec_level_type::tc128);
return context;
}

std::vector<int> get_rotations_steps(){
std::vector<int> steps = {1,31,33};
return steps; 
}

void roberts_cross(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::BatchEncoder& encoder, const seal::Encryptor& encryptor, const seal::Evaluator& evaluator, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys)
{
seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
seal::Ciphertext ciphertext15;
evaluator.rotate_rows(ciphertext0,31, galois_keys,ciphertext15);
seal::Ciphertext ciphertext16;
evaluator.sub(ciphertext0,ciphertext15,ciphertext16);
seal::Ciphertext ciphertext23;
evaluator.multiply(ciphertext16,ciphertext16,ciphertext23);
seal::Ciphertext ciphertext18;
evaluator.relinearize(ciphertext23,relin_keys,ciphertext18);
seal::Ciphertext ciphertext7;
evaluator.rotate_rows(ciphertext18,1, galois_keys,ciphertext7);
seal::Ciphertext ciphertext2;
evaluator.rotate_rows(ciphertext0,33, galois_keys,ciphertext2);
seal::Ciphertext ciphertext3;
evaluator.sub(ciphertext0,ciphertext2,ciphertext3);
seal::Ciphertext ciphertext25;
evaluator.multiply(ciphertext3,ciphertext3,ciphertext25);
seal::Ciphertext ciphertext6;
evaluator.relinearize(ciphertext25,relin_keys,ciphertext6);
seal::Ciphertext ciphertext8;
evaluator.add(ciphertext6,ciphertext7,ciphertext8);
encrypted_outputs.insert({"result",ciphertext8});
}
