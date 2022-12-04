# Loseless Image Codec
An implementation of a lossless image codec, based on Golomb coding of the prediction residuals.

A main class was developed, LosslessImageCodec, which has 2 modes, 'e' for encoding, and 'd' for decoding.
Each mode has a method supporting its functionality.

## To Build
To build the project run:
```
make
```

## To Test
To test the functionality of the codec run the following command:
```
../limagecodec-bin/test_codec ../../datasets/imagens\ PPM/airplane.ppm
```

Where the argument can be a .ppm file to your desire.
