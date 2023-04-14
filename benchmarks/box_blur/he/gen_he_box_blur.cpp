#include"gen_he_box_blur.hpp"
seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(4096);
params.set_plain_modulus(seal::PlainModulus::Batching(4096,17));
params.set_coeff_modulus(seal::CoeffModulus::Create(4096,{54, 55}));
seal::SEALContext context(params,false,seal::sec_level_type::tc128);
return context;
}
std::vector<int32_t> get_rotations_steps(){
std::vector<int32_t> steps = {1,32,992,1023};
return steps; 
}
void Computation::box_blur(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs){
seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
seal::Ciphertext ciphertext39;
evaluator.rotate_rows(ciphertext0,1, galois_keys,ciphertext39);
seal::Ciphertext ciphertext34;
evaluator.rotate_rows(ciphertext0,1023, galois_keys,ciphertext34);
seal::Ciphertext ciphertext35;
evaluator.add(ciphertext34,ciphertext0,ciphertext35);
seal::Ciphertext ciphertext40;
evaluator.add(ciphertext35,ciphertext39,ciphertext40);
seal::Ciphertext ciphertext14;
evaluator.rotate_rows(ciphertext40,32, galois_keys,ciphertext14);
seal::Ciphertext ciphertext6;
evaluator.rotate_rows(ciphertext40,992, galois_keys,ciphertext6);
seal::Ciphertext ciphertext15;
evaluator.add(ciphertext6,ciphertext40,ciphertext15);
seal::Ciphertext ciphertext16;
evaluator.add(ciphertext15,ciphertext14,ciphertext16);
encrypted_outputs.insert({"result",ciphertext16});
}