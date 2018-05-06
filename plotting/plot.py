import os
import matplotlib.pyplot as plt
from collections import defaultdict


class results_dict(defaultdict):
    def get(self, *args):
        data = self["ALL_FILES"]
        for arg in args:
            data = data & self[arg]
        return data

    def get_gflops(self, diff_type, node, n_steps, NX, n_threads):
        if diff_type is None:
            files = self.get(node, n_steps, NX, n_threads)
            data = {}
            for path in files:
                filename = path.split('/')[-1]
                diff_type = filename.split('.')[0]
                with open(path, "r") as handle:
                    while(handle.readline() != ""):
                        gflops = handle.readline().split(" ")[1]
                        data[diff_type] = gflops
        elif node is None:
            files = self.get(diff_type, n_steps, NX, n_threads)
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
            files = self.get(diff_type, node, NX, n_threads)
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
            files = self.get(diff_type, n_steps, node, n_threads)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                filename = path.split('/')[-1]
                NX = filename.split('.')[3]
                data[NX] = gflops
        elif n_threads is None:
            files = self.get(diff_type, n_steps, node, NX)
            data = {}
            for path in files:
                with open(path, "r") as handle:
                    if(handle.readline() == ""):
                        continue
                    gflops = handle.readline().split(" ")[1]
                filename = path.split('/')[-1]
                if len(filename.split('.')) == 4:
                    if node == 'h_node':
                        n_threads = '28'
                    elif node == 'f_node':
                        n_threads = '56'
                    else:
                        n_threads = '1'
                else:
                    n_threads = filename.split('.')[4]
                data[n_threads] = gflops
        else:
            files = self.get(diff_type, n_steps, node, NX, n_threads)
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
            self["N_STEPS_"+n_steps].add(path+filename)
            nx = filename.split('.')[3]
            self["NX_"+nx].add(path+filename)
            if len(filename.split('.')) == 4:
                if node == 'h_node':
                    n_threads = '28'
                elif node == 'f_node':
                    n_threads = '56'
                else:
                    n_threads = '1'
            else:
                n_threads = filename.split('.')[4]
            self["N_THREADS_"+n_threads].add(path+filename)


def load_files(rel_path):
    data = results_dict(set)
    base_path = os.path.dirname(os.path.realpath(__file__))
    path = base_path + '/' + rel_path + '/'
    data.add_from_path(path)
    return data


def main():
    files = load_files('../output_files')

    fig, ax = plt.subplots()

    x_val = []
    y_val = []
    for key, value in files.get_gflops(
            "omp", "f_node",
            "N_STEPS_1000", "NX_32768", None).items():
        x_val.append(int(key))
        y_val.append(float(value))
    # ax.semilogx(x_val, y_val, 'ro', basex=10, label='OpenMP')
    ax.plot(x_val, y_val, 'ro', label='OpenMP')

    x_val = []
    y_val = []
    for key, value in files.get_gflops(
            "plain", "f_node",
            None, "NX_4096", "N_THREADS_1").items():
        x_val.append(int(key))
        y_val.append(float(value))
    # ax.semilogx(x_val, y_val, 'bo', basex=10, label='Unmodified Code')

    ax.legend()
    ax.grid()
    ax.set(
        xlabel='Number of threads',
        ylabel='GFlops'
    )

    fig.tight_layout()
    fig.savefig("../assignment_1/tex_subfiles/omp_threads_long.pdf")
    plt.show()
    return 0


if __name__ == "__main__":
    main()
