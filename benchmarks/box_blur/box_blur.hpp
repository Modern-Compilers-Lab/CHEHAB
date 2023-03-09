#include"seal/seal.h"
#include<vector>
#include<unordered_map>
seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(1024);
params.set_plain_modulus(seal::PlainModulus::Batching(1024,17));
params.set_coeff_modulus(seal::CoeffModulus::Create(1024,{27, 27}));
seal::SEALContext context(params);
return context;
}

void box_blur(std::unordered_map<std::string, seal::Ciphertext>& encrypted_inputs, std::unordered_map<std::string, seal::Plaintext>& encoded_inputs, std::unordered_map<std::string, seal::Ciphertext>& encrypted_outputs, std::unordered_map<std::string, seal::Plaintext>& encoded_outputs, const seal::SEALContext& context, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::PublicKey& public_key)
{
seal::Ciphertext c0 = encrypted_inputs["c0"];
seal::Evaluator evaluator(context);
seal::Ciphertext ciphertext3;
evaluator.rotate_vector(c0,6, galois_keys,ciphertext3);
seal::Ciphertext ciphertext2;
evaluator.rotate_vector(c0,5, galois_keys,ciphertext2);
evaluator.add_inplace(ciphertext2,ciphertext3);seal::Ciphertext ciphertext1;
evaluator.rotate_vector(c0,1, galois_keys,ciphertext1);
evaluator.add_inplace(ciphertext1,c0);evaluator.add_inplace(ciphertext1,ciphertext2);seal::Ciphertext output = ciphertext1;
encrypted_outputs.insert({"output",output});
}
