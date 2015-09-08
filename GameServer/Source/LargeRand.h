#ifndef LARGERAND_H
#define LARGERAND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MAX_LARGE_RAND_STATE	625

class CLargeRand
{
public:

	CLargeRand();
	virtual ~CLargeRand();

	void  seedMT(DWORD seed);
	DWORD  reloadMT();
	DWORD  randomMT()
	{
		this->left--;

		if ( this->left< 0 )
		{
			return this->reloadMT();
		}

		DWORD y = *next;
		next++;
		y ^= ( (DWORD)y >> (DWORD)11 );
		y ^= ( y << 7 ) & 2636928640;
		y ^= ( y << 15 ) & 4022730752;

		return y ^ ( y >> 18 );
	};
 
private:
 
	DWORD state[MAX_LARGE_RAND_STATE];	// 4
	DWORD * next;	// 9C8
	int left;	// 9CC
};

DWORD GetLargeRand();

class CRandomMersenne {
// MT19937:
#define MERS_N   624
#define MERS_M   397
#define MERS_R   31
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   18
#define MERS_A   0x9908B0DF
#define MERS_B   0x9D2C5680
#define MERS_C   0xEFC60000
#endif

public:
   CRandomMersenne(int seed) {         // Constructor
      RandomInit(seed); LastInterval = 0;}
   void RandomInit(int seed);          // Re-seed
   void RandomInitByArray(int const seeds[], int NumSeeds); // Seed by more than 32 bits
   int IRandom(int min, int max);     // Output random integer
   int IRandom(int max);
   int IRandomX(int min, int max);     // Output random integer, exact
   int IRandomX(int max);
   double Random();                    // Output random float
   DWORD BRandom();                 // Output random bits
private:
   void Init0(int seed);               // Basic initialization procedure
   DWORD mt[MERS_N];                // State vector
   int mti;                            // Index into mt
   DWORD LastInterval;              // Last interval length for IRandomX
   DWORD RLimit;                    // Rejection limit used by IRandomX
};

extern CRandomMersenne g_MersenneRand;