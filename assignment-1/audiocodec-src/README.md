To build:
    make

To test:
    
    ../audiocodec-bin/audiocodec -e ../sndfile-example-src/sample.wav out.bin // Generates a new coded file based on a wav file
    
    ../audiocodec-bin/audiocodec -d out.bin out.wav // generates a wav file based on a previously generated coded file
