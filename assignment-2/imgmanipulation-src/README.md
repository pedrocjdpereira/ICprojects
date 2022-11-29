# Image Manipulation

## To Build
To build the project run:
```
make
```

## To Test
Exercise 1) - To test img_cpy run:
```
../imgmanipulation-bin/img_cpy ../../datasets/imagens\ PPM/airplane.ppm airplane_copy.ppm
```

Exercise 2.a) - To test img_eff negative run:
```
../imgmanipulation-bin/img_eff -n ../../datasets/imagens\ PPM/airplane.ppm airplane_negative.ppm
```

Exercise 2.b.a) - To test img_eff mirrored horizontaly run:
```
../imgmanipulation-bin/img_eff -mh ../../datasets/imagens\ PPM/airplane.ppm airplane_mirrored_horizontal.ppm
```

Exercise 2.b.b) - To test img_eff mirrored vertically run:
```
../imgmanipulation-bin/img_eff -mv ../../datasets/imagens\ PPM/airplane.ppm airplane_mirrored_vertical.ppm
```

Exercise 2.c)  - To test img_eff rotation run (90 degrees in the example):
```
../imgmanipulation-bin/img_eff -r 90 ../../datasets/imagens\ PPM/airplane.ppm airplane_rotation.ppm
```

Exercise 2.d) - To test img_eff increase/decrease intensity run (beta equals -30 in the example):
```
../imgmanipulation-bin/img_eff -i -30 ../../datasets/imagens\ PPM/airplane.ppm airplane_intensity.ppm
```

## Extra info
### img_eff
This program accepts multiple flags at once, so it is possible to apply multiple effects at once.
The degree on the rotation can be either negative or positive and must be a multiple of 90 (degrees).
The beta value on the intensity must be between -100 and 100 (inclusive).
The program assumes that the values passed as arguments are sanitized and will throw an error if they are invalid.
