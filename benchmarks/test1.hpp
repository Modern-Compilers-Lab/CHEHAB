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
void test1(std::unordered_map<std::string, ufhe::Ciphertext>& encrypted_inputs, std::unordered_map<std::string, ufhe::Plaintext>& encoded_inputs, std::unordered_map<std::string, ufhe::Ciphertext>& encrypted_outputs, std::unordered_map<std::string, ufhe::Plaintext>& encoded_outputs, const ufhe::EncryptionContext& context, const ufhe::RelinKeys& relin_keys, const ufhe::GaloisKeys& galois_keys, const ufhe::PublicKey& public_key)
{
ufhe::Ciphertext B = encrypted_inputs["B"];
ufhe::Evaluator evaluator(context);
ufhe::Ciphertext ciphertext7;
evaluator.rotate_rows(B,3, galois_keys,ciphertext7);
ufhe::Ciphertext A = encrypted_inputs["A"];
ufhe::Ciphertext ciphertext6;
evaluator.rotate_rows(A,3, galois_keys,ciphertext6);
evaluator.add_inplace(ciphertext6,ciphertext7);ufhe::Ciphertext ciphertext4;
evaluator.rotate_rows(B,5, galois_keys,ciphertext4);
ufhe::Ciphertext ciphertext3;
evaluator.rotate_rows(A,3, galois_keys,ciphertext3);
evaluator.multiply_inplace(ciphertext3,ciphertext4);evaluator.relinearize_inplace(ciphertext3,relin_keys);evaluator.add_inplace(ciphertext3,ciphertext6);ufhe::Ciphertext output1 = ciphertext3;
encrypted_outputs.insert({"output1",output1});
}
