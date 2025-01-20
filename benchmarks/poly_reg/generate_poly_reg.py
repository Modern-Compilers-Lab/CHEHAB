import numpy as np
import argparse
# Create the parser
parser = argparse.ArgumentParser(description="Get io_file generation parameters")
is_vectorization_activated = True
parser.add_argument("--slot_count", required=True,type=int, help="Slot_count", default=0)
# Parse arguments
args = parser.parse_args()
function_slot_count = args.slot_count 
##################################################
c0 = np.random.randint(0,10,(function_slot_count))
c1 = np.random.randint(0,10,(function_slot_count))
c2 = np.random.randint(0,10,(function_slot_count))
c3 = np.random.randint(0,10,(function_slot_count))
c4 = np.random.randint(0,10,(function_slot_count))
result = np.zeros((function_slot_count)) 
for i in range(function_slot_count):
    result[i]=c1[i] + (c0[i] * c0[i] * c4[i] + c0[i] * c3[i] + c2[i])
is_cipher = 1 
is_signed = 0
## writing matrix to file  
if is_vectorization_activated :
    with open("fhe_io_example.txt","w") as file : 
        nb_inputs = function_slot_count * 5 
        nb_outputs = function_slot_count 
        header = str(1)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
        file.write(header)
        rows =[]
        for i in range(function_slot_count):
            row = "c0_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(c0[i])+"\n"
            rows.append(row)
        for i in range(function_slot_count):
            row = "c1_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(c1[i])+"\n"
            rows.append(row)
        for i in range(function_slot_count):
            row = "c2_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(c2[i])+"\n"
            rows.append(row)
        for i in range(function_slot_count):
            row = "c3_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(c3[i])+"\n"
            rows.append(row)
        for i in range(function_slot_count):
            row = "c4_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(c4[i])+"\n"
            rows.append(row)
        for i in range(function_slot_count):
            row = "c_result_{}".format(i)+" "+str(is_cipher)+" "+str(int(result[i]))+"\n"
            rows.append(row)
        file.writelines(rows)
#######################################################################
#######################################################################
else :
    with open("fhe_io_example.txt","w") as file : 
        nb_inputs= 5
        nb_outputs = 1
        header = str(function_slot_count)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
        file.write(header) 
        rows=[] 
        row = "c0"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c0)+"\n"
        rows.append(row)
        row = "c1"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c1)+"\n"
        rows.append(row)
        row = "c2"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c2)+"\n"
        rows.append(row)
        row = "c3"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c3)+"\n"
        rows.append(row)
        row = "c4"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c4)+"\n"
        rows.append(row)
        row = "c_result"+" "+str(is_cipher)+" "+" ".join(str(int(num)) for num in result)+"\n"
        rows.append(row)
        file.writelines(rows)