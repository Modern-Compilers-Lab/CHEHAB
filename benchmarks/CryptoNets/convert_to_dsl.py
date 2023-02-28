import sys
from enum import Enum


class VarRole(Enum):
    INPUT = "input"
    TEMP = "temp"
    OUTPUT = "output"


class VarType(Enum):
    CIPHER = 1
    SCALAR = 2


def transpile(src_fname, dest_fname):
    with open(src_fname) as src_file, open(dest_fname, "w") as dest_file:
        write_startin_boilerplate_code(src_fname.split(".")[0], dest_file)
        cipher_vars = set()
        scalar_vars = set()
        line_number = 0
        for line in src_file:
            line_number += 1
            inst = line.strip().split(",")
            inst.extend(["", "", ""])
            full_inst = inst[:4]
            result, arg1, arg2, op = full_inst
            # determine result variable type
            result_role = VarRole.TEMP
            if result[0] == "#":
                result_role = VarRole.INPUT
                result = result[1:]
            elif result[0] == "$":
                result_role = VarRole.OUTPUT
                result = result[1:]

            def check_inst():
                if op == "":
                    if arg1 == "":
                        if result in cipher_vars or result in scalar_vars:
                            if result_role == VarRole.OUTPUT:
                                return
                            raise Exception(
                                f"line {line_number}: undefined instruction"
                            )
                        raise Exception(
                            f"line {line_number}: declaration without initialization, cannot infer type"
                        )
                    if (
                        arg1 in cipher_vars
                        or arg1 in scalar_vars
                        or represents_int(arg1)
                    ):
                        return
                    raise Exception(
                        f"line {line_number}: referencing undeclared variable '{arg1}'"
                    )
                if arg1 == "":
                    raise Exception(
                        f"line {line_number}: binary operation without first argument"
                    )
                if arg2 == "":
                    raise Exception(
                        f"line {line_number}: binary operation without second argument"
                    )
                if arg1 in cipher_vars or arg1 in scalar_vars or represents_int(arg1):
                    if (
                        arg2 in cipher_vars
                        or arg2 in scalar_vars
                        or represents_int(arg2)
                    ):
                        return
                    raise Exception(
                        f"line {line_number}: referencing undeclared variable '{arg2}'"
                    )
                raise Exception(
                    f"line {line_number}: referencing undeclared variable '{arg1}'"
                )

            check_inst()

            if result not in cipher_vars and result not in scalar_vars:
                # Declaration
                def infer_result_type():
                    if result_role == VarRole.INPUT or result_role == VarRole.OUTPUT:
                        return VarType.CIPHER
                    if arg1 in cipher_vars:
                        return VarType.CIPHER
                    if arg1 in scalar_vars or represents_int(arg1):
                        if op == "":
                            return VarType.SCALAR
                        if arg2 in cipher_vars:
                            return VarType.CIPHER
                        if arg2 in scalar_vars or represents_int(arg2):
                            return VarType.SCALAR

                result_type = infer_result_type()
                if result_type == VarType.CIPHER:
                    dest_file.write(declare_cipher_var(result, result_role))
                    cipher_vars.add(result)
                if result_type == VarType.SCALAR:
                    dest_file.write(declare_scalar_var(result))
                    scalar_vars.add(result)

            # result variable already declared
            if op == "":
                if result_role == VarRole.INPUT:
                    # raise Exception("input variable modified at declaration")
                    continue
                if arg1 != "":
                    dest_file.write(f"{result}={arg1};\n")
            else:
                if result_role == VarRole.INPUT:
                    raise Exception("input variable modified at declaration")
                dest_file.write(f"{result}={arg1}{op}{arg2};\n")
        write_ending_boilerplate_code(src_fname.split(".")[0] + ".hpp", dest_file)


def write_startin_boilerplate_code(program_name, dest_file):
    dest_file.write(
        f"""#include "fhecompiler/fhecompiler.hpp"
using namespace fhecompiler;
int main()
{{
init("{program_name}", Scheme::bfv);
"""
    )


def write_ending_boilerplate_code(output_file_name, dest_file):
    dest_file.write(
        f"""params_selector::EncryptionParameters params;
size_t polynomial_modulus_degree = 4096;
size_t plaintext_modulus = 786433;
params.set_plaintext_modulus(plaintext_modulus);
params.set_polynomial_modulus_degree(polynomial_modulus_degree);
fhecompiler::compile("{output_file_name}", &params);
}}"""
    )


def declare_cipher_var(id, role):
    return f'Ciphertext {id}("{id}",VarType::{role.value});\n'


def declare_scalar_var(id):
    return f'Scalar {id}("{id}");\n'


def represents_int(s):
    try:
        int(s)
        return True
    except ValueError:
        return False


if __name__ == "__main__":
    assert len(sys.argv) > 1, "srs_fname is required"
    srs_fname = sys.argv[1]
    if len(sys.argv) > 2:
        dest_fname = sys.argv[2]
    else:
        dest_fname = srs_fname.split(".")[0] + ".cpp"
    transpile(srs_fname, dest_fname)
