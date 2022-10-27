#pragma once

#include <seal/seal.h>
#include <string>
#include <unordered_map>
#include <variant>

class Outputs
{
private:
  std::unordered_map<std::string, std::variant<seal::Ciphertext, seal::Plaintext>> container;

public:
  template <typename T>
  void insert(const std::pair<std::string, T> &entry)
  {
    container.insert(entry);
  }
  seal::Ciphertext get_ciphertext(const std::string &key)
  {
    auto it = container.find(key);
    if (it == container.end())
      throw("no input with the given label\n");

    if (auto cipher = std::get_if<seal::Ciphertext>(&(container[key])))
    {
      return *cipher;
    }
    else
      throw("no ciphertext with the given label\n");
  }

  seal::Plaintext get_plaintext(const std::string &key)
  {
    auto it = container.find(key);
    if (it == container.end())
      throw("no input with the given label\n");

    if (auto plain = std::get_if<seal::Plaintext>(&(container[key])))
    {
      return *plain;
    }
    else
      throw("no plaintext with the given label\n");
  }
};

class Inputs
{
private:
  std::unordered_map<std::string, std::variant<seal::Ciphertext, seal::Plaintext>> container;

public:
  template <typename T>
  void insert(const std::pair<std::string, T> &entry)
  {
    container.insert(entry);
  }
  seal::Ciphertext get_ciphertext(const std::string &key)
  {
    auto it = container.find(key);
    if (it == container.end())
      throw("no input with the given label\n");

    if (auto cipher = std::get_if<seal::Ciphertext>(&(container[key])))
    {
      return *cipher;
    }
    else
      throw("no ciphertext with the given label\n");
  }

  seal::Plaintext get_plaintext(const std::string &key)
  {
    auto it = container.find(key);
    if (it == container.end())
      throw("no input with the given label\n");

    if (auto plain = std::get_if<seal::Plaintext>(&(container[key])))
    {
      return *plain;
    }
    else
      throw("no plaintext with the given label\n");
  }
};
