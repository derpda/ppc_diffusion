import os
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict

class results_dict(defaultdict):

    def get(self, *args):
        data = self["ALL_FILES"]
        for arg in args:
            data = data & self[arg]
        return data

    def get_gflops(self, diff_type, node, n_steps, NX):
        if diff_type is None:
            files = self.get(node, n_steps, NX)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                filename = path.split('/')[-1]
                diff_type = filename.split('.')[0]
                data[diff_type] = gflops
        elif node is None:
            files = self.get(diff_type, n_steps, NX)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                filename = path.split('/')[-1]
                node = filename.split('.')[1]
                data[node] = gflops
        elif n_steps is None:
            files = self.get(diff_type, node, NX)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                filename = path.split('/')[-1]
                n_steps = filename.split('.')[2]
                data[n_steps] = gflops
        elif NX is None:
            files = self.get(diff_type, n_steps, node)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                filename = path.split('/')[-1]
                NX = filename.split('.')[3]
                data[NX] = gflops
        else:
            files = self.get(diff_type, n_steps, node, NX)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                data = gflops
        return data

    def add_from_path(self, path):
        self.path = path
        for filename in os.listdir(path):
            self["ALL_FILES"].add(path+filename)
            diff_type = filename.split('.')[0]
            self[diff_type].add(path+filename)
            node = filename.split('.')[1]
            self[node].add(path+filename)
            n_steps = filename.split('.')[2]
            self["N_STEPS__"+n_steps].add(path+filename)
            nx = filename.split('.')[3]
            self["NX_"+nx].add(path+filename)

def load_files(rel_path):
    data = results_dict(set)
    base_path = os.path.dirname(os.path.realpath(__file__))
    path = base_path + '/' + rel_path + '/'
    data.add_from_path(path)
    return data

def main():

    files = load_files('../output_files')

    for key, value in files.get_gflops("cuda", "h_node", None, "NX_1024").items():
        plt.plot(int(key), float(value))
    plt.show



    return 0

if __name__ == "__main__":
    main()
