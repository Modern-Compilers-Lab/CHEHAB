import numpy as np
def load(file_name):
    data = []
    with open(file_name, 'r') as file:
        for line in file:
            line = line.strip()  # Remove any leading/trailing whitespace or newlines
            if line:  # Ensure the line is not empty
                data.append(int(line))
    return data

# initialize input image
n_rows_image = 28 # 28
n_cols_image = 28 # 28
n_channels_image = 1 # 1
n_channels_in = n_channels_image
kernel_shape = [5,5,1,5] # 5 5 1 5 
print(kernel_shape)
strides = [2,2]
mean_pool_kernel_shape = [2, 2]
input_image = np.random.randint(0,2,(n_rows_image,n_cols_image, n_channels_in))
# pad_2d of image : 
n_rows_kernel = kernel_shape[0] 
n_cols_kernel = kernel_shape[1] 
n_channels_out = kernel_shape[3]  
row_stride = strides[0]
col_stride = strides[1]
######################################################
n_rows_out =int((n_rows_image + 1) / row_stride); # 6 
n_cols_out = int((n_cols_image + 1) / col_stride); # 6

pad_rows = max((n_rows_out - 1) * row_stride + n_rows_kernel - n_rows_image,0); # 2
pad_cols = max((n_cols_out - 1) * col_stride + n_cols_kernel - n_cols_image,0); # 2
pad_top = int(pad_rows / 2)
pad_left =int(pad_cols / 2)

n_rows_out = n_rows_image + pad_rows; # 13
n_cols_out = n_cols_image + pad_cols; # 13
# initialize output 
print(f"padded_2d {(n_rows_out, n_cols_out,n_channels_in)}")
padded_2d = np.zeros((n_rows_out, n_cols_out,n_channels_in)) # (13,13,1)
for i in range(n_rows_image):
    for j in range( n_cols_image):     
        for k in range(n_channels_in):                                                                              
          padded_2d[i + pad_top,j + pad_left] = input_image[i,j]

#############################################
# kernel will be loaded from an existing file
n_channels_kernel = kernel_shape[2]
kernels = np.zeros((n_rows_kernel, n_cols_kernel, n_channels_kernel, n_channels_out))
with open("w1.txt", "r") as f:
    lines = f.readlines()
    flatten_kernel = []
    for line in lines : 
        values = line.split(" ")
        for value in values : 
            flatten_kernel.append(int(value))
    ############################
    comp = 0 
    for i_kernels in range(n_rows_kernel):
        for j_kernels in range(n_cols_kernel):
            for k_kernels in range(n_channels_kernel):
                for k_out in range(n_channels_out):
                    kernels[i_kernels, j_kernels, k_kernels, k_out] = flatten_kernel[comp]
                    comp=comp+1
n_rows_out = int(n_rows_image / row_stride + n_rows_image % row_stride) 
n_cols_out = int(n_cols_image / col_stride + n_cols_image % col_stride) 
print(f"conv_2d : {n_rows_out,n_cols_out,n_channels_out}")
output = np.zeros((n_rows_out, n_cols_out, n_channels_out))
row_offset = 0
for i_out in range(n_rows_out): # n_rows_out
    col_offset = 0
    for j_out in range(n_cols_out): # n_cols_out
        for k_out in range(n_channels_out):
            ###############################
            for i_kernels in range(n_rows_kernel):
                for j_kernels in range(n_cols_kernel):
                    for k_kernels in range(n_channels_kernel):
                        #print(f"{i_kernels + row_offset,j_kernels + col_offset, k_kernels}, {i_kernels, j_kernels, k_kernels, k_out}")
                        output[i_out, j_out, k_out] += (
                            padded_2d[(i_kernels + row_offset),(j_kernels + col_offset), k_kernels] *
                            kernels[i_kernels, j_kernels, k_kernels, k_out]
                        )
        col_offset += col_stride  # Update column offset for stride
    row_offset += row_stride  # Update row offset for stride
############################################################################
b1_raw=load("b1.txt")
print(f"sum with vec b1 : {n_rows_out,n_cols_out,n_channels_out}")
for i_out in range(n_rows_out):
    for j_out in range(n_cols_out):
        for k_out in range(n_channels_out):
            output[i_out, j_out, k_out]=output[i_out, j_out, k_out]+b1_raw[k_out]
       
###########################square output##################################
function_name = "square"
print(f"Calculate the square : {n_rows_out,n_cols_out,n_channels_out}")
for i_out in range(n_rows_out):
    for j_out in range(n_cols_out):
        for k_out in range(n_channels_out):
            output[i_out, j_out, k_out]=output[i_out, j_out, k_out]*output[i_out, j_out, k_out]
            #print(f"output[{i_out}, {j_out}, {k_out}] = {output[i_out, j_out, k_out]}")
