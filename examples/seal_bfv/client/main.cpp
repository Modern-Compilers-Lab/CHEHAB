#include "compiler_output/inputs_setup.hpp"
#include "compiler_output/outputs_setup.hpp"
#include "seal/seal.h"
#include "some_def.h"
/*
Scalar class
EncodedInputsMap = map<string, variant<Ciphertext, Plaintext, Scalar>>
EncodedOutputsMap = map<string, variant<Ciphertext, Plaintext, Scalar>>
ClearInputs = map<string, variant<vector<Scalar>, Scalar>>
ClearOutputs = map<string, variant<vector<Scalar>, Scalar>>
*/
using namespace std;

int main() {
    // load selected encryption parameters or use custom parameters
    EncryptionParameters parms;
    parms.load(stream);
    SEALContext context(parms);
    // load existing keys or generate new secret key and other keys
    PublicKey pk;
    pk.load(context, stream);
    RelinKeys relin_keys;
    relin_keys.load(context, stream);
    GaloisKeys galois_keys;
    galois_keys.load(context, stream);
    // get clear data
    vector<Scalar> x = get_data();
    Scalar shift = get_data();
    // call the server to perform homomorphic computations
    EncodedOutputsMap encoded_results =
        call_server(context, relin_keys, galois_keys,
                    inputs_setup(context, pk, {{"x" : x}, {"shift" : shift}}));
    // load existing secret key or use the generated key (above)
    SecretKey sk;
    ClearOutputs clear_results = ouputs_setup(context, sk, encoded_results);
    // show clear output
    return 0;
}
