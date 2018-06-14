import os
import sys
import matplotlib.pyplot as plt
from collections import defaultdict


class results_dict(defaultdict):
    def get(self, skip, *args):
        files = self["ALL_FILES"].keys()
        for index in range(len(args)):
            arg = args[index]
            if index >= skip:
                index += 1
            files = files & self["ATTR" + str(index) + "_" + arg]
        return files

    def get_gflops(self, *args):
        if None in args:
            none_list = [x for x in args if x is None]
            assert len(none_list) == 1
            index = args.index(None)
            rest_args = [x for x in args if x is not None]

            files = self.get(index, *rest_args)
            data = {}
            for filename in files:
                diff_type = filename.split('.')[index]
                gflops = self["ALL_FILES"][filename]
                data[diff_type] = gflops
        else:
            filename = self.get(*args)[0]
            data = self["ALL_FILES"][filename]
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
                for i in range(len(filename.split('.'))):
                    attr = filename.split('.')[i]
                    self["ATTR" + str(i) + "_" + attr].add(filename)
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

    # This list informs the program of the shape of the input filenames.
    # Setting one of them as None makes it the variable for which to plot the
    # gflops. Modify this when the filename syntax changes
    args = [
        "cuda",  # EXEC
        "f_node",  # NODE
        "5000",  # N_STEPS
        None,  # NX
        "56"  # N_THREADS
    ]

    x_val = []
    y_val = []
    args[0] = "cuda"
    for key, value in files.get_gflops(*args).items():
        x_val.append(int(key))
        y_val.append(float(value))
    ax.semilogx(
        # ax.plot(
        x_val, y_val,
        color='black',
        marker='o',
        linestyle='None',
        basex=2,
        label='Single-stream'
    )
    x_val = []
    y_val = []

    files = load(sys.argv[2])
    args[0] = "cuda_split"
    for key, value in files.get_gflops(*args).items():
        x_val.append(int(key))
        y_val.append(float(value))
    ax.semilogx(
        # ax.plot(
        x_val, y_val,
        color='red',
        marker='o',
        linestyle='None',
        basex=2,
        label='Dual-stream'
    )

    ax.legend()
    ax.grid()
    # ax.set_ylim([0, 25])
    ax.set(
        xlabel='N of square N by N matrix',
        ylabel='GFlops'
    )

    fig.tight_layout()
    fig.savefig("../assignment_3/tex_subfiles/cuda_single_vs_dual.pdf")
    # plt.show()
    return 0


if __name__ == "__main__":
    main()
