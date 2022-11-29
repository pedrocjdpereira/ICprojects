#include <string>
#include <math.h>
#include "golomb.hpp"

using namespace std;

Golomb::Golomb(){}

Golomb::Golomb(unsigned m) {
	/* Verify if m > 0 */
	if(m <= 0) throw invalid_argument("m must be greater than 0!");

	this->m = m;
	this->b = (unsigned)ceil(log2(m));
}

string Golomb::encoder(int num) {
	/* Check if m is initialized */
	if(m<=0) throw runtime_error("m is not initialized!");

	this->i = num;
	/* Enable support for negative numbers */
	if(num < 0) num = 2*(-num) - 1;
	else num *= 2;

	this->q = num / this->m;
	this->r = num - this->q * this->m;

	/* Get the unary represenation of q */
	string q_una = uns2una(this->q);

	/* Get the truncated binary representation of r */	
	string r_truncBin;
	unsigned r_numBits;
	if( this->r < (pow(2,this->b) - m) ) {
		r_truncBin = uns2bin(this->r);
		r_numBits = this->b - 1;
	} 
	else {
		r_truncBin = uns2bin(this->r + pow(2,this->b) - this->m);
		r_numBits = this->b;
	}

	/* Obtain final representation of num */
	string res = r_truncBin;
	if(res.length() < r_numBits) res = '0' + res;
	res = q_una + res;
	return res;
}

int Golomb::decoder(string bits) {
	/* Check if m is initialized */
	if(m<=0) throw runtime_error("m is not initialized!");

	/* Get value of q */
	this->q = bits.find('0');

	/* Get truncated binary representation of r */
	string r_truncBin = bits.substr(this->q + 1);

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
	/* Verify if m > 0 */
	if(m <= 0) throw invalid_argument("m must be greater than 0!");

	this->m = m;
	this->b = (unsigned)ceil(log2(m));
}

string Golomb::uns2bin(unsigned num) {
	/*
	if(num==0) return 0;
	if(num==1) return 1;
	return (num % 2) + 10 * uns2bin(num / 2);
	*/

	if(num==0) return "0";
	if(num==1) return "1";
	return uns2bin(num / 2) + to_string(num % 2);
}

string Golomb::uns2una(unsigned num) {
	/*
	unsigned res = 0;
	for(unsigned i = 1; i <= num; i++) res += pow(10,i);
	return res;
	*/

	string res = "0";
	for(unsigned i = 1; i <= num; i++) res = '1' + res;
	return res;
}

unsigned Golomb::bin2uns(string bin) {
	/*
	unsigned digit, res=0, tmp=bin;
	for (unsigned i = 0; tmp > 0; i++) {
		digit = tmp%10;
		res += digit*pow(2,i);
		tmp /= 10;
	}
	return res;
	*/

	unsigned res=0, len=int(bin.length());
	for(int i=0; i < len; i++) {
		res += int(bin[len-1-i] - '0') * pow(2,i);
	}
	return res;
}

unsigned Golomb::una2uns(string una) {
	/*
	if(una == 0) return 0;
	unsigned tmp = 10, res = 1;
	while(una > tmp) {
		res++;
		tmp += pow(10,res);
	}
	return res;
	*/
	return una.length() - 1;
}
