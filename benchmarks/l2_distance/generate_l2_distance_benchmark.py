import numpy as np
function_slot_count= 64
c1 = np.random.randint(0,10,(function_slot_count))
c2 = np.random.randint(0,10,(function_slot_count))
result = np.zeros((function_slot_count)) 
res = 0 
for i in range(function_slot_count):
    res+= (c1[i] - c2[i])*(c1[i] - c2[i]) 
for i in range(function_slot_count):
    result[i]=res
is_cipher = 1 
is_signed = 1
## writing matrix to file  
with open("fhe_io_example.txt","w") as file : 
    nb_inputs= 2
    nb_outputs = 1
    header = str(function_slot_count)+" "+str(nb_inputs)+" "+str(nb_outputs)+"\n"
    file.write(header) 
    rows=[]
    row = "c1"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c1)+"\n"
    rows.append(row)
    row = "c2"+" "+str(is_cipher)+" "+str(is_signed)+" "+" ".join(str(num) for num in c2)+"\n"
    rows.append(row)
    row = "result"+" "+str(is_cipher)+" "+" ".join(str(int(num)) for num in result)+"\n"
    rows.append(row)
    file.writelines(rows)