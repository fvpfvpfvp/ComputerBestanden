#include <fstream>
#include <iostream>

using namespace std;

int main(){

	ofstream file("out.bin", ios::out | ios::binary | ios::trunc);

	unsigned char data[2048];

	for(int mode = 0; mode < 2; mode ++){

		for(int value = 0; value < 256; value ++){

			int currentValue = value;
			int values[4];
			for(int i = 3; i >= 0; i --){

				values[i] = currentValue % 10;
				currentValue = (currentValue - (currentValue % 10)) / 10;
			}

			for(int digit = 0; digit < 4; digit++){

				//0 1 2 3 4 5 6 7
				//b c g e d p a f
				unsigned char digitValue = 0;
				if (values[digit] == 0) digitValue = 0b11011011; //a b c d e f
				else if (values[digit] == 1) digitValue = 0b00000011; //b c
				else if (values[digit] == 2) digitValue = 0b01011101; //a b d e g
				else if (values[digit] == 3) digitValue = 0b01010111; //a b c d g
				else if (values[digit] == 4) digitValue = 0b10000111; //b c f g
				else if (values[digit] == 5) digitValue = 0b11010110; //a c d f g 
				else if (values[digit] == 6) digitValue = 0b11011110; //a c d e f g
				else if (values[digit] == 7) digitValue = 0b01000011; //a b c
				else if (values[digit] == 8) digitValue = 0b11011111; //a b c d e f g
				else if (values[digit] == 9) digitValue = 0b11010111; //a b c d f g

				data[(mode << 10) | (digit << 8) | (value << 0)] = digitValue;
			}
		}
	}

	file.write((const char*)data, 2048);
	file.close();
}