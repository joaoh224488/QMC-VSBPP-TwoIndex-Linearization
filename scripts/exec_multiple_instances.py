import os
import subprocess
import pandas as pd

def main():
    p_e = './program'
    p_f_i = 'instances/'
    p_p = 'auxiliar/instances_info.csv'
    i_l_i = 4
    s_l_i = 96

    execute_multiple_instances(p_e, p_f_i, p_p, i_l_i, s_l_i)

def execute_multiple_instances(path_executable, path_folder_instances, path_params, inferior_limit_instances, superior_limit_instances):
    if inferior_limit_instances > superior_limit_instances:
        print("The inferior limit of instances must be less than or equal to the superior limit.")
        return
    
    data = pd.read_csv(path_params)
    instances_upper_bound = data['cost'].tolist()
    instances_num_bins = data['num_bins'].tolist()

    with os.scandir(path_folder_instances) as entries:
        files = sorted(entries, key=lambda e: (int(e.name.split('_')[1].split('=')[1]), int(e.name.split('_')[2].split('=')[1]), int(e.name.split('_')[3].split('=')[1]), int(e.name.split('_')[0].split('=')[1])))

        for i, entry in enumerate(files):
            if i < inferior_limit_instances:
                continue
            if i > superior_limit_instances:
                break
            print(f"Executing instance: {entry.name}")
            subprocess.run([path_executable, entry.path, str(instances_upper_bound[int(entry.name.split('_')[0].split('=')[1])]), str(instances_num_bins[int(entry.name.split('_')[0].split('=')[1])])])

            with open('out/instances_executed.txt', 'a') as f:
                f.write(f'Instance: {entry.name}\n')

if __name__ == "__main__":
    main()