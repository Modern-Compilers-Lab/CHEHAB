# polynomials_coyote
import os
import shutil
import subprocess
import csv
import re
import statistics

# Specify the parent folder containing the benchmarks and build subfolders
benchmarks_folder = "benchmarks"
build_folder = os.path.join("build", "benchmarks")
output_csv = "polynomials_results.csv" 
operations = ["add", "sub", "multiply_plain", "rotate_rows", "square", "multiply"]
infos = ["benchmark"]
additional_infos =[ "Depth", "Multplicative Depth","compile_time( ms )", "execution_time (ms)"]
infos.extend(operations)
infos.extend(additional_infos)
with open(output_csv, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(infos)
    
""" with open(vectorization_csv, mode = 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Benchmark","Vecorization Time"])     """
    
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

# Iterate through each item in the benchmarks folder
polynomial_folders = ["polynomials_coyote"]
for subfolder_name in polynomial_folders: 
    benchmark_path = os.path.join(benchmarks_folder, subfolder_name)
    build_path = os.path.join(build_folder, subfolder_name)
    # build_path = build/benchmarks/dot_product 
    ## informations to collect 
    depths = [5, 10]
    regimes = ["50-50", "100-50", "100-100"]
    iterations = 1
    for regime in regimes :
        for tree_depth in depths : 
            operation_stats = {
            "add": [0], "sub": [0], "multiply_plain": [0], "rotate_rows": [0],
            "square": [0], "multiply": [0], "Depth": [0], "Multiplicative Depth": [0],
            "compile_time (ms)": [0], "execution_time (ms)": [0]
            }
            benchmark_name = f'tree_{regime}-{tree_depth}'
            print(f"Benchmark '{benchmark_name}' will be runned...")
            for i in range(iterations):
                optimization_time=""
                execution_time=""
                depth = ""
                multiplicative_depth = ""
                if os.path.isdir(build_path):
                    # Paths for the .cpp files
                    #benchmark_cpp_path = os.path.join(benchmark_path, f"{subfolder_name}.cpp")
                    #backup_cpp_path = os.path.join(benchmark_path, "backup.cpp")
                    #fhe_generated_cpp_path = os.path.join(build_path, "fhe_vectorized.cpp")
                    print(f"=========> Iteration : {i+1}")
                    # Step 1: Construct the command to run `.\\benchmark` in the subfolder
                    command = f"./{subfolder_name} {tree_depth} {i+1} {regime} "
                    # # Set the current working directory to the subfolder
                    try: 
                        result = subprocess.run(
                            command, shell=True, check=True, 
                            stdout=subprocess.PIPE, 
                            stderr=subprocess.PIPE, 
                            universal_newlines=True, 
                            cwd=build_path
                        )
                        lines = result.stdout.splitlines()
                        for line in lines:
                            if 'ms' in line:
                                optimization_time = line.split()[0]
                                operation_stats["compile_time (ms)"].append(float(optimization_time))
                                break
                        depth_match = re.search(r'max:\s*\((\d+),\s*(\d+)\)', result.stdout)
                        #print(f"\n\n {depth_match} \n\n")
                        depth = depth_match.group(1) if depth_match else None
                        multiplicative_depth = depth_match.group(2) if depth_match else None
                        operation_stats["Depth"].append(int(depth))
                        operation_stats["Multiplicative Depth"].append(int(multiplicative_depth))
                        print(f"Depth: {depth} --MultipliDepth {multiplicative_depth}")
                    except subprocess.CalledProcessError as e:
                        print(f"Command for {subfolder_name} failed with error:\n{e.stderr}")
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
                            if 'ms' in line:
                                execution_time = line.split()[0]
                                operation_stats["execution_time (ms)"].append(float(execution_time))
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
            for key, values in operation_stats.items():
                row.append(int(statistics.mean(values))) if values else None
            with open(output_csv, mode='a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(row)       
                