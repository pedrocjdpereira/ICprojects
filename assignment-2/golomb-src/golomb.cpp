#include "golomb.hpp"
#include <string>
#include <math.h>

using namespace std;

Golomb::Golomb(){}

Golomb::Golomb(unsigned m) {
	// TODO: if m == 0 throw error
	this->m = m;
	this->b = (unsigned)ceil(log2(m));
}

string Golomb::encoder(int num) {
	// TODO: verify if m is set

	this->i = num;
	/* Enable support for negative numbers */
	if(num < 0) num = 2*(-num) - 1;
	else num *= 2;

	this->q = num / this->m;
	this->r = num - this->q * this->m;

	/* Get the unary represenation of q */
	unsigned q_una = uns2una(this->q);

	/* Get the truncated binary representation of r */	
	unsigned r_truncBin, r_numBits;
	if( this->r < (pow(2,this->b) - m) ) {
		r_truncBin = uns2bin(this->r);
		r_numBits = this->b - 1;
	} 
	else {
		r_truncBin = uns2bin(this->r + pow(2,this->b) - this->m);
		r_numBits = this->b;
	}

	/* Obtain final representation of num */
	string res = to_string(r_truncBin);
	if(res.length() < r_numBits) res = '0' + res;
	res = to_string(q_una) + res;
	return res;
}

int Golomb::decoder(string bits) {
	// TODO: verify if m is set

	/* Get value of q */
	this->q = bits.find('0');

	/* Get truncated binary representation of r */
	int r_truncBin = stoi(bits.substr(this->q + 1));

	/* Calculate the value of r */
	this->r = bin2uns(r_truncBin);
	if( this->r >= (pow(2,this->b) - this->m) )
		this->r -= (pow(2,this->b) - this->m);
	
	/* Calculate the original value */
	int res = this->q * this->m + this->r;
	if(res % 2 != 0) res = -(res+1)/2;
	else res /= 2;
	return res;
}

void Golomb::setM(unsigned m) {
	// TODO: if m == 0 throw error
	this->m = m;
	this->b = (unsigned)ceil(log2(m));
}

unsigned Golomb::uns2bin(unsigned num) {
	if(num==0) return 0;
	if(num==1) return 1;
	return (num % 2) + 10 * uns2bin(num / 2);
}

unsigned Golomb::uns2una(unsigned num) {
	unsigned res = 0;
	for(unsigned i = 1; i <= num; i++) res += pow(10,i);
	return res;
}

unsigned Golomb::bin2uns(unsigned bin) {
	unsigned digit, res=0, tmp=bin;
	for (unsigned i = 0; tmp > 0; i++) {
		digit = tmp%10;
		res += digit*pow(2,i);
		tmp /= 10;
	}
	return res;
}

unsigned Golomb::una2uns(unsigned una) {
	if(una == 0) return 0;
	unsigned tmp = 10, res = 1;
	while(una > tmp) {
		res++;
		tmp += pow(10,res);
	}
	return res;
}
