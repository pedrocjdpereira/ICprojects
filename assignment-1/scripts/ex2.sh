#!/bin/bash
# Exercise 2 script:
#   This script will build the project, and test
#   the functionalities of the wav_hist program. 

SCRIPTDIR=$(dirname "$0")
VARBINS=$1

if [ -z "$VARBINS" ]; then
    echo "USAGE: $0 <number of bins>"
    exit 1
fi

cd "$SCRIPTDIR"
cd ../sndfile-example-src/

echo "Building Project..."
make > /dev/null
if [ $? -ne 0 ]; then
    echo "Building failed!"
    exit 1
fi

echo "Running wav_hist..."
../sndfile-example-bin/wav_hist --store-histograms sample.wav 0 > /dev/null
if [ $? -ne 0 ]; then
    echo "Running failed"
    exit 1
fi

cd ../plots/
python3 generate_plot.py --all histograms/ "$VARBINS" 2> /dev/null

if [ $? -ne 0 ]; then
    echo "Plotting failed"
    exit 1
fi
