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
is_cipher = 1  
is_signed = 1
if is_vectorization_activated :
    m = 5 
    b = 2
    v1 = np.random.randint(0,10,(function_slot_count))
    v2 = np.random.randint(0,10,(function_slot_count))
    output = np.random.randint(0,10,(function_slot_count))
    for i in range(function_slot_count):
        output[i]= v2[i] + (m * v1[i] + b)
    with open("fhe_io_example.txt","w") as file : 
        nb_inputs = function_slot_count*2 
        nb_outputs = function_slot_count
        slot_count = 1
        header = str(slot_count)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
        file.write(header) 
        rows = []
        for i in range(function_slot_count):
            line= "v1_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(int(v1[i]))+"\n"
            rows.append(line)
        for i in range(function_slot_count):
            line= "v2_{}".format(i)+" "+str(is_cipher)+" "+str(is_signed)+" "+str(int(v2[i]))+"\n"
            rows.append(line)
        for i in range(function_slot_count):
            line= "output_{}".format(i)+" "+str(is_cipher)+" "+str(int(output[i]))+"\n"
            rows.append(line)
        file.writelines(rows)
    ###############################################################################
    ###############################################################################
else :
    c0 = np.random.randint(0,10,(function_slot_count))
    c1 = np.random.randint(0,10,(function_slot_count))
    c2 = np.random.randint(0,10,(function_slot_count))
    c3 = np.random.randint(0,10,(function_slot_count))
    result = np.zeros((function_slot_count))
    for i in range(function_slot_count):
        result[i]=c1[i] - (c2[i] * c0[i]) - c3[i]
    ## writing matrix to file  
    with open("fhe_io_example.txt","w") as file : 
        nb_inputs= 4
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
        row = "c_result"+" "+str(is_cipher)+" "+" ".join(str(num) for num in result)+"\n"
        rows.append(row)
        file.writelines(rows)