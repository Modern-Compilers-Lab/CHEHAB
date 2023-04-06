#include "seal/seal.h"
#include <vector>
#include <map>
#include <cstdint>

seal::SEALContext create_context(){
seal::EncryptionParameters params(seal::scheme_type::bfv);
params.set_poly_modulus_degree(8192);
params.set_plain_modulus(seal::PlainModulus::Batching(8192,18));
params.set_coeff_modulus(seal::CoeffModulus::Create(8192,{60, 60, 60}));
seal::SEALContext context(params,true,seal::sec_level_type::tc128);
return context;
}

void poly_reg(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs, std::map<std::string, seal::Ciphertext>& encrypted_outputs, std::map<std::string, seal::Plaintext>& encoded_outputs, const seal::BatchEncoder& encoder, const seal::Encryptor& encryptor, const seal::Evaluator& evaluator, const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys)
{
seal::Ciphertext ciphertext2 = encrypted_inputs["c2"];
seal::Ciphertext ciphertext2_level0;
evaluator.mod_switch_to_next(ciphertext2,ciphertext2_level0);
seal::Ciphertext ciphertext3 = encrypted_inputs["c3"];
seal::Ciphertext ciphertext4 = encrypted_inputs["c4"];
seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
seal::Ciphertext ciphertext11;
evaluator.multiply(ciphertext0,ciphertext4,ciphertext11);
seal::Ciphertext ciphertext5;
evaluator.relinearize(ciphertext11,relin_keys,ciphertext5);
seal::Ciphertext ciphertext6;
evaluator.add(ciphertext5,ciphertext3,ciphertext6);
seal::Ciphertext ciphertext13;
evaluator.multiply(ciphertext0,ciphertext6,ciphertext13);
seal::Ciphertext ciphertext13_level0;
evaluator.mod_switch_to_next(ciphertext13,ciphertext13_level0);
seal::Ciphertext ciphertext7;
evaluator.relinearize(ciphertext13_level0,relin_keys,ciphertext7);
seal::Ciphertext ciphertext8;
evaluator.add(ciphertext7,ciphertext2_level0,ciphertext8);
seal::Ciphertext ciphertext1 = encrypted_inputs["c1"];
seal::Ciphertext ciphertext1_level0;
evaluator.mod_switch_to_next(ciphertext1,ciphertext1_level0);
seal::Ciphertext ciphertext9;
evaluator.sub(ciphertext1_level0,ciphertext8,ciphertext9);
encrypted_outputs.insert({"c_result",ciphertext9});
}
