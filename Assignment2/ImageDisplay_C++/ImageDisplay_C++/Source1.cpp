#include <iostream>
#include <stdlib.h>
#include <cmath.c>

using namespace std;

void DCT(float** resBlock)
{
	float dctCoeff[8][8];

	float cu = 0;
	float cv = 0;
	dctCoeff = new float*[8];
	for (int i = 0; i < 8; i++) {
		dctCoeff[i] = new float[8];
	}
	for (int u = 0; u < 8; ++u) {
		for (int v = 0; v < 8; ++v) {
			dctCoeff[u][v] = 0;
			float dct = 0;
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					dct += resBlock[x][y] * cos(((2 * x + 1) * u * M_PI) / (float)16) * cos(((2 * y + 1) * v * M_PI) / (float)16);
					/*dctCoeff[u][v] += resBlock[i][j] * cos(M_PI / ((float)8)*(i + 1.0 / 2.0)*u)*cos(M_PI / ((float)8)*(j + 1.0 / 2.0)*v);*/
				}
			}
			if (0 == u && 0 == v) {
				cu = 1 / (float)sqrt(2);
				cv = 1 / (float)sqrt(2);
			}
			else {
				cu = 1;
				cv = 1;
			}
			dctCoeff[u][v] = (1 / (float)4) * cu * cv * dct;
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%d ", dctCoeff);
		}
		printf("\n");
	}
}


int main() {
	float orig[8][8] = { {139, 144, 149, 153, 155, 155, 155, 155},
						{144, 151, 153, 156, 159, 156, 156, 156},
						{150, 155, 160, 163, 158, 156, 156, 156},
						{159, 161, 162, 160, 160, 159, 159, 159},
						{159, 160, 161, 162, 162, 155, 155, 155},
						{161, 161, 161, 161, 160, 157, 157, 157},
						{162, 162, 161, 163, 162, 157, 157, 157},
						{162, 162, 161, 161, 163, 158, 158, 158} };

	DCT(orig);
}