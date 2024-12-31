import os
import shutil
import subprocess
import csv
import re
import statistics
# Specify the parent folder containing the benchmarks and build subfolders
benchmarks_folder = "benchmarks"
build_folder = os.path.join("build", "benchmarks")
# to be run after with slot_count = 8 for both matrix_mul and rober_cross
output_csv = "results.csv"
# vectorization_csv = "vectorization.csv"
operations = ["add", "sub", "multiply_plain", "rotate_rows", "negate", "multiply"]
infos = ["benchmark"]
additional_infos =[ "Depth", "Multplicative Depth","compile_time( ms )", "execution_time (ms)"]
infos.extend(operations)
infos.extend(additional_infos)

with open(output_csv, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(infos)

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
    print(f"Command failed with error:\n{e.stderr.decode('utf-8')}")    

# Iterate through each item in the benchmarks folder
# "hamming_dist","poly_reg","lin_reg","l2_distance","dot_product","box_blur"
# "box_blur","gx_kernel","gy_kernel","sobel","roberts_cross","matrix_mul"
benchmark_folders = ["hamming_dist","poly_reg","lin_reg","l2_distance","dot_product","box_blur","gx_kernel","gy_kernel","roberts_cross","matrix_mul","sobel"] 
#benchmark_folders = ["gx_kernel","gy_kernel","roberts_cross","sobel"]
#benchmark_folders = ["dot_product"]
###############################
### specify the number of iteration  
iterations = 1
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
        slot_counts= [8]
        window_size = 0
        for slot_count in slot_counts :
            print("****************************************************************")
            print(f"*****run {subfolder_name} , for slot_count : {slot_count}******")
            operation_stats = {
            "add": [], "sub": [], "multiply_plain": [], "rotate_rows": [],
            "negate": [], "multiply": [], "Depth": [], "Multiplicative Depth": [],
            "compile_time (ms)": [], "execution_time (ms)": []
            }
            ###generate io_file for benchmark with slot_count 
            pro = subprocess.Popen(['python3', 'generate_{}.py'.format(subfolder_name),'--slot_count',str(slot_count)],cwd=build_path)
            pro.wait()
            ######################################
            for iteration in range(iterations):
                print(f"===> Running iteration : {iteration + 1}")
                # Step 1: Run the first benchmark command
                command = f"./{subfolder_name} 1 {window_size} 1 1 {slot_count}"
                try:
                    result = subprocess.run(
                        command, shell=True, check=True, 
                        stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE, 
                        universal_newlines=True, 
                        cwd=build_path
                    )
                    lines = result.stdout.splitlines()

                    # Collect compile time (ms)
                    for line in lines: 
                        if 'ms' in line:
                            optimization_time = float(line.split()[0])
                            operation_stats["compile_time (ms)"].append(optimization_time)
                            break

                    # Collect depth and multiplicative depth
                    #print(result.stdout)
                    depth_match = re.search(r'max:\s*\((\d+),\s*(\d+)\)', result.stdout)
                    #print(depth_match)
                    depth = int(depth_match.group(1)) if depth_match else None
                    multiplicative_depth = int(depth_match.group(2)) if depth_match else None
                    print(f"Depth=>{depth}, multiplcative_depth=>{multiplicative_depth}")
                    operation_stats["Depth"].append(depth)
                    operation_stats["Multiplicative Depth"].append(multiplicative_depth)

                except subprocess.CalledProcessError as e:
                    error_message = e.stderr.decode('utf-8') if e.stderr else "No error message available."
                    print("Command for {} failed with error:\n{}".format(subfolder_name, error_message))
                    continue

                # Step 2: Build and run fhe code
                build_path_he = os.path.join(build_path, "he")
                try:
                    result = subprocess.run(['cmake', '-S', '.', '-B', 'build'], 
                        cwd=build_path_he,
                        check=True, 
                        stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE, 
                        universal_newlines=True)
                    result =subprocess.run(['cmake', '--build', 'build'], cwd=build_path_he,universal_newlines=True,
                        check=True, 
                        stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE, )

                    build_path_he_build = os.path.join(build_path_he, "build")
                    # Run the compiled program
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
                    lines = result.stdout.splitlines()
                    # Collect execution time (ms)
                    for line in lines:
                        if 'ms' in line:
                            #print(f"==> execution time {line.split()[0]}")
                            execution_time = float(line.split()[0])
                            operation_stats["execution_time (ms)"].append(execution_time)
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
            for key, values in operation_stats.items():
                print(f"{key} {values} {statistics.median(values)}")
                row.append(statistics.mean(values)) if values else None
            #####################################################################
            #######################################################################
            with open(output_csv, mode='a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(row)
        