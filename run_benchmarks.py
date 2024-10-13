import os
import shutil
import subprocess
import csv
import re

# Specify the parent folder containing the benchmarks and build subfolders
benchmarks_folder = "benchmarks"
build_folder = os.path.join("build", "benchmarks")
slot_count = 64
output_csv = format("results_{}.csv", str(slot_count))
# vectorization_csv = "vectorization.csv"
operations = ["add", "sub", "multiply_plain", "rotate_rows", "square", "multiply", "Depth", "Multplicative Depth"]
infos = ["benchmark", "compile_time( ms )", "execution_time (ms)"]
infos.extend(operations)

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
benchmark_folders = ["box_blur", "gx_kernel", "l2_distance", "lin_reg", "poly_reg", "matrix_mul"] 
for subfolder_name in benchmark_folders:
    benchmark_path = os.path.join(benchmarks_folder, subfolder_name)
    build_path = os.path.join(build_folder, subfolder_name)
    
    optimization_time = ""
    execution_time = ""
    depth = ""
    multiplicative_depth = ""

    if os.path.isdir(build_path):
        command = f"./{subfolder_name} 0 0 1 1 {slot_count}"
        print(f"Benchmark '{subfolder_name}' will be run...")

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
                    break
            depth_match = re.search(r'max:\s*\((\d+),\s*(\d+)\)', result.stdout)
            depth = depth_match.group(1) if depth_match else None
            multiplicative_depth = depth_match.group(2) if depth_match else None
            print(f"\n {depth} -- {multiplicative_depth} \n")
        except subprocess.CalledProcessError as e:
            print(f"Command for {subfolder_name} failed with error:\n{e.stderr.decode('utf-8')}")

        print("Running fhe code \n")
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
        except subprocess.CalledProcessError as e:
            print(f"Failed in building fhe_code for benchmark: {subfolder_name} with error \n{e.stderr.decode('utf-8')}")

        build_path_he_build = os.path.join(build_path_he, "build")
        
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
            for line in lines:
                if 'ms' in line:
                    execution_time = line.split()[0]
                    break

            file_name = build_path_he + "/_gen_he_fhe.cpp"
            row = [subfolder_name, optimization_time, execution_time]

            with open(file_name, "r") as file:
                file_content = file.read()
                for op in operations:
                    nb_occurrences = len(re.findall(rf'\b{op}', file_content))
                    row.append(nb_occurrences)

            row.append(depth)
            row.append(multiplicative_depth)

            with open(output_csv, mode='a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(row)

        except subprocess.CalledProcessError as e:
            print(f"Command for {subfolder_name} failed with error:\n{e.stderr.decode('utf-8')}")
