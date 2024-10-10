import numpy as np
from math import sqrt
slot_count = 64
####################
n_rows_image = int(sqrt(slot_count))
n_cols_image = n_rows_image
N = n_rows_image*n_cols_image
input_image = np.random.randint(0,10,(N))
output_image = np.zeros((n_rows_image*n_cols_image))
###################
kernel = [[1,1,1],[1,8,1],[1,1,1]]
for x in range(n_rows_image):
    for y in range(n_cols_image):
        t=0 
        for j in range(-1,2):
            for i in range(-1,2):
                #print(f"kernel :{((j+1)*3)+i+1}, Image :{((x+i)*n_rows_image+(y+j))%N}")
                t+=kernel[i+1][j+1]*input_image[((x+i)*n_rows_image+(y+j))%N]
        output_image[(x*n_rows_image+y)%N]=t
is_cipher = 1 
is_signed = 0
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
    

