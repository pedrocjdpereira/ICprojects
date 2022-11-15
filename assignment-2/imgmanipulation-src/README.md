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
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_negative.ppm 0
```

Exercise 2.b.a) - To test img_eff mirrored horizontaly run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_mirrored_horizontal.ppm 1
```

Exercise 2.b.b) - To test img_eff mirrored vertically run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_mirrored_vertical.ppm 2
```

Exercise 2.c)  - To test img_eff rotation run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_rotation.ppm 3
```

Exercise 2.d) - To test img_eff increase/decrease intensity run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_intensity.ppm 4
```
