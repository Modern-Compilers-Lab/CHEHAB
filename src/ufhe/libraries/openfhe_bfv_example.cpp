#include "openfhe.h"

using namespace lbcrypto;

int main()
{
  // Sample Program: Step 1: Set CryptoContext
  CCParams<CryptoContextBFVRNS> parameters;
  parameters.SetPlaintextModulus(65537);
  parameters.SetMultiplicativeDepth(2);

  CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
  // Enable features that you wish to use
  cryptoContext->Enable(PKE);
  cryptoContext->Enable(KEYSWITCH);
  cryptoContext->Enable(LEVELEDSHE);

  // Sample Program: Step 2: Key Generation

  // Initialize Public Key Containers
  KeyPair<DCRTPoly> keyPair;

  // Generate a public/private key pair
  keyPair = cryptoContext->KeyGen();

  // Generate the relinearization key
  cryptoContext->EvalMultKeyGen(keyPair.secretKey);

  // Generate the rotation evaluation keys
  cryptoContext->EvalRotateKeyGen(keyPair.secretKey, {1, -2});

  // Sample Program: Step 3: Encryption

  // First plaintext vector is encoded
  std::vector<int64_t> vector1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  Plaintext plaintext1 = cryptoContext->MakePackedPlaintext(vectorOfInts1);

  // The encoded vectors are encrypted
  auto ciphertext1 = cryptoContext->Encrypt(keyPair.publicKey, plaintext1);

  // Sample Program: Step 4: Evaluation

  // Homomorphic additions
  auto ciphertextAddResult = cryptoContext->EvalAdd(ciphertext1, ciphertext1);

  // Homomorphic multiplications
  auto ciphertextMultResult = cryptoContext->EvalMult(ciphertext1, ciphertext1);

  // Homomorphic rotations
  auto ciphertextRot1 = cryptoContext->EvalRotate(ciphertext1, 1);
  auto ciphertextRot2 = cryptoContext->EvalRotate(ciphertext1, -2);

  // Sample Program: Step 5: Decryption

  // Decrypt the result of additions
  Plaintext plaintextAddResult;
  cryptoContext->Decrypt(keyPair.secretKey, ciphertextAddResult, &plaintextAddResult);

  return 0;
}
