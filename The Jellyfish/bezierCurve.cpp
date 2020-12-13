#include "bezierCurve.h"

// returns combination(n,r)
float combination(float n, float r) {
	
	// tgamma returns factorial(n-1)
	double fac_n = tgamma(n + 1.0);  
	double fac_r = tgamma(r + 1.0);
	double fac_nmr = tgamma(n - r + 1.0);

	return (float)(fac_n / (fac_nmr * fac_r));
}

