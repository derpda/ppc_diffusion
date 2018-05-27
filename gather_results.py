import sys
import os

def main():
    directory = './output_files/' + sys.argv[1] + '/'
    with open('results/' + sys.argv[1], "w") as output:
        for filename in os.listdir(directory):
            with open(directory + filename, "r") as handle:
                if(handle.readline() == ""):
                    continue
                gflops = handle.readline().split(" ")[1]
            output.write('{}\t{}\n'.format(filename, gflops))


if __name__ == "__main__":
    main()