################# scaled_mean_pool_2d #####################################
input1 = output
n_rows_in = len(input1) # 14 
n_cols_in = len(input1[0])  # 14 
n_channels_in = len(input1[0][0]); # 5
n_rows_kernel = mean_pool_kernel_shape[0];  # = 2
n_cols_kernel = mean_pool_kernel_shape[1];  # = 2
row_stride = strides[0]
col_stride = strides[1]
n_rows_out = int(n_rows_in / row_stride + (n_rows_in % row_stride))
n_cols_out = int(n_cols_in / col_stride + (n_cols_in % col_stride))
n_channels_output = n_channels_in
print(f"scaled_mean_pool_2d : {n_rows_out, n_cols_out, n_channels_output}")
output = np.zeros((n_rows_out, n_cols_out, n_channels_output))
row_offset = 0
for i_out in range(n_rows_out) : # n_rows_out
    col_offset = 0
    for j_out in range(n_cols_out): # n_cols_out
        for k_out in range(n_channels_output):
            for i_kernel in range(n_rows_kernel):
                for j_kernel in range(n_cols_kernel):
                    output[i_out, j_out, k_out] += input1[i_kernel + row_offset, j_kernel + col_offset, k_out]
        #print(f"output[{i_out}, {j_out}, {k_out}] = {output[i_out, j_out, k_out]}")
        col_offset += col_stride
    row_offset += row_stride
###############################################################################
##############################Conv_2d##########################################
kernel_shape_w4 = [5, 5, 5, 10]
n_rows_kernel = kernel_shape_w4[0]
n_cols_kernel = kernel_shape_w4[1]
n_channels_kernel = kernel_shape_w4[2] 
n_channels_out = kernel_shape_w4[3]
###################### pad_2d #####################
input1 = output 
n_rows_in = len(input1) 
n_cols_in = len(input1[0])   
n_channels_in = len(input1[0][0]); 
n_rows_out =int((n_rows_in + 1) / row_stride); 
n_cols_out = int((n_cols_in + 1) / col_stride); 
pad_rows = max((n_rows_out - 1) * row_stride + n_rows_kernel - n_rows_in,0); 
pad_cols = max((n_cols_out - 1) * col_stride + n_cols_kernel - n_cols_in,0); 
pad_top = int(pad_rows / 2)
pad_left =int(pad_cols / 2)
n_rows_out = n_rows_in + pad_rows; 
n_cols_out = n_cols_in + pad_cols;  
# initialize output 
print(f"padded_2d : {n_rows_out, n_cols_out,n_channels_in}")
padded_2d = np.zeros((n_rows_out, n_cols_out,n_channels_in))
for i in range(n_rows_in):
    for j in range( n_cols_in):     
        for k in range(n_channels_in):                                                                              
          padded_2d[i + pad_top,j + pad_left] = input1[i,j]
##############Loading kernel#############
#########################################
kernels = np.zeros((n_rows_kernel, n_cols_kernel, n_channels_kernel, n_channels_out))
with open("w4.txt", "r") as f:
    lines = f.readlines()
    flatten_kernel = []
    for line in lines : 
        values = line.split(" ")
        for value in values : 
            flatten_kernel.append(int(value))
    ############################
    comp = 0 
    for i_kernels in range(n_rows_kernel):
        for j_kernels in range(n_cols_kernel):
            for k_kernels in range(n_channels_kernel):
                for k_out in range(n_channels_out):
                    kernels[i_kernels, j_kernels, k_kernels, k_out] = flatten_kernel[comp]
                    comp=comp+1
#####################################################################
#print(f"n_rows_in = {n_rows_in} , n_cols_in = {n_cols_in} ")
n_rows_out = int(n_rows_in / row_stride + n_rows_in%row_stride) # 14
n_cols_out = int(n_cols_in / col_stride + n_cols_in%col_stride)  # 14
print(f"conv_2d : {n_rows_out,n_cols_out,n_channels_out}")
output = np.zeros((n_rows_out, n_cols_out, n_channels_out))
row_offset = 0
for i_out in range(n_rows_out): # n_rows_out
    col_offset = 0
    for j_out in range(n_cols_out): # n_cols_out
        for k_out in range(n_channels_out):
            ###############################
            for i_kernels in range(n_rows_kernel):
                for j_kernels in range(n_cols_kernel):
                    for k_kernels in range(n_channels_kernel):
                        output[i_out, j_out, k_out] += (
                            padded_2d[(i_kernels + row_offset),(j_kernels + col_offset), k_kernels] *
                            kernels[i_kernels, j_kernels, k_kernels, k_out]
                        )
            ###############################
            print(f"output[{i_out}, {j_out}, {k_out}] = {output[i_out, j_out, k_out]}")
        col_offset += col_stride  # Update column offset for stride
    row_offset += row_stride  # Update row offset for stride
