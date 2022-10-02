#include <map>
#include <string>
#include <vector>

#include "seal/seal.h"
#include "some_def.h"
/*
Scalar class
EncodedOutputsMap = map<string, variant<Ciphertext, Plaintext, Scalar>>
ClearOutputs = map<string, variant<vector<Scalar>, Scalar>>
*/

using namespace std;
using namespace seal;

ClearOutputs ouputs_setup(const SEALContext &context,
                          const SecretKey &secret_key,
                          const EncodedOutputsMap &outputs) {
    Decryptor decryptor(context, secret_key);
    BatchEncoder batch_encoder(context);
    Ciphertext &y_cipher = outputs["y"];
    Plaintext y_plain;
    decryptor.decrypt(y, y_plain);
    vector<Scalar> y_clear;
    batch_encoder.decode(y_plain, y_clear);
    return {{"y" : y_clear}};
}
