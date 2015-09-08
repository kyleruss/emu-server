#include "stdafx.h"
#include "LargeRand.h"

CLargeRand g_LargLand;

DWORD GetLargeRand()
{
	return g_LargLand.randomMT();
}


CLargeRand::CLargeRand()
{
	this->left = -1;
}



CLargeRand::~CLargeRand()
{
	return;
}


void CLargeRand::seedMT(DWORD seed)
{
	DWORD x = seed | 1;
	DWORD * s = this->state;
	this->left = 0;
	*s = x;
	s++;

	for ( int j = MAX_LARGE_RAND_STATE-1 ; --j != 0 ;  x *= 69069, *s = x, s++);
}



DWORD CLargeRand::reloadMT()
{
	DWORD * p0 = this->state;
	DWORD * p2 = &this->state[2];
	DWORD * pM = &this->state[397];

	if ( this->left < -1 )
		this->seedMT(0x1105);

	this->left = MAX_LARGE_RAND_STATE-2;
	this->next = &this->state[1];
	DWORD s0 = this->state[0];	
	DWORD s1 = this->state[1];

	int j;
	for ( j= 228; --j != 0 ; s0 = s1, s1 = *p2, p2++)
	{
		*p0++ = (*pM++ ^ (((s0 & 0x80000000) | (s1 & 0x7FFFFFFF)) >> 1)) ^ ((((s1&1)!=0)?-1:0)&0x9908B0DF);
	}

	pM = this->state;

	for( j=397 ; --j != 0 ; s0=s1 , s1 = *p2 , p2++ )
	{
		*p0++ = (*pM++ ^ (((s0 & 0x80000000) | (s1 & 0x7FFFFFFF)) >> 1)) ^ ((((s1&1)!=0)?-1:0)&0x9908B0DF);
	}

	s1 = this->state[0];
	*p0 = (*pM ^ (((s0 & 0x80000000) | (s1 & 0x7FFFFFFF)) >> 1)) ^ ((((s1&1)!=0)?-1:0)&0x9908B0DF);
	s1 ^= s1 >> 11;
	s1 ^= (s1 << 7) & 0x9D2C5680;
	s1 ^= (s1 << 15) & 0xEFC60000;

	return s1 ^ ( s1 >> 18 );
}


CRandomMersenne g_MersenneRand((int)time(NULL));

void CRandomMersenne::Init0(int seed) {
   // Seed generator
   const DWORD factor = 1812433253UL;
   mt[0]= seed;
   for (mti=1; mti < MERS_N; mti++) {
      mt[mti] = (factor * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
   }
}

void CRandomMersenne::RandomInit(int seed) {
   // Initialize and seed
   Init0(seed);

   // Randomize some more
   for (int i = 0; i < 37; i++) BRandom();
}


void CRandomMersenne::RandomInitByArray(int const seeds[], int NumSeeds) {
   // Seed by more than 32 bits
   int i, j, k;

   // Initialize
   Init0(19650218);

   if (NumSeeds <= 0) return;

   // Randomize mt[] using whole seeds[] array
   i = 1;  j = 0;
   k = (MERS_N > NumSeeds ? MERS_N : NumSeeds);
   for (; k; k--) {
      mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL)) + (DWORD)seeds[j] + j;
      i++; j++;
      if (i >= MERS_N) {mt[0] = mt[MERS_N-1]; i=1;}
      if (j >= NumSeeds) j=0;}
   for (k = MERS_N-1; k; k--) {
      mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL)) - i;
      if (++i >= MERS_N) {mt[0] = mt[MERS_N-1]; i=1;}}
   mt[0] = 0x80000000UL;  // MSB is 1; assuring non-zero initial array

   // Randomize some more
   mti = 0;
   for ( i = 0; i <= MERS_N; i++) BRandom();
}


