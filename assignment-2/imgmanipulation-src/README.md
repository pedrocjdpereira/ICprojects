# Image Manipulation

## To Build
To build the project run:
```
make
```

## To Test
To test img_cpy run:
```
../imgmanipulation-bin/img_cpy ../../datasets/imagens\ PPM/airplane.ppm airplane_copy.ppm
```

To test img_eff negative run:
```
./imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_negative.ppm 0
```

To test img_eff mirrored horizontaly run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_mirr_hor.ppm 1
```

To test img_eff mirrored vertically run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_mirr_ver.ppm 2
```

To test img_eff rotation run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_rot.ppm 3
```

To test img_eff increase/decrease intensity run:
```
../imgmanipulation-bin/img_eff ../../datasets/imagens\ PPM/airplane.ppm airplane_int.ppm 4
```
