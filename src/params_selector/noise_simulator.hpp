#pragma once

#include "fhecompiler_const.hpp"
#include "program.hpp"
#include <NTL/RR.h>
#include <NTL/quad_float.h>
#include <NTL/xdouble.h>

namespace noise_simulator
{

class BGVSimulator
{
private:
  /*
    X's standard deviation sigma , X is used to sample random polynomials that serves as error in ecnryption and
    generating keys, X is a discrete gaussian distribution
  */

  NTL::RR standard_deviation; /* 3.2 is the value recommended by HE standard, sometimes it is 3.19 */

  const double sigma_value = 3.2;

  /*
    This simulator is based on bounds that are defined on canonical embedding of noise polynomial v, and the canonical
    embedding is bounded by variance of v, main idea is based on the inequality : canonical_norm(v) <= 6 * sqrt(n *
    V(v)) where V(v) is the variance of v. Distributions used in sampling random polynomials in schemes are usually well
    known ones with well defined parameters (mean, standard deviation, and standard deviation bound) namely gaussian and
    uniform. this method is heuristic and it underestimates noise budget (for more details
    https://eprint.iacr.org/2019/493.pdf). In the paper refered previously they don't provide a bound for add_plain and
    mul_plain but we can follow the same pattern in order to get bounds on noise growth for these operations. All bounds
    are valid with a high probability
  */

  std::unordered_map<std::string, NTL::RR>
    noise_norm_bound; // noise_bound to store noise bound for each ciphertext defined by a label
  std::unordered_map<std::string, NTL::RR>
    noise_budget; // noise_budget to store noise budget for each ciphertext defined by a label

  // dimension n
  NTL::RR n;

  // plaintext modulus t;
  NTL::RR t;

  // coef modulus q
  NTL::RR q; // a big number

  // logq
  NTL::RR log_q;

  size_t estimated_required_noise_budget = 0; // estimation

  NTL::RR log_base2(const NTL::RR &a)
  {
    NTL::RR two(2);
    return NTL::log(a) / NTL::log(two);
  }

public:
  BGVSimulator(double _n, double _t, double _log_q) : n(_n), t(_t), log_q(_log_q), standard_deviation(sigma_value)
  {
    std::cout << "n = " << n << " "
              << "t = " << t << " "
              << "logQ = " << log_q << "\n";
  } // log_q = log2(q); }

  NTL::RR encrypt()
  {
    standard_deviation = 3 + 0.2;
    NTL::RR a = (n / 12) + n * (NTL::power(standard_deviation, 2)) * ((4 / 3) * n + 1);
    return 6 * t * NTL::sqrt(a);
  }

  NTL::RR plain_bound()
  {
    standard_deviation = 3 + 0.2;
    NTL::RR a = (n / 12) + n * (NTL::power(standard_deviation, 2)) * ((2 / 3) * n);
    return 6 * t * NTL::sqrt(a);
  }

  NTL::RR estimate_noise_budget(NTL::RR norm_bound) { return log_q - log_base2(norm_bound) - 1; }

  NTL::RR multiply(const std::string &ct1, const std::string &ct2)
  {
    return noise_norm_bound[ct1] * noise_norm_bound[ct2];
  }

  NTL::RR multiply_plain(const std::string &ct) { return noise_norm_bound[ct] * plain_bound(); }

  NTL::RR add(const std::string &ct1, const std::string &ct2) { return noise_norm_bound[ct1] + noise_norm_bound[ct2]; }

  NTL::RR add_plain(const std::string &ct) { return noise_norm_bound[ct] + plain_bound(); }

  NTL::RR mod_switch(const std::string &ct) { return static_cast<NTL::RR>(0.0); }

  NTL::RR relinearize(const std::string &ct) { return static_cast<NTL::RR>(0.0); }

  NTL::RR sub(const std::string &ct1, const std::string &ct2) { return add(ct1, ct2); }

  NTL::RR sub_plain(const std::string &ct, const std::string &pt) { return add_plain(ct); }

