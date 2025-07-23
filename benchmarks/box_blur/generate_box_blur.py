import numpy as np
from math import sqrt
import argparse
# Create the parser
parser = argparse.ArgumentParser(description="Get io_file generation parameters")
is_vectorization_activated = True
parser.add_argument("--slot_count", required=True,type=int,help="Slot_count")
# Parse arguments
args = parser.parse_args()
slot_count = args.slot_count 
#################### 
n_rows_image = slot_count
n_cols_image = n_rows_image 
N = n_rows_image*n_cols_image
input_image = np.random.randint(0,10,(N))
output_image = np.zeros((n_rows_image*n_cols_image))
##############################################
##############################################
kernel = [[1,1,1],[1,1,1],[1,1,1]]
for x in range(n_rows_image): 
    for y in range(n_cols_image):
        t=0 
        for i in range(-1,2):
            for j in range(-1,2):
                ni = x + i
                nj = y + j
                if ni >= 0 and ni < n_rows_image and nj >= 0 and nj < n_cols_image :
                    t += kernel[i+1][j+1]*input_image[ni*n_cols_image+nj]
        output_image[(x*n_cols_image+y)%N]=t
########################################################################
########################################################################
is_cipher = 1 
is_signed = 0
if not is_vectorization_activated : 
    function_slot_count= N
    nb_inputs = 1
    nb_outputs = 1
    #nb_outputs = n_rows_out*n_cols_out
    #nb_outputs = 1 # 3*3*4 
    with open("fhe_io_example.txt", "w") as f:
        header = str(function_slot_count)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
        f.write(header)
        f.write(f"img {is_cipher} {is_signed} "+" ".join(f"{num}" for num in input_image )+"\n")
        f.write(f"result {is_cipher} "+" ".join(f"{int(num)}" for num in output_image)+"\n")
else : 
    function_slot_count= 1
    nb_inputs = N
    nb_outputs = N
    #nb_outputs = n_rows_out*n_cols_out
    #nb_outputs = 1 # 3*3*4 
    with open("fhe_io_example.txt", "w") as f:
        header = str(function_slot_count)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
        f.write(header)
        input_lines = []
        output_lines = []
        for i in range(n_rows_image):
            for j in range(n_cols_image):
                input_line= "in_{}_{}".format(i,j)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(int(input_image[i*n_cols_image+j]))+"\n"
                input_lines.append(input_line)
                output_line= "out_{}_{}".format(i,j)+" "+str(is_cipher)+" "+str(int(output_image[i*n_cols_image+j]))+"\n"
                output_lines.append(output_line)
        f.writelines(input_lines)
        f.writelines(output_lines)
###################################################################
##########################################################################
"""
input_image1=np.reshape(input_image,(n_rows_image,n_cols_image))
output_image1=np.reshape(output_image,(n_rows_image,n_cols_image))
half_kernel = 1
for i in range(n_rows_image):
        for j in range(n_cols_image):
            total_sum = 0
            # Traverse the kernel window centered around (i, j)
            for ki in range(-half_kernel, half_kernel + 1):
                for kj in range(-half_kernel, half_kernel + 1):
                    ni, nj = i + ki, j + kj

                    # Ensure the indices are within the image bounds
                    if 0 <= ni < n_rows_image and 0 <= nj < n_cols_image:
                        total_sum += input_image1[ni, nj]

            # Calculate the average for the output pixel
            output_image1[i, j] = total_sum 

# Process each element
for i in range(n_rows_image):
        for j in range(n_cols_image):
            # Handle boundary elements by copying img to output
            if i == 0 or i == n_rows_image - 1 or j == 0 or j == n_cols_image - 1:
                output[i*n_cols_image+j] = input_image[i*n_cols_image+j]
                continue

            # Compute the sum of neighboring elements (with boundary checks)
            sum_ = input_image[i*n_cols_image+j]  # Start with the current pixel

            # Add neighbors with boundary checks
            if i > 0 and j > 0:
                sum_ = sum_ + input_image[(i - 1)*n_cols_image+j - 1]  # Low left
            if j > 0:
                sum_ = sum_ + input_image[i*n_cols_image+j - 1]      # Low center
            if i < n_rows_image - 1 and j > 0:
                sum_ = sum_ + input_image[(i + 1)*n_cols_image+j - 1]  # Low right

            if i > 0:
                sum_ = sum_ + input_image[(i - 1)*n_cols_image+j]      # Mid left
            if i < n_rows_image - 1:
                sum_ = sum_ + input_image[(i + 1)*n_cols_image+j]      # Mid right

            if i > 0 and j < n_cols_image - 1:
                sum_ = sum_ + input_image[(i - 1)*n_cols_image+j + 1]  # Top left
            if j < n_cols_image - 1:
                sum_ = sum_ + input_image[i*n_cols_image + j + 1]      # Top center
            if i < n_rows_image - 1 and j < n_cols_image - 1:
                sum_ = sum_ + input_image[(i + 1)*n_cols_image + j + 1]  # Top right
            ##############################
            output[i*n_cols_image+j] = sum_
for i in range(n_rows_image):
        for j in range(n_cols_image):
            print(f"{output_image[i*n_cols_image+j]} : {output[i*n_cols_image+j]} \n")
output_image=output 
"""
