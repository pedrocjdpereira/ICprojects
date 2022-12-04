# Golomb
A C++ implementation for golomb coding. It supports encoding of integers, positive or negative, and decoding of appropriate strings.

## Relevant functions
Encode an integer, returning the corresponding string of bits.
```
string Golomb::encoder(int num);
```

Decode a string of bits, returning the corresponding integer.
```
int Golomb::decoder(string bits);
```

Set the value of **m**.
```
void Golomb::setM(unsigned m);
```
