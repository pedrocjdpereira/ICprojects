import sys
import math
import os.path
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2:
        print_usage()
        return 1

    bins = 1000
    dirall = False
    if sys.argv[1] == "--all":
        if len(sys.argv) < 3:
            print_usage()
            return 1
        dirall = True
        path = sys.argv[2]
    else:
        path = sys.argv[1]

    if ((len(sys.argv) > 2 and not dirall) or (len(sys.argv) > 3) and dirall) and sys.argv[-1].isdigit():
        bins = int(sys.argv[-1])

    if dirall:
        if not (os.path.exists(path) and os.path.isdir(path)):
            print("ERROR: Invalid directory")
            print_usage()
            return 1

        # Add '/' to end of path in case it does not exist
        if path[-1] != '/': path += '/'

        # Get list of '.dat' files
        dir_list = sorted([ (path+d) for d in os.listdir(path) if d.split('.')[-1] == 'dat' ])

        # number of plot rows and columns
        n = round(math.sqrt(len(dir_list)) + 0.5)
        figure, axis = plt.subplots(n,n)

        x,y = 0,0
        for fpath in dir_list:
            name = fpath.split("/")[-1].split(".")[0]
            data = get_data_from_file(fpath)
            # Initialize each one of the plots
            print("Plotting histogram " + name + "...")
            axis[x%n,y%n].hist(data,bins=bins)
            axis[x%n,y%n].set_title(name + " with " + str(bins) + " bins")

            x+=1
            if x%n==0: y+=1

        plt.show()
        return 0
                

    if not (os.path.exists(path) and os.path.isfile(path)):
        print("ERROR: Invalid input file")
        print_usage()
        return 1

    name = path.split("/")[-1].split(".")[0]
    data = get_data_from_file(path)
    
    figure, axis = plt.subplots(1,1)
    print("Plotting histogram " + name + "...")
    axis.hist(data,bins=bins)
    axis.set_title(name + " with " + str(bins) + " bins")
    
    plt.show()
    return 0

### Auxiliary functions ###
# Store data from file in an array
#   This is technically reversing what was done by the previous program
#   However it is the fastest way found to plot the graph using matplotlib.
#   Storing the histogram in a dict and then plot it with a bar graph takes too
#   much time because we dont have an available variable "bins" to reduce its size
#   like in an histogram.
def get_data_from_file(path):
    data = []
    with open(path) as f:
        lines = f.readlines()
        for line in lines:
            value, counter = line.replace("\n", "").split("," , maxsplit=1)
            [data.append(int(value)) for _ in range(int(counter))]
    return data

def print_usage():
    print("USAGE: python3 " + sys.argv[0] + " <histogram file> [<number of bins>]")
    print("       python3 " + sys.argv[0] + " --all <histogram directory> [<number of bins>]")
    print("\nINFO: When plotting from a directory, only files with the '.dat' extension will be read.")
    print("INFO: bins are the name given to the towers or bars in an histogram. Default is 1000.")


if __name__ == "__main__":
    exit(main())
