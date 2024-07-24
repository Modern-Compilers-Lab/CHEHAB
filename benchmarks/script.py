from typing import List, Tuple, Dict
from declarations import file_content


def process_vectorized_code(content: str) -> List[str]:
    """
    Parameters:
    filename (str): The path to the file.

    Returns:
    List[str]: The cleaned and split content of the file.
    """

    cleaned_content = content.replace("(", "( ").replace(")", " )").replace("\n", " ")
    return [token for token in cleaned_content.split(" ") if token]


def is_literal(token: str) -> bool:
    """
    Checks if a string represents a literal or a variable.
    A string is considered a literal if it does not contain any letters.

    Parameters:
    token (str): The string to check.

    Returns:
    bool: True if the string is a literal, False if it is a variable.
    """
    return not any(char.isalpha() for char in token)


def process(
    tokens: List[str],
    index: int,
    dictionary: Dict[str, str],
    inputs: List[str],
    inputs_types: List[str],
) -> Tuple[str, int]:
    """
    Processes the tokens recursively and builds the necessary computation and declaration strings.

    Parameters:
    tokens (List[str]): The list of tokens to process.
    index (int): The current index in the tokens list.
    dictionary (Dict[str, str]): A dictionary to store processed vectors.
    inputs (List[str]): The list of input variables.
    inputs_types (List[str]): The list of input variable types.

    Returns:
    Tuple[str, int]: The processed label and the updated index.
    """
    global id_counter, computations, declarations, new_inputs
    while index < len(tokens):
        if tokens[index] == "(":
            index += 1
            if tokens[index] == "Vec":
                vector, nested_level = "Vec ", 0
                index += 1
                while nested_level >= 0:
                    vector += tokens[index] + " "
                    nested_level += (
                        1 if tokens[index] == "(" else -1 if tokens[index] == ")" else 0
                    )
                    index += 1
                vector = vector.strip()[:-2]

                if vector not in dictionary:
                    elements = vector.replace("Vec ", "").split()
                    new_input = ""
                    all_literals = True
                    for element in elements:
                        if not is_literal(element):
                            all_literals = False
                            new_input += "@" + str(inputs.index(element)) + " "
                            if inputs_types[inputs.index(element)] == "1":
                                all_literals = False
                        else:
                            new_input += element + " "

                    if all_literals:
                        label_type = "Plaintext "
                        label = "p" + str(id_counter)
                        new_input = "0 1 " + new_input
                    else:
                        label_type = "Ciphertext "
                        label = "c" + str(id_counter)
                        new_input = "1 1 " + new_input
                    new_inputs += new_input + "\n"
                    id_counter += 1
                    declarations += f'{label_type}{label}("{label}");\n'
                    dictionary[vector] = label
                return dictionary[vector], index
            if len(tokens[index]) > 3:
                operation = tokens[index]
                index += 1
                operand_1, index = process(
                    tokens, index, dictionary, inputs, inputs_types
                )
                if tokens[index] == "(":
                    operand_2, index = process(
                        tokens, index, dictionary, inputs, inputs_types
                    )
                    op = (
                        "+"
                        if operation == "VecAdd"
                        else "-" if operation == "VecMinus" else "*"
                    )
                    label = "c" + str(id_counter)
                    id_counter += 1
                    computations += (
                        f"Ciphertext {label} = {operand_1} {op} {operand_2};\n"
                    )
                    index += 1
                    return label, index
                else:
                    index += 1
                    label = "c" + str(id_counter)
                    id_counter += 1
                    computations += f"Ciphertext {label} = -{operand_1};\n"
                    return label, index
    return "", index


def create_file(filename: str, content: str) -> None:
    """
    Creates a file and writes the content to it.

    Parameters:
    filename (str): The path to the file.
    content (str): The content to write to the file.
    """
    with open(filename, "w") as file:
        file.write(content)


if __name__ == "__main__":
    id_counter = 0
    declarations, computations, new_inputs = "", "", ""
    inputs_file = "../inputs.txt"
    with open(inputs_file) as f:
        inputs, inputs_types = f.readlines()
    inputs, inputs_types = inputs.split(), inputs_types.split()
    vectorized_file = "../vectorized_code.txt"
    with open(vectorized_file, "r") as file:
        vectorized_content = file.read()
    expressions = vectorized_content.split("\n")
    slot_count, sub_vector_size = expressions[-1].split(" ")
    outputs = []
    for expression in expressions[:-1]:
        tokens = process_vectorized_code(expression)
        process(tokens, 0, {}, inputs, inputs_types)
        outputs.append("c" + str(id_counter - 1))
    final_output_label = "c" + str(id_counter)
    final_output = "Ciphertext " + final_output_label + " = " + outputs[0]
    step = 1
    sub_vector_size = int(sub_vector_size)
    for output in outputs[1:]:
        final_output += " + " + output + ">>" + str(sub_vector_size * step)
        step += 1
    final_output += ";\n"
    computations += final_output
    computations += f'{final_output_label}.set_output("{final_output_label}");\n'
    content = file_content.format(
        function_definition=f"void fhe() {{\n{declarations}{computations}}}\n",
        slot_count=slot_count,
    )
    create_file("fhe_vectorized.cpp", content)
    create_file("new_inputs_outputs.txt", new_inputs + final_output_label)
