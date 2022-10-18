#!/bin/bash

SCRIPTDIR=$(dirname "$0")
BITS_TO_REDUCE=$1

if [ -z "$BITS_TO_REDUCE" ]; then
    echo "USAGE: $0 <number of bits to reduce>"
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

echo "Running wav_quant..."
mkdir -p ../output/
../sndfile-example-bin/wav_quant sample.wav ../output/ex3-quant-"$BITS_TO_REDUCE"bits.wav "$BITS_TO_REDUCE" > /dev/null
if [ $? -ne 0 ]; then
    echo "Running failed"
    exit 1
fi
