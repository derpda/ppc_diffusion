import os
import sys
import matplotlib.pyplot as plt
from collections import defaultdict


class results_dict(defaultdict):
    def get(self, *args):
        files = self["ALL_FILES"].keys()
        for arg in args:
            files = files & self[arg]
        return files

    def get_gflops(self, *args):
        if None in args:
            none_list = [x for x in args if x is None]
            assert len(none_list) == 1
            index = args.index(None)
            rest_args = [x for x in args if x is not None]

            files = self.get(*rest_args)
            data = {}
            for filename in files:
                diff_type = filename.split('.')[index]
                gflops = self["ALL_FILES"][filename]
                data[diff_type] = gflops
        return data

    def add_from_file(self, path):
        self.path = path
        self["ALL_FILES"] = {}
        with open(path, "r") as handle:
            content = handle.readline()
            while (content != ""):
                filename = content.split('\t')[0]
                gflops = content.split('\t')[1].strip()
                self["ALL_FILES"][filename] = gflops
                diff_type = filename.split('.')[0]
                self[diff_type].add(filename)
                node = filename.split('.')[1]
                self[node].add(filename)
                n_steps = filename.split('.')[2]
                self["N_STEPS_"+n_steps].add(filename)
                nx = filename.split('.')[3]
                self["NX_"+nx].add(filename)
                if len(filename.split('.')) == 4:
                    if node == 'h_node':
                        n_threads = '28'
                    elif node == 'f_node':
                        n_threads = '56'
                    else:
                        n_threads = '1'
                else:
                    n_threads = filename.split('.')[4]
                self["N_THREADS_"+n_threads].add(filename)
                content = handle.readline()


def load(rel_path):
    data = results_dict(set)
    base_path = os.path.dirname(os.path.realpath(__file__))
    path = base_path + '/' + rel_path
    data.add_from_file(path)
    return data


def main():
    files = load(sys.argv[1])

    fig, ax = plt.subplots()

    N_STEPS = None  # "N_STEPS_2000"
    NX = "NX_8192"
    N_THREADS = "N_THREADS_56"

    x_val = []
    y_val = []
    for key, value in files.get_gflops(
            "3d_omp", "f_node",
            N_STEPS,
            NX,
            N_THREADS
    ).items():
        x_val.append(int(key))
        y_val.append(float(value))
    # ax.semilogx(
    ax.plot(
        x_val, y_val,
        color='black',
        marker='o',
        linestyle='None',
        # basex=2,
        label='3D OpenMP'
    )
    x_val = []
    y_val = []

    for key, value in files.get_gflops(
            "1d_omp", "f_node",
            N_STEPS,
            NX,
            N_THREADS
    ).items():
        x_val.append(int(key))
        y_val.append(float(value))
    # ax.semilogx(
    ax.plot(
        x_val, y_val,
        color='red',
        marker='o',
        linestyle='None',
        # basex=2,
        label='1D OpenMP'
    )

    ax.legend()
    ax.grid()
    ax.set_ylim([0, 25])
    ax.set(
        xlabel='Amount of time steps',
        ylabel='GFlops'
    )

    fig.tight_layout()
    # fig.savefig("../assignment_1/tex_subfiles/omp_long.pdf")
    plt.show()
    return 0


if __name__ == "__main__":
    main()