##############################################################################
b4_raw=load("b4.txt")
print(f"sum with vec b4 : {n_rows_out,n_cols_out,n_channels_out}")
for i_out in range(n_rows_out):
    for j_out in range(n_cols_out):
        for k_out in range(n_channels_out):
            output[i_out, j_out, k_out]=output[i_out, j_out, k_out]+b4_raw[k_out]
################# scaled_mean_pool_2d #####################################
input1 = output
n_rows_in = len(input1) # 14 
n_cols_in = len(input1[0])  # 14 
n_channels_in = len(input1[0][0]); # 5
n_rows_kernel = mean_pool_kernel_shape[0];  # = 2
n_cols_kernel = mean_pool_kernel_shape[1];  # = 2
row_stride = strides[0]
col_stride = strides[1]
n_rows_out = int(n_rows_in / row_stride + (n_rows_in % row_stride))
n_cols_out = int(n_cols_in / col_stride + (n_cols_in % col_stride))
n_channels_output = n_channels_in
print(f"scaled_mean_pool_2d : {n_rows_out, n_cols_out, n_channels_output}")
output = np.zeros((n_rows_out, n_cols_out, n_channels_output))
row_offset = 0
for i_out in range(n_rows_out) : # n_rows_out
    col_offset = 0
    for j_out in range(n_cols_out): # n_cols_out
        for k_out in range(n_channels_output):
            for i_kernel in range(n_rows_kernel):
                for j_kernel in range(n_cols_kernel):
                    output[i_out, j_out, k_out] += input1[i_kernel + row_offset, j_kernel + col_offset, k_out]
        #print(f"output[{i_out}, {j_out}, {k_out}] = {output[i_out, j_out, k_out]}")
        col_offset += col_stride
    row_offset += row_stride
############################### Flattening #################################
input1 = output 
n_rows = n_rows_out*n_cols_out*n_channels_output
output = np.zeros((n_rows))
comp = 0
print(f"Flatteing==> {(n_rows)}")
for i_out in range(n_rows_out) :
    for j_out in range(n_cols_out): 
        for k_out in range(n_channels_output):
            output[comp] = input1[i_out, j_out, k_out]
            comp=comp+1
##########################Square################################################
print(f"square {(n_rows)}")
for i_out in range(n_rows):
    output[i_out] = output[i_out]*output[i_out]
########################Dot product ########################################
##########load w8 ###################
w8_shape = [40,10]
weights = np.zeros((w8_shape[0], w8_shape[1]))
with open("w8.txt", "r") as f:
    lines = f.readlines()
    flatten_kernel = []
    for line in lines : 
        values = line.split(" ")
        for value in values : 
            flatten_kernel.append(int(value))
    comp = 0 
    for i_kernels in range(w8_shape[0]):
        for j_kernels in range(w8_shape[1]):
            weights[i_kernels, j_kernels] = flatten_kernel[comp]
            comp=comp+1
######################################
input1 = output 
print(f"Dot_product :  {(w8_shape[1])}")
output = np.zeros((w8_shape[1]))
for i in range(w8_shape[1]) :
    for j in range(w8_shape[0]):
        output[i] += input1[j] * weights[j][i]
##############################################
b8_raw=load("b8.txt")
n_rows_out = len(b8_raw)
print(f"sum with vec b8 : {n_rows_out}")
for i_out in range(n_rows_out):
        output[i_out]=(output[i_out]+b8_raw[i_out])%65537
##############################################################################
########################################################################
########### Formatting the output file ##################################
with open("fhe_io_example.txt","w") as file : 
        is_cipher = 1 
        is_signed = 1
        nb_inputs= n_rows_image*n_cols_image*n_channels_image
        nb_outputs = n_rows_out
        function_slot_count = 1
        header = str(function_slot_count)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
        file.write(header) 
        rows=[]
        for i in range(n_rows_image): 
            for j in range(n_cols_image):
                for k in range(n_channels_image):
                    row = "in_{}_{}_{}".format(i,j,k)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(input_image[i,j,k])+"\n"
                    rows.append(row)
        file.writelines(rows)
        rows=[]
        for i in range(n_rows_out): 
            row = "out_{}".format(i)+" "+str(is_cipher)+" "+str(output[i])+"\n"
            rows.append(row)
        file.writelines(rows)