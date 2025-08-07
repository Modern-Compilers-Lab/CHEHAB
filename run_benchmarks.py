import os
import shutil
import subprocess
import csv 
import re
import statistics
# Specify the parent folder containing the benchmarks and build subfolders
benchmarks_folder = "benchmarks"  
################################
build_folder = os.path.join("build", "benchmarks")
operations = ["add", "sub", "multiply_plain", "rotate_rows", "negate", "multiply"]
infos = ["benchmark"]
additional_infos =[ "Depth", "Multplicative Depth","compile_time (s)", "execution_time (s)"]
infos.extend(operations) 
infos.extend(additional_infos) 

#############################################
try:
    print("run=> cmake', '-S', '.', '-B', 'build' ")
    result = subprocess.run(
        ['cmake', '-S', '.', '-B', 'build'], 
        check=True, 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE, 
        universal_newlines=True
    )
    print("run=> 'cmake', '--build', 'build'")
    result = subprocess.run(
        ['cmake', '--build', 'build'], 
        check=True, 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE, 
        universal_newlines=True
    )  
except subprocess.CalledProcessError as e:
    print(f"Command failed with error:\n{e.stderr}")    

benchmark_folders = ["max","sort","discrete_cosin_transform","poly_derivative","box_blur","lin_reg","hamming_dist","poly_reg","l2_distance","dot_product","gx_kernel","gy_kernel","roberts_cross","matrix_mul","max","sort"] 
#benchmark_folders = ["lin_reg","hamming_dist","poly_reg","l2_distance","dot_product","gx_kernel","gy_kernel","roberts_cross","matrix_mul","max","sort"] 
exceptions = ["max","sort","discrete_cosin_transform","poly_derivative"]
benchmarks_slot_counts  = {
    "max" : [3,4,5], 
    "sort" : [3,4],
    "discrete_cosin_transform":[1],
    "poly_derivative":[1]
} 
###############################
### specify the number of iteration  
###### Configurations ############## 
optimization_method = 0 # 0 = egraph (default), 1 = RL
cse_enabled = 1
vectorize_code = 1 
slot_counts= [4,8,16,32]
iterations = 30 #minimum 2
window_size = 0    
depths = [5,10] 
regimes = ["50-50","100-50","100-100"]
number_instances_each_polynomial_configuration = 2
compile_time_timeout_seconds = 7200
output_csv = f"results_{'RL' if optimization_method == 1 else 'EGraph'}.csv"

