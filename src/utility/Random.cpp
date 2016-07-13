#include "Random.h"
using namespace demeter_uti; 
#include <iostream>
#include <algorithm>
using namespace std; 
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>


vector<int32_t> Random::m_vtrInts;

//////////////////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

Random::Random()
{
}


Random::~Random()
{
}


//////////////////////////////////////////////////////////////////////////////////////////
// Operations

double Random::RandUni(const double left, const double right)
{
	return (double)RandInt() / (RAND_MAX + 1.0) * (right - left) + left;
}


double Random::RandNormal(const double mu, const double sigma)
{
	double u1 = RandUni(); 
	double u2 = RandUni(); 
	double z = sqrt(0.0 - 2.0 * log(u1)) * cos(2.0 * M_PI * u2);  
	return sigma * z + mu; 
}


int32_t Random::RandBinomial(const int32_t n, const double p)
{
	if(p < 0 || p > 1.0)
		return 0; 
	int32_t c = 0; 
	for(int32_t i = 0; i < n; i++) 
	{
		if(RandUni() < p)
			c++; 
	}
	return c; 
}


//////////////////////////////////////////////////////////////////////////////////////////
// Internal Operations 

int32_t Random::RandInt()
{
	if(m_vtrInts.empty())
	{
		for(int32_t k = 1; k < 10; k++) 
			m_vtrInts.push_back(k*10+k); 	
	}
	random_shuffle(m_vtrInts.begin(), m_vtrInts.end()); 

	timeval now;
	gettimeofday(&now, NULL);	
	srand(now.tv_sec + now.tv_usec + m_vtrInts[0]); 
	return rand(); 
}


