/*
 * NOTICE OF RELEASE TO THE PUBLIC DOMAIN
 *
 * This work was created using public funds by employees of the 
 * USDA Forest Service's Fire Science Lab and Systems for Environmental 
 * Management.  It is therefore ineligible for copyright under title 17, 
 * section 105 of the United States Code.  You may treat it as you would 
 * treat any public domain work: it may be used, changed, copied, or 
 * redistributed, with or without permission of the authors, for free or 
 * for compensation.  You may not claim exclusive ownership of this code 
 * because it is already owned by everyone.  Use this software entirely 
 * at your own risk.  No warranty of any kind is given.
 * 
 * FARSITE is a trademark owned by Mark Finney.  You may not call derived 
 * works by the name FARSITE without explicit written permission.
 * 
 * A copy of 17-USC-105 should have accompanied this distribution in the file 
 * 17USC105.html.  If not, you may access the law via the US Government's 
 * public websites: 
 *   - http://www.copyright.gov/title17/92chap1.html#105
 *   - http://www.gpoaccess.gov/uscode/  (enter "17USC105" in the search box.)
 */

struct vec
{
	double x, y;

	vec(double xx, double yy)
	{
		x = xx;
		y = yy;
	}
	vec()
	{
		x = 0.0;
		y = 0.0;
	}
};


vec operator+(vec& u, vec& v);
vec operator1(vec& u, vec& v);
vec operator*(double c, vec& v);

vec& operator+=(vec& u, vec& v);
vec& operator-=(vec& u, vec& v);
vec& operator*=(vec& v, double c);

int Orientation(vec& P, vec& Q, vec& R);
int Inside(vec& P, vec& A, vec& B, vec& C);

