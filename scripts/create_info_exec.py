import os
import pandas as pd

def create_info_csv(path_folder_benchmark, path_output_csv):
    instance_best = {}
    
    with os.scandir(path_folder_benchmark) as entries:
        for entry in entries:
            if not entry.is_file():
                continue
            
            try:
                with open(entry.path, 'r') as f:
                    content = f.read().split('\n')
                    
                    instance_id = entry.name.split('=')[1].split('_')[0]
                    
                    cost = int(content[2].split(':')[1].strip())
                    bins_str = content[1].split(':')[1].strip()
                    num_bins = len([x for x in bins_str.split(',') if x.strip()])
                    
                    if instance_id not in instance_best or cost < instance_best[instance_id]['cost']:
                        instance_best[instance_id] = {
                            'cost': cost,
                            'num_bins': num_bins,
                            'filename': entry.name
                        }
            except (IndexError, ValueError, KeyError) as e:
                print(f"Warning: Failed to parse {entry.name}: {e}")
    
    # Sort by instance ID and build output
    data = {
        'instance_name': [],
        'cost': [],
        'num_bins': []
    }
    
    for instance_id in sorted(instance_best.keys(), key=lambda x: int(x)):
        data['instance_name'].append(f"instance={instance_id}")
        data['cost'].append(instance_best[instance_id]['cost'])
        data['num_bins'].append(instance_best[instance_id]['num_bins'])
    
    df = pd.DataFrame(data)
    df.to_csv(path_output_csv, index=False)
    print(f"Info CSV created at: {path_output_csv}")
    print(f"Processed {len(instance_best)} instances")

def main():
    path_folder_benchmark = 'auxiliar/LNS_NewRemovals-25M/'
    path_output_csv = 'auxiliar/instances_info.csv'

    create_info_csv(path_folder_benchmark, path_output_csv)

if __name__ == '__main__':
    main()