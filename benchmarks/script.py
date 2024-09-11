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
    slot_count: int,
    sub_vector_size: int,
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
    global id_counter, computations, declarations, new_inputs, labels_map
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
                            new_input += "@" + str(inputs.index(element)) + " "
                            if inputs_types[inputs.index(element)] == "1":
                                all_literals = False
                        else:
                            new_input += element + " "
                    number_of_sub_vectors = slot_count // sub_vector_size
                    new_input = (" " + new_input) * number_of_sub_vectors
                    new_input = new_input.strip()
                    if all_literals:
                        label_type = "Plaintext "
                        label = "p" + str(id_counter)
                        new_input = "0 1 " + new_input
                    else:
                        label_type = "Ciphertext "
                        label = "c" + str(id_counter)
                        new_input = "1 1 " + new_input
                    labels_map[id_counter] = label
                    new_inputs += new_input + "\n"
                    id_counter += 1
                    declarations += f'{label_type}{label}("{label}");\n'
                    dictionary[vector] = label
                return dictionary[vector], index
            if len(tokens[index]) > 3:
                operation = tokens[index]
                index += 1
                operand_1, index = process(
                    tokens,
                    index,
                    dictionary,
                    inputs,
                    inputs_types,
                    slot_count,
                    sub_vector_size,
                )
                if tokens[index] == "(":
                    operand_2, index = process(
                        tokens,
                        index,
                        dictionary,
                        inputs,
                        inputs_types,
                        slot_count,
                        sub_vector_size,
                    )
                    op = (
                        "+"
                        if operation == "VecAdd"
                        else "-" if operation == "VecMinus" else "*"
                    )
                    label = "c" + str(id_counter)
                    labels_map[id_counter] = label
                    id_counter += 1
                    computations += (
                        f"Ciphertext {label} = {operand_1} {op} {operand_2};\n"
                    )
                    index += 1
                    return label, index
                else:
                    index += 1
                    label = "c" + str(id_counter)
                    labels_map[id_counter] = label
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
    labels_map = (
        {}
    )  # Dictionary to store labels for inputs, outputs and intermediate results
    id_counter = 0  # Counter for labeling each new declared ciphertext or plaintext
    declarations, computations, new_inputs = (
        "",
        "",
        "",
    )  # Initializations for the declarations, computations, and new inputs
    inputs_file = "../inputs.txt"  # Path to the scalar inputs file

    # Read the input file and split the contents by spaces
    with open(inputs_file) as f:
        inputs, input_types = f.readlines()
    inputs = inputs.split()
    input_types = input_types.split()

    vectorized_file = (
        "../vectorized_code.txt"  # Path to the vectorized expressions file
    )

    # Read the vectorized file content
    with open(vectorized_file, "r") as file:
        vectorized_content = file.read()

    # Split vectorized expressions by new lines
    expressions = vectorized_content.split("\n")
    # The last line contains two values: the size of the final vector and the size of each sub vector
    slot_count, sub_vector_size = int(expressions[-1].split(" ")[0]), int(
        expressions[-1].split(" ")[1]
    )
    outputs = []  # List to store the outputs of sub-programs

    # Process each expression except the last line
    for expression in expressions[:-1]:
        tokens = process_vectorized_code(expression)  # Split the expression into tokens
        process(
            tokens, 0, {}, inputs, input_types, slot_count, sub_vector_size
        )  # Process tokens to determine new vectorized inputs and necessary computations for sub outputs
        outputs.append(labels_map[id_counter - 1])  # Append the output label

    if len(outputs) == 1:
        # If there is only one sub output, no need to calculate the final output
        final_output_label = outputs[0]
    else:
        final_output_label = f"c{id_counter}"  # Label for the final output

        for i in range(len(outputs)):
            declarations += f'Plaintext mask_{i}("mask_{i}");\n'
            new_input = "0 1"  # Declare mask
            for j in range(0, i * sub_vector_size):
                new_input += " 0"
            for j in range(i * sub_vector_size, (i + 1) * sub_vector_size):
                new_input += " 1"
            for j in range((i + 1) * sub_vector_size, slot_count):
                new_input += " 0"
            new_inputs += new_input + "\n"

        final_output = f"Ciphertext {final_output_label} = "  # Initialize the final output with the first sub output

        # Calculate the final output using sub outputs with mask multiplication
        for i in range(len(outputs)):
            final_output += f" + ({outputs[i]}*mask_{i})"

    final_output += ";\n"
    computations += final_output  # Add the final output to computations
    computations += f'{final_output_label}.set_output("{final_output_label}");\n'  # Set the output label

    # Format the final source code content
    content = file_content.format(
        function_definition=f"void fhe() {{\n{declarations}{computations}}}\n",
        slot_count=str(slot_count),
    )

    # Create the new source file and the mapping file for inputs and outputs
    create_file("fhe_vectorized.cpp", content)
    create_file("new_inputs_outputs.txt", new_inputs + final_output_label)