  void simulate_noise_growth(ir::Program *program)
  {

    using Ptr = std::shared_ptr<ir::Term>;
    const std::vector<Ptr> &sorted_nodes = program->get_dataflow_sorted_nodes();
    for (auto &node_ptr : sorted_nodes)
    {
      auto operands_opt = node_ptr->get_operands();
      if (operands_opt != std::nullopt)
      {
        auto &operands = *operands_opt;
        if (operands.size() == 1)
        {
          if (node_ptr->get_opcode() == ir::OpCode::assign)
          {
            noise_budget[node_ptr->get_label()] = noise_budget[operands[0]->get_label()];
            // list_pointer = list_pointer->get_next();
            noise_norm_bound[node_ptr->get_label()] = noise_norm_bound[operands[0]->get_label()];
          }
          else if (node_ptr->get_opcode() == ir::OpCode::encrypt)
          {

            noise_norm_bound[node_ptr->get_label()] = encrypt();
            noise_budget[node_ptr->get_label()] = estimate_noise_budget(noise_norm_bound[node_ptr->get_label()]);
            std::cout << "freshly encrypted noise budget : " << noise_budget[node_ptr->get_label()] << '\n';
            std::cout << "freshly encryptd noise : " << log_base2(noise_norm_bound[node_ptr->get_label()]) << "\n";
            std::cout << "--------------------------------------------------------\n";
          }
          continue;
        }
        else if (operands.size() == 2)
        {

          auto &lhs = operands[0];
          auto &rhs = operands[1];

          ir::OpCode opcode = node_ptr->get_opcode();

          if (opcode == ir::OpCode::add)
          {
            noise_norm_bound[node_ptr->get_label()] = add(lhs->get_label(), rhs->get_label());

            noise_budget[node_ptr->get_label()] = estimate_noise_budget(noise_norm_bound[node_ptr->get_label()]);
            std::cout << "noise budget of" << node_ptr->get_label()
                      << " left after add : " << noise_budget[node_ptr->get_label()] << "\n";
            std::cout << "-------------------------------------------------\n";
          }
          else if (opcode == ir::OpCode::add_plain)
          {
            noise_norm_bound[node_ptr->get_label()] = add_plain(lhs->get_label());

            noise_budget[node_ptr->get_label()] = estimate_noise_budget(noise_norm_bound[node_ptr->get_label()]);
            std::cout << "noise budget of" << node_ptr->get_label()
                      << " left after add_plain : " << noise_budget[node_ptr->get_label()] << "\n";
            std::cout << "-------------------------------------------------\n";
          }
          else if (opcode == ir::OpCode::mul)
          {
            noise_norm_bound[node_ptr->get_label()] = multiply(lhs->get_label(), rhs->get_label());
            std::cout << log_base2(noise_norm_bound[node_ptr->get_label()]) << "\n";

            noise_budget[node_ptr->get_label()] = estimate_noise_budget(noise_norm_bound[node_ptr->get_label()]);
            std::cout << "noise budget of" << node_ptr->get_label()
                      << " left after mul : " << noise_budget[node_ptr->get_label()] << "\n";
            std::cout << "-------------------------------------------------\n";
          }
          else if (opcode == ir::OpCode::mul_plain)
          {
            noise_norm_bound[node_ptr->get_label()] = multiply_plain(lhs->get_label());
            std::cout << log_base2(noise_norm_bound[node_ptr->get_label()]) << "\n";

            noise_budget[node_ptr->get_label()] = estimate_noise_budget(noise_norm_bound[node_ptr->get_label()]);
            std::cout << "noise budget left after mul_plain : " << noise_budget[node_ptr->get_label()] << "\n";
            std::cout << "-------------------------------------------------\n";
          }
          /* there bounds for relinearize and modulus switching as well */
        }
        else
          throw("yet to treat case \n");
      }
      else
      {
        if (node_ptr->get_term_type() == ir::ciphertextType)
        {
          noise_norm_bound[node_ptr->get_label()] = encrypt();
          noise_budget[node_ptr->get_label()] = estimate_noise_budget(noise_norm_bound[node_ptr->get_label()]);
          std::cout << "freshly encrypted noise budget : " << noise_budget[node_ptr->get_label()] << '\n';
          std::cout << "freshly encryptd noise : " << log_base2(noise_norm_bound[node_ptr->get_label()]) << "\n";
          std::cout << "--------------------------------------------------------\n";
        }
      }
    }
  }
};

} // namespace noise_simulator
;