DWORD CRandomMersenne::BRandom() {
   // Generate 32 random bits
   DWORD y;

   if (mti >= MERS_N) {
      // Generate MERS_N words at one time
      const DWORD LOWER_MASK = (1LU << MERS_R) - 1;       // Lower MERS_R bits
      const DWORD UPPER_MASK = 0xFFFFFFFF << MERS_R;      // Upper (32 - MERS_R) bits
      static const DWORD mag01[2] = {0, MERS_A};

      int kk;
      for (kk=0; kk < MERS_N-MERS_M; kk++) {    
         y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
         mt[kk] = mt[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];}

      for (; kk < MERS_N-1; kk++) {    
         y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
         mt[kk] = mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ mag01[y & 1];}      

      y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
      mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
      mti = 0;
   }
   y = mt[mti++];

   // Tempering (May be omitted):
   y ^=  y >> MERS_U;
   y ^= (y << MERS_S) & MERS_B;
   y ^= (y << MERS_T) & MERS_C;
   y ^=  y >> MERS_L;

   return y;
}


double CRandomMersenne::Random() {
   // Output random float number in the interval 0 <= x < 1
   // Multiply by 2^(-32)
   return (double)BRandom() * (1./(65536.*65536.));
}


int CRandomMersenne::IRandom(int min, int max) {
   // Output random integer in the interval min <= x <= max
   // Relative error on frequencies < 2^-32
   if (max <= min) {
      if (max == min) return min; else return 0x80000000;
   }
   // Multiply interval with random and truncate
   int r = int((double)(DWORD)(max - min + 1) * Random() + min); 
   if (r > max) r = max;
   return r;
}

int CRandomMersenne::IRandom(int max) {
   // Multiply interval with random and truncate
   int r = int((double)(DWORD)(max) * Random()); 
   if (r >= max) r = max-1;
   return r;
}

int CRandomMersenne::IRandomX(int min, int max) {
   // Output random integer in the interval min <= x <= max
   // Each output value has exactly the same probability.
   // This is obtained by rejecting certain bit values so that the number
   // of possible bit values is divisible by the interval length
   if (max <= min) {
      if (max == min) return min; else return 0x80000000;
   }

   // 64 bit integers available. Use multiply and shift method
   unsigned __int32 interval;                    // Length of interval
   unsigned __int64 longran;                     // Random bits * interval
   unsigned __int32 iran;                        // Longran / 2^32
   unsigned __int32 remainder;                   // Longran % 2^32

   interval = (DWORD)(max - min + 1);
   if (interval != LastInterval) {
      // Interval length has changed. Must calculate rejection limit
      // Reject when remainder >= 2^32 / interval * interval
      // RLimit will be 0 if interval is a power of 2. No rejection then
      RLimit = DWORD(((unsigned __int64)1 << 32) / interval) * interval - 1;
      LastInterval = interval;
   }
   do { // Rejection loop
      longran  = (unsigned __int64)BRandom() * interval;
      iran = (DWORD)(longran >> 32);
      remainder = (DWORD)longran;
   } while (remainder > RLimit);
   // Convert back to signed and return result
   return (__int32)iran + min;
}

int CRandomMersenne::IRandomX(int max) {
   // 64 bit integers available. Use multiply and shift method
   unsigned __int32 interval;                    // Length of interval
   unsigned __int64 longran;                     // Random bits * interval
   unsigned __int32 iran;                        // Longran / 2^32
   unsigned __int32 remainder;                   // Longran % 2^32

   interval = (DWORD)(max);
   if (interval != LastInterval) {
      // Interval length has changed. Must calculate rejection limit
      // Reject when remainder >= 2^32 / interval * interval
      // RLimit will be 0 if interval is a power of 2. No rejection then
      RLimit = DWORD(((unsigned __int64)1 << 32) / interval) * interval - 1;
      LastInterval = interval;
   }
   do { // Rejection loop
      longran  = (unsigned __int64)BRandom() * interval;
      iran = (DWORD)(longran >> 32);
      remainder = (DWORD)longran;
   } while (remainder > RLimit);
   // Convert back to signed and return result
   return (__int32)iran;
}