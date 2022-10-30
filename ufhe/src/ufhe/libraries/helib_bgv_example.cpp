#include <helib/helib.h>
#include <iostream>

int main(int argc, char *argv[])
{
  /*  Example of BGV scheme  */

  // Plaintext prime modulus
  unsigned long p = 4999;
  // Cyclotomic polynomial - defines phi(m)
  unsigned long m = 32109;
  // Hensel lifting (default = 1)
  unsigned long r = 1;
  // Number of bits of the modulus chain
  unsigned long bits = 500;
  // Number of columns of Key-Switching matrix (default = 2 or 3)
  unsigned long c = 2;

  // Initialize context
  // This object will hold information about the algebra created from the
  // previously set parameters
  helib::Context context = helib::ContextBuilder<helib::BGV>().m(m).p(p).r(r).bits(bits).c(c).build();

  // Create a secret key associated with the context
  helib::SecKey secret_key(context);
  // Generate the secret key
  secret_key.GenSecKey();
  // Compute key-switching matrices that we need
  helib::addSome1DMatrices(secret_key);

  // Public key management
  // Set the secret key (upcast: SecKey is a subclass of PubKey)
  const helib::PubKey &public_key = secret_key;

  // Get the EncryptedArray of the context
  const helib::EncryptedArray &ea = context.getEA();

  // Get the number of slot (phi(m))
  long nslots = ea.size();

  // Create a vector of long with nslots elements
  helib::Ptxt<helib::BGV> ptxt(context);
  // Set it with numbers 0..nslots - 1
  // ptxt = [0] [1] [2] ... [nslots-2] [nslots-1]
  for (int i = 0; i < ptxt.size(); ++i)
  {
    ptxt[i] = i;
  }

  // Create a ciphertext object
  helib::Ctxt ctxt(public_key);
  // Encrypt the plaintext using the public_key
  public_key.Encrypt(ctxt, ptxt);

  /********** Operations **********/
  // Ciphertext and plaintext operations are performed
  // "entry-wise".

  // Square the ciphertext
  ctxt.multiplyBy(ctxt);
  // Plaintext version
  ptxt.multiplyBy(ptxt);

  // Create a plaintext for decryption
  helib::Ptxt<helib::BGV> plaintext_result(context);
  // Decrypt the modified ciphertext
  secret_key.Decrypt(plaintext_result, ctxt);

  // We can also add constants
  ctxt.addConstant(NTL::ZZX(1l));
  // Plaintext version
  ptxt.addConstant(NTL::ZZX(1l));

  // And multiply by constants
  ctxt *= 1l;
  // Plaintext version
  ptxt *= 1l;

  // We can also perform ciphertext-plaintext operations
  // Note: the output of this is also a ciphertext
  ctxt += ptxt;

  // Decrypt the modified ciphertext into a new plaintext
  helib::Ptxt<helib::BGV> new_plaintext_result(context);
  secret_key.Decrypt(new_plaintext_result, ctxt);

  return 0;
}
