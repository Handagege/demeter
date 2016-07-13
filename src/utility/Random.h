#ifndef _DEMETER_UTILITY_RANDOM_H 
#define _DEMETER_UTILITY_RANDOM_H 

#include <vector>
using namespace std; 
#include <stdint.h>


namespace demeter_uti
{
	
// CLASS
//	Random - random value generator
// 
// DESCRIPTION
//	Supports uniform, normal, binomial distributions
//
class Random
{
private: 
	Random(); 
	virtual ~Random(); 
	
public: 
	// NAME
	//	RandUni - generate random value based on uniform distribution
	//
	// DESCRIPTION
	//	left, right: bottom and top of the range
	//
	// RETURN
	//	random value
	static double RandUni(const double left = 0.0, const double right = 1.0);
	
	// NAME	
	//	RandNormal - generate random value based on normal distribution based on Box-Muller algorithm
	//
	// DESCRIPTION
	//	mu, sigma: mean and standard deviation the normal distribution
	//
	// RETURN
	//	random value
	static double RandNormal(const double mu = 0.0, const double sigma = 1.0);

	// NAME
	//	RandBinomial - generate random value based on binomial distribution
	//
	// DESCRIPTION
	//	n: number of trials
	//	p: the probability of success
	//
	// RETURN
	//	random value
	static int32_t RandBinomial(const int32_t n, const double p = 0.5);   

private: 
	// generate random integer value
	static int32_t RandInt(); 

	static vector<int32_t> m_vtrInts;
};

}

#endif /* _DEMETER_UTILITY_RANDOM_H */ 


