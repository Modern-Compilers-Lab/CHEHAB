#include"seal/seal.h"
#include<vector>
#include<unordered_map>
#include<cstdint>

seal::SEALContext create_context();
std::vector<int32_t> get_rotations_steps();

class Computation{private:
seal::GaloisKeys galois_keys;
seal::RelinKeys relin_keys;
seal::Evaluator evaluator;
seal::BatchEncoder encoder;
seal::Encryptor encryptor;
std::map<std::string, seal::Ciphertext> static_ciphertexts;
std::map<std::string, seal::Plaintext> static_plaintexts;
public: 
std::map<std::string, seal::Ciphertext> encrypted_outputs;
std::map<std::string, seal::Plaintext> encoded_outputs;
void box_blur(std::map<std::string, seal::Ciphertext>& encrypted_inputs, std::map<std::string, seal::Plaintext>& encoded_inputs);
Computation(const seal::RelinKeys& relin_keys, const seal::GaloisKeys& galois_keys, const seal::SEALContext& context, const seal::PublicKey& public_key): relin_keys(relin_keys), encoder(context),galois_keys(galois_keys),evaluator(context),encryptor(context,public_key){} 
}; 