######################################
with open(output_csv, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(infos)
########################################
for subfolder_name in benchmark_folders:
    benchmark_path = os.path.join(benchmarks_folder, subfolder_name)
    build_path = os.path.join(build_folder, subfolder_name) 
    optimization_time = ""
    execution_time = ""
    depth = ""
    multiplicative_depth = ""
    if os.path.isdir(build_path):
        ###############################################
        ##### loop over specified slot_counts #########
        updated_slot_counts = slot_counts 
        if subfolder_name in exceptions :
            updated_slot_counts = benchmarks_slot_counts[subfolder_name]
        for slot_count in updated_slot_counts:
            try :
                benchmark_compilation_timed_out = False
                print("****************************************************************")
                print(f"*****run {subfolder_name} , for slot_count : {slot_count}******")
                operation_stats = {
                "add": [], "sub": [], "multiply_plain": [], "rotate_rows": [],
                "negate": [], "multiply": [], "Depth": [], "Multiplicative Depth": [],
                "compile_time (s)": [], "execution_time (s)": [], 
                }
                ###generate io_file for benchmark with slot_count 
                if not subfolder_name in exceptions :
                    pro = subprocess.Popen(['python3', 'generate_{}.py'.format(subfolder_name),'--slot_count',str(slot_count)],cwd=build_path)
                    pro.wait()
                ######################################
                for iteration in range(iterations):
                    print(f"===> Running iteration : {iteration + 1}")
                    # Step 1: Run the first benchmark command
                    benchmark_run_command = f"./{subfolder_name} {vectorize_code} {slot_count} {optimization_method} {window_size} 1 {cse_enabled}  1 "
                    try: 
                        result = subprocess.run(
                            benchmark_run_command, shell=True, check=True, 
                            stdout=subprocess.PIPE, 
                            stderr=subprocess.PIPE, 
                            universal_newlines=True, 
                            cwd=build_path,
                            timeout=compile_time_timeout_seconds 
                        )
                        lines = result.stdout.splitlines()

                        # Collect compile time (ms)
                        compile_time_found = False 
                        poly_mod_found = True 
                        for line in lines: 
                            if ' ms' in line:
                                #print(f"=======> compile_time line : {line}")
                                optimization_time = float(line.split()[0])
                                operation_stats["compile_time (s)"].append(optimization_time)
                                compile_time_found = True
                            if 'poly_mod:' in line:
                                print(f"======> poly_mod : {line}")
                                poly_mod = float(line.split()[1])
                                operation_stats["poly_modulus"].append(poly_mod)
                                poly_mod_found = True
                            if compile_time_found and poly_mod_found :
                                break
                        # Collect depth and multiplicative depth
                        #print(result.stdout)
                        depth_match = re.search(r'max:\s*\((\d+),\s*(\d+)\)', result.stdout)
                        print(depth_match)
                        depth = int(depth_match.group(1)) if depth_match else None
                        multiplicative_depth = int(depth_match.group(2)) if depth_match else None
                        #print(depth)
                        #print(multiplicative_depth)
                        print(f"Depth=>{depth}, multiplcative_depth=>{multiplicative_depth}")
                        operation_stats["Depth"].append(depth)
                        operation_stats["Multiplicative Depth"].append(multiplicative_depth)
                    except subprocess.TimeoutExpired:
                        print(f"Command `{benchmark_run_command}` timed out after {compile_time_timeout_seconds} seconds.")
                        benchmark_compilation_timed_out = True
                    except subprocess.CalledProcessError as e:
                        error_message = e.stderr if e.stderr else "No error message available."
                        print("Command for {} failed with error:\n{}".format(subfolder_name, error_message))
                        continue
                    if benchmark_compilation_timed_out : 
                        break 
                    ## building and running fhe code 
                    build_path_he = os.path.join(build_path, "he")
                    result = subprocess.run(['cmake', '-S', '.', '-B', 'build'], 
                                cwd=build_path_he,
                                check=True, 
                                stdout=subprocess.PIPE, 
                                stderr=subprocess.PIPE, 
                                universal_newlines=True
                                )
                    result =subprocess.run(['cmake', '--build', 'build'], cwd=build_path_he,universal_newlines=True,
                        check=True, 
                        stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE,)
                    build_path_he_build = os.path.join(build_path_he, "build")
                    # Step 2: Build and run fhe code
                    
                    if iteration == iterations-1 :
                        build_path_he = os.path.join(build_path, "he")
                        try:
                            # Run the compiled program 
                            for counter in range(iterations):
                                command = f"./main"
                                result = subprocess.run(
                                    command, shell=True, check=True, 
                                    stdout=subprocess.PIPE, 
                                    stderr=subprocess.PIPE, 
                                    universal_newlines=True, 
                                    cwd=build_path_he_build
                                )
                                print("**fhe run done**")
                                #print(result.stderr)
                                if counter > 0 :
                                    lines = result.stdout.splitlines()
                                    # Collect execution time (ms)
                                    for line in lines:
                                        if ' ms' in line:
                                            #print(f"==> execution time {line.split()[0]}")
                                            execution_time = float(line.split()[0])
                                            operation_stats["execution_time (s)"].append(execution_time)
                                            break

                        except subprocess.CalledProcessError as e:
                            print(f"Failed in building fhe_code for benchmark: {subfolder_name}")
                            continue

                    # Step 3: Parse operation counts from the generated C++ code
                    file_name = os.path.join(build_path_he, "_gen_he_fhe.cpp")
                    with open(file_name, "r") as file:
                        file_content = file.read()
                        for op in operations:
                            nb_occurrences = len(re.findall(rf'\b{op}', file_content))
                            operation_stats[op].append(int(nb_occurrences))
                ####################################################################
                bench_name = subfolder_name+"_"+str(slot_count)
                row=[bench_name]
                if not benchmark_compilation_timed_out : 
                    for key, values in operation_stats.items():
                        if values == []:
                            print(f"Warning: No values found for {key} in {subfolder_name} with slot_count {slot_count}.")
                            result = "N/A"
                        else : 
                            result = statistics.median(values) 
                            if key == "compile_time (s)" or key == "execution_time (s)" :
                                result = result / 1000
                                result = format(result, ".3f")
                            row.append(result) if values else None
                                     
                        print(f"{key} {values} {result}")
                ##########################################################################
                ##########################################################################
                with open(output_csv, mode='a', newline='') as file:
                    writer = csv.writer(file)
                    writer.writerow(row)
            except Exception as e:
                print(f"Command for {subfolder_name} failed with error:\n{e}")
                continue
######################################################################################
######################################################################################
print("Run polynomial benchmarks !!!!!! ")
polynomial_folders = ["polynomials_coyote"]
for subfolder_name in polynomial_folders: 
    benchmark_path = os.path.join(benchmarks_folder, subfolder_name)
    build_path = os.path.join(build_folder, subfolder_name)
    # build_path = build/benchmarks/dot_product 
    ## informations to collect 
    for regime in regimes :
        for tree_depth in depths :
            for instance in range(1,number_instances_each_polynomial_configuration+1):
                try:  
                    benchmark_compilation_timed_out = False
                    operation_stats = {
                    "add": [], "sub": [], "multiply_plain": [], "rotate_rows": [],
                    "negate": [], "multiply": [], "Depth": [], "Multiplicative Depth": [],
                    "compile_time (s)": [], "execution_time (s)": [], 
                    }
                    benchmark_name = f'tree_{regime}_{tree_depth}_{instance}'
                    print(f"Benchmark '{benchmark_name}' will be run...")
                    for iteration in range(iterations):
                        optimization_time=""
                        execution_time=""
                        depth = ""
                        multiplicative_depth = ""
                        if os.path.isdir(build_path):
                            print(f"=========> Iteration : {iteration+1}")
                            command = f"./{subfolder_name} {tree_depth} {instance} {regime} {vectorize_code} {optimization_method}"
                            try: 
                                result = subprocess.run(
                                    command, shell=True, check=True, 
                                    stdout=subprocess.PIPE, 
                                    stderr=subprocess.PIPE, 
                                    universal_newlines=True, 
                                    cwd=build_path,
                                    timeout=compile_time_timeout_seconds 
                                )
                                lines = result.stdout.splitlines()
                                compile_time_found = False 
                                poly_mod_found = True
                                for line in lines: 
                                    if ' ms' in line:
                                        print(f"=======> compile_time line : {line}")
                                        optimization_time = float(line.split()[0])
                                        operation_stats["compile_time (s)"].append(optimization_time)
                                        compile_time_found = True
                                    if compile_time_found and poly_mod_found :
                                        break
                                depth_match = re.search(r'max:\s*\((\d+),\s*(\d+)\)', result.stdout)
                                #print(f"\n\n {depth_match} \n\n")
                                depth = depth_match.group(1) if depth_match else None
                                multiplicative_depth = depth_match.group(2) if depth_match else None
                                operation_stats["Depth"].append(int(depth))
                                operation_stats["Multiplicative Depth"].append(int(multiplicative_depth))
                                print(f"Depth: {depth} --MultipliDepth {multiplicative_depth}")
                            except subprocess.TimeoutExpired:
                                print(f"Command `{command}` timed out after {compile_time_timeout_seconds} seconds.")
                                benchmark_compilation_timed_out = True
                            except subprocess.CalledProcessError as e:
                                print(f"Command for {subfolder_name} failed with error:\n{e.stderr}")
                            if benchmark_compilation_timed_out : 
                                break 
                            #########################################################################
                            ## building and running fhe code 
                            build_path_he = os.path.join(build_path, "he")
                            try:
                                result = subprocess.run(
                                    ['cmake', '-S', '.', '-B', 'build'], 
                                    check=True, 
                                    stdout=subprocess.PIPE, 
                                    stderr=subprocess.PIPE, 
                                    universal_newlines=True, 
                                    cwd=build_path_he
                                )
                                result = subprocess.run(
                                    ['cmake', '--build', 'build'], 
                                    check=True, 
                                    stdout=subprocess.PIPE, 
                                    stderr=subprocess.PIPE, 
                                    universal_newlines=True, 
                                    cwd=build_path_he
                                )
                                # result = subprocess.run(['sudo','cmake','--install','build'], check=True, capture_output=False, text=True)
                            except :
                                print(f"Failed in building fhe_code for benchmark:{subfolder_name} ,with error \n")   
                            build_path_he_build = os.path.join(build_path_he, "build")
                            ########################################################################## 
                            if iteration == iterations-1 :
                                try:
                                    # Run the compiled program 
                                    for counter in range(iterations):
                                        command = f"./main"
                                        result = subprocess.run(
                                            command, shell=True, check=True, 
                                            stdout=subprocess.PIPE, 
                                            stderr=subprocess.PIPE, 
                                            universal_newlines=True, 
                                            cwd=build_path_he_build
                                        )
                                        print("**fhe run done**")
                                        #print(result.stderr)
                                        if counter > 0 :
                                            lines = result.stdout.splitlines()
                                            # Collect execution time (ms)
                                            for line in lines:
                                                if ' ms' in line:
                                                    #print(f"==> execution time {line.split()[0]}")
                                                    execution_time = float(line.split()[0])
                                                    operation_stats["execution_time (s)"].append(execution_time)
                                                    break

                                except subprocess.CalledProcessError as e:
                                    print(f"Failed in running fhe_code for benchmark: {subfolder_name}")
                                    continue
                            ###########################################################################
                        # Step 3: Parse operation counts from the generated C++ code
                        file_name = os.path.join(build_path_he, "_gen_he_fhe.cpp")
                        with open(file_name, "r") as file:
                            file_content = file.read()
                            for op in operations:
                                nb_occurrences = len(re.findall(rf'\b{op}', file_content))
                                operation_stats[op].append(int(nb_occurrences))
                            try:
                                command = f"./main"
                                result = subprocess.run(
                                    command, shell=True, check=True, 
                                    stdout=subprocess.PIPE, 
                                    stderr=subprocess.PIPE, 
                                    universal_newlines=True, 
                                    cwd=build_path_he_build
                                )
                                lines = result.stdout.splitlines()
                                for line in lines :
                                    if ' ms' in line:
                                        execution_time = line.split()[0]
                                        print(f"====> Execution time {execution_time}")
                                        operation_stats["execution_time (s)"].append(float(execution_time))
                                        break
                                ## analyzing generated code for statistics ###
                                file_name = build_path_he +"/_gen_he_fhe.cpp"
                                ########################################################
                                with open(file_name,"r") as file : 
                                    file_content = file.read()
                                    for op in operations :
                                        nb_occurrences = len(re.findall(rf'\b{op}', file_content))
                                        operation_stats[op].append(int(nb_occurrences))              
                                # print(f"Output for {subfolder_name}:\n{result.stdout}")
                            except subprocess.CalledProcessError as e:
                                print(f"Running fhe code for {subfolder_name} failed with error:\n{e.stderr}")
                    ##################################################################
                    ##################################################################
                    row=[benchmark_name]
                    if not benchmark_compilation_timed_out : 
                        for key, values in operation_stats.items():
                            if values == []:
                                print(f"Warning: No values found for {key} in {subfolder_name} with slot_count {slot_count}.")
                                result = "N/A"
                            else : 
                                result = statistics.median(values) 
                                if key == "compile_time (s)" or key == "execution_time (s)" :
                                    result = result / 1000
                                    result = format(result, ".3f")
                                row.append(result) if values else None
                                    
                            print(f"{key} {values} {result}")
                    with open(output_csv, mode='a', newline='') as file:
                        writer = csv.writer(file)
                        writer.writerow(row)   
                except Exception as e:
                    print(f"Command for {subfolder_name} failed with error:\n{e}")
                    continue                 
        