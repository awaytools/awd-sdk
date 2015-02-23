#include "Stdafx.h"

using namespace IceCore;

// Use one or the other
//#define SORTER	RadixSort
#define SORTER	RadixSort3

int main(int argc, char* argv[])
{
	// Sort positive/negative floats
	if(1)
	{
		udword Nb = 22;
		float* InputValues = new float[Nb];

		// Get random floating-point values
		for(udword i=0;i<Nb;i++)
		{
			InputValues[i] = float(rand()-16384);
		}

		// Force some negative zeros
		const int Binary = 0x80000000;
		InputValues[2] = *(float*)&Binary;
		InputValues[5] = *(float*)&Binary;
		InputValues[7] = *(float*)&Binary;
		InputValues[13] = *(float*)&Binary;

		// Create sorter
		SORTER RS;

		// Go for it
		const udword* Sorted = RS.Sort(InputValues, Nb).GetRanks();

		// Print results
		for(udword i=0;i<Nb;i++)	printf("%f\n", InputValues[Sorted[i]]);

		DELETEARRAY(InputValues);
	}

	// Multiple sort keys
	if(0)
	{
		udword n = 1000;
		udword* input = new udword[n];
		udword* input2 = new udword[n];
		udword* input3 = new udword[n];

		// Get multiple random values
		for(udword i=0;i<n;i++)
		{
			input[i] = rand()&7;
			input2[i] = (rand()&31) - 16;
			input3[i] = rand();
		}

		// Create sorter
		SORTER Core;

		// Multiple sorts in one line!
		const udword* sorted = Core.Sort(input3, n).Sort(input2, n).Sort(input, n).GetRanks();

		// Print results
		FILE* fp = fopen("c:\\radix.log", "w");
		for(udword i=0;i<n;i++) fprintf(fp, "%d  %d  %d\n", input[sorted[i]], input2[sorted[i]], input3[sorted[i]]);
		fclose(fp);

		DELETEARRAY(input3);
		DELETEARRAY(input2);
		DELETEARRAY(input);
	}

	return 0;
}
