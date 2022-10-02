#include <map>
#include <string>
#include <vector>

#include "seal/seal.h"
#include "some_def.h"
/*
Scalar class
EncodedInputsMap = map<string, variant<Ciphertext, Plaintext, Scalar>>
ClearInputs = map<string, variant<vector<Scalar>, Scalar>>
*/

using namespace std;
using namespace seal;

EncodedInputsMap inputs_setup(const SEALContext &context,
                              const PublicKey &public_key,
                              const ClearInputs &inputs) {
    Encryptor encryptor(context, public_key);
    BatchEncoder batch_encoder(context);
    vector<Scalar> &x_clear = inputs["x"];
    Scalar &shift = inputs["shift"];
    Plaintext x_plain;
    batch_encoder.encode(x_clear, plain_matrix);
    Ciphertext x_cipher;
    encryptor.encrypt(x_plain, x_cipher);
    return {{"x", x_cipher}, {"shift", shift}};
}
