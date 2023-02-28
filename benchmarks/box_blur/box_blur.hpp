#include"ufhe/ufhe.hpp"
#include<vector>
#include<unordered_map>
ufhe::EncryptionContext create_context(){
ufhe::EncryptionParams params(ufhe::scheme_type::bfv);
params.set_poly_modulus_degree(4096);
params.set_coeff_modulus(ufhe::CoeffModulus::BFVDefault(4096,ufhe::sec_level_type::tc128));
params.set_plain_modulus(786433);
ufhe::EncryptionContext context(params);
return context;
}

std::vector<int32_t> get_rotations_steps(){
std::vector<int32_t> steps = {5,1}; return steps; }void box_blur(std::unordered_map<std::string, ufhe::Ciphertext>& encrypted_inputs, std::unordered_map<std::string, ufhe::Plaintext>& encoded_inputs, std::unordered_map<std::string, ufhe::Ciphertext>& encrypted_outputs, std::unordered_map<std::string, ufhe::Plaintext>& encoded_outputs, const ufhe::EncryptionContext& context, const ufhe::RelinKeys& relin_keys, const ufhe::GaloisKeys& galois_keys, const ufhe::PublicKey& public_key)
{
ufhe::Ciphertext c0 = encrypted_inputs["c0"];
ufhe::Evaluator evaluator(context);
ufhe::Ciphertext ciphertext17;
evaluator.rotate_rows(c0,1, galois_keys,ciphertext17);
ufhe::Ciphertext ciphertext18;
evaluator.add(ciphertext17,c0,ciphertext18);
evaluator.rotate_rows_inplace(ciphertext18,5, galois_keys);evaluator.add_inplace(ciphertext17,ciphertext18);ufhe::Ciphertext output;
evaluator.add(ciphertext17,c0,output);
encrypted_outputs.insert({"output",output});
}
