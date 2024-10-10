import numpy as np
function_slot_count= 64
c0 = np.random.randint(0,10,(function_slot_count))
c1 = np.random.randint(0,10,(function_slot_count))
c2 = np.random.randint(0,10,(function_slot_count))
c3 = np.random.randint(0,10,(function_slot_count))
result = np.zeros((function_slot_count))
for i in range(function_slot_count):
    result[i]=c1[i] - (c2[i] * c0[i]) - c3[i]
is_cipher = 1 
is_signed = 1
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