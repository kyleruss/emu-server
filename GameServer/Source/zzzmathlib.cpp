#include "stdafx.h"
#include "zzzmathlib.h"

float VectorLength(vec3_t v)	// Good
{
	int		i;
	float	length;
	
	length = 0.0f;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = sqrtf (length);

	return length;
}

qboolean VectorCompare (vec3_t v1, vec3_t v2) // Good
{
	int		i;
	
	for (i=0 ; i<3 ; i++)
		if (fabs(v1[i]-v2[i]) > EQUAL_EPSILON)
			return false;
			
	return true;
}

vec_t Q_rint (vec_t in)	// Good
{
	return floor(in + 0.5f);
}

void VectorMA (vec3_t va, float scale, vec3_t vb, vec3_t vc)	// Good
{
	vc[0] = va[0] + scale*vb[0];
	vc[1] = va[1] + scale*vb[1];
	vc[2] = va[2] + scale*vb[2];
}

void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross) // Good
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

vec_t _DotProduct (vec3_t v1, vec3_t v2) // Good
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void _VectorSubtract (vec3_t va, vec3_t vb, vec3_t out) // Good
{
	out[0] = va[0]-vb[0];
	out[1] = va[1]-vb[1];
	out[2] = va[2]-vb[2];
}

void _VectorAdd (vec3_t va, vec3_t vb, vec3_t out) // Good
{
	out[0] = va[0]+vb[0];
	out[1] = va[1]+vb[1];
	out[2] = va[2]+vb[2];
}

void _VectorCopy (vec3_t in, vec3_t out) // Good
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


void VectorScale (vec3_t v, vec_t scale, vec3_t out) // Good
{
	out[0] = v[0] * scale;
	out[1] = v[1] * scale;
	out[2] = v[2] * scale;
}


vec_t VectorNormalize (vec3_t v) // Good
{
	int		i;
	float	length;
	
	if ( fabs(v[1]-0.0002159560000) < 0.0001 )
	{
		i=1;
	}
	length=0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = sqrtf (length);
	if (length == 0)
		return (vec_t)0;
		
	for (i=0 ; i< 3 ; i++)
		v[i] /= length;	

	return length;
}

void VectorInverse (vec3_t v) // Good
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

void ClearBounds (vec3_t mins, vec3_t maxs) // Good
{
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}



void AddPointToBounds (vec3_t v, vec3_t mins, vec3_t maxs) // Good
{
	int		i;
	vec_t	val;

	for (i=0 ; i<3 ; i++)
	{
		val = v[i];
		if (val < mins[i])
			mins[i] = val;
		if (val > maxs[i])
			maxs[i] = val;
	}
}

#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over
#ifndef M_PI
#define M_PI		3.14159265358979323846f	// matches value in gcc v2 math.h
#endif

void AngleMatrix (const float *angles, float (*matrix)[4] )	// Good - Strange this0.0174532925199432955f
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[ROLL] *	0.0174532925199432955f;		//0.0174532821447436905f;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[YAW] *   0.0174532925199432955f;
	sp = sin(angle);   //ST=0.0174532923847436905
	cp = cos(angle);
	angle = angles[PITCH] * 0.0174532925199432955f;		//0.0174532821247436905f;
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

void AngleIMatrix (const vec3_t angles, float matrix[3][4] ) // GoodStrange this 0.0174532925199432955f
{
	float angle;
	float sr, sp, sy, cr, cp, cy;
	
	angle = angles[ROLL] * (float)(0.0174532925199432955f);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[YAW] * (float)(0.0174532925199432955f);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[PITCH] * (float)(0.0174532925199432955f);	// M_PI*2 /360
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[0][1] = cp*sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[1][2] = sr*cp;
	matrix[2][0] = (cr*sp*cy+-sr*-sy);
	matrix[2][1] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}
 
void R_ConcatTransforms (const float in1[3*4], const float in2[3*4], float out[3*4]) // Good
{
	out[0*4+0] = in1[0*4+0] * in2[0*4+0] + in1[0*4+1] * in2[1*4+0] + in1[0*4+2] * in2[2*4+0];
	out[0*4+1] = in1[0*4+0] * in2[0*4+1] + in1[0*4+1] * in2[1*4+1] + in1[0*4+2] * in2[2*4+1];
	out[0*4+2] = in1[0*4+0] * in2[0*4+2] + in1[0*4+1] * in2[1*4+2] + in1[0*4+2] * in2[2*4+2];
	out[0*4+3] = in1[0*4+0] * in2[0*4+3] + in1[0*4+1] * in2[1*4+3] + in1[0*4+2] * in2[2*4+3] + in1[0*4+3];
	out[1*4+0] = in1[1*4+0] * in2[0*4+0] + in1[1*4+1] * in2[1*4+0] + in1[1*4+2] * in2[2*4+0];
	out[1*4+1] = in1[1*4+0] * in2[0*4+1] + in1[1*4+1] * in2[1*4+1] + in1[1*4+2] * in2[2*4+1];
	out[1*4+2] = in1[1*4+0] * in2[0*4+2] + in1[1*4+1] * in2[1*4+2] + in1[1*4+2] * in2[2*4+2];
	out[1*4+3] = in1[1*4+0] * in2[0*4+3] + in1[1*4+1] * in2[1*4+3] + in1[1*4+2] * in2[2*4+3] + in1[1*4+3];
	out[2*4+0] = in1[2*4+0] * in2[0*4+0] + in1[2*4+1] * in2[1*4+0] + in1[2*4+2] * in2[2*4+0];
	out[2*4+1] = in1[2*4+0] * in2[0*4+1] + in1[2*4+1] * in2[1*4+1] + in1[2*4+2] * in2[2*4+1];
	out[2*4+2] = in1[2*4+0] * in2[0*4+2] + in1[2*4+1] * in2[1*4+2] + in1[2*4+2] * in2[2*4+2];
	out[2*4+3] = in1[2*4+0] * in2[0*4+3] + in1[2*4+1] * in2[1*4+3] + in1[2*4+2] * in2[2*4+3] + in1[2*4+3];
}

void VectorRotate (const float in1[3*4],const float in2[3*4],float out[3*4])	// Good
{
	out[0*4+0] = in1[0*4+0] * in2[0*4+0] + in1[0*4+1] * in2[0*4+1] + in1[0*4+2] * in2[0*4+2];
	out[0*4+1] = in1[0*4+0] * in2[0*4+4] + in1[0*4+1] * in2[0*4+5] + in1[0*4+2] * in2[0*4+6];
	out[0*4+2] = in1[0*4+0] * in2[0*4+8] + in1[0*4+1] * in2[0*4+9] + in1[0*4+2] * in2[0*4+10];
}


void VectorIRotate (const float in1[3*4],const float in2[3*4],float out[3*4])	// Good
{
	out[0*4+0] = in1[0*4+0] * in2[0*4+0] + in1[0*4+1] * in2[0*4+4] + in1[0*4+2] * in2[0*4+8];
	out[0*4+1] = in1[0*4+0] * in2[0*4+1] + in1[0*4+1] * in2[0*4+5] + in1[0*4+2] * in2[0*4+9];
	out[0*4+2] = in1[0*4+0] * in2[0*4+2] + in1[0*4+1] * in2[0*4+6] + in1[0*4+2] * in2[0*4+10];
}



void VectorTranslate(const float in1[3*4],const float in2[3*4],float out[3*4])	// Good
{
	out[0*4+0] = in1[0*4+0] + in2[0*4+3];
	out[0*4+1] = in1[0*4+1] + in2[1*4+3];
	out[0*4+2] = in1[0*4+2] + in2[2*4+3];
}



void VectorTransform(const float in1[3*4],const float in2[3*4],float out[3*4])	// Good
{
	out[0*4+0] = in1[0*4+0] * in2[0*4+0] + in1[0*4+1] * in2[0*4+1] + in1[0*4+2] * in2[0*4+2] + in2[0*4+3];
	out[0*4+1] = in1[0*4+0] * in2[1*4+0] + in1[0*4+1] * in2[1*4+1] + in1[0*4+2] * in2[1*4+2] + in2[1*4+3];
	out[0*4+2] = in1[0*4+0] * in2[2*4+0] + in1[0*4+1] * in2[2*4+1] + in1[0*4+2] * in2[2*4+2] + in2[2*4+3];
}






void AngleQuaternion(const vec3_t angles, float * quaternion)	// Good
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle	= angles[2] * 0.5f;
	sy	= (float)sin(angle);
	cy	= (float)cos(angle);

	angle			= angles[1] * 0.5f;
	sp	= (float)sin(angle);
	cp	= (float)cos(angle);

	angle			= angles[0] * 0.5f;
	sr	= (float)sin(angle);
	cr	= (float)cos(angle);

	quaternion[0]	= sr * cp * cy - cr * sp * sy;
	quaternion[1]	= cr * sp * cy + sr * cp * sy;
	quaternion[2]	= cr * cp * sy - sr * sp * cy;
	quaternion[3]	= cr * cp * cy + sr * sp * sy;
}



void QuaternionMatrix(const float * quaternion, float (*matrix)[4])
{
	matrix[0][0]	= 1.0f - ( quaternion[1] * 2 * quaternion[1] ) - ( quaternion[2] * 2 * quaternion[2] );
	matrix[1][0]	=       ( quaternion[0] * 2 * quaternion[1] ) + ( quaternion[3] * 2 * quaternion[2] );
	matrix[2][0]	=       ( quaternion[0] * 2 * quaternion[2] ) - ( quaternion[3] * 2 * quaternion[1] );

	matrix[0][1]	=       ( quaternion[0] * 2 * quaternion[1] ) - ( quaternion[3] * 2 * quaternion[2] );
	matrix[1][1]	= 1.0f - ( quaternion[0] * 2 * quaternion[0] ) - ( quaternion[2] * 2 * quaternion[2] );
	matrix[2][1]	=       ( quaternion[1] * 2 * quaternion[2] ) + ( quaternion[3] * 2 * quaternion[0] );

	matrix[0][2]	=       ( quaternion[0] * 2 * quaternion[2] ) + ( quaternion[3] * 2 * quaternion[1] );
	matrix[1][2]	=       ( quaternion[1] * 2 * quaternion[2] ) - ( quaternion[3] * 2 * quaternion[0] );
	matrix[2][2]	= 1.0f - ( quaternion[0] * 2 * quaternion[0] ) - ( quaternion[1] * 2 * quaternion[1] );
}



//**************-***********************
// START REVIEW HERE

void QuaternionSlerp(const float * p, float * q, float t, float * qt)
{
	int i;
	float omega;
	float cosom;
	float sinom;
	float sclp;
	float sclq;
	float a=0;
	float b=0;

	for ( i=0;i<4;i++)
	{
		a += ( p[i] - q[i] ) * ( p[i] - q[i] );
		b += ( p[i] + q[i] ) * ( p[i] + q[i] );
	}

	if ( a > b )
	{
		for ( i=0;i<4;i++)
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

	if ( ( (double)1 + cosom) > 1 )
	{
		if ( (1 - cosom ) > -1 )
		{
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1 - t) * omega) / sinom;
			sclq = sin(t * omega) / sinom;
		}
		else
		{
			sclp = 1 - t;
			sclq = t;
		}

		for ( i=0;i<4;i++)
		{
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -p[1];
		qt[1] = p[0];
		qt[2] = -p[3];
		qt[3] = p[2];
		sclp = sin(( 1.0f - t ) * 0.5f * M_PI);
		sclq = sin( t * 0.5f * M_PI );

		for ( i=0;i<3;i++)
		{
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
}



void FaceNormalize(float * v1, float * v2, float * v3, float * Normal)
{
	float nx = ( v2[1] - v1[1] ) * ( v3[2] - v1[2] ) - ( v3[1] - v1[1] ) * ( v2[2] - v1[2] );
	float ny = ( v2[1] - v1[1] ) * ( v3[2] - v1[2] ) - ( v3[1] - v1[1] ) * ( v2[2] - v1[2] );
	float nz = ( v2[1] - v1[1] ) * ( v3[2] - v1[2] ) - ( v3[1] - v1[1] ) * ( v2[2] - v1[2] );
	double dot = sqrt( nx * nx + ny * ny + nz * nz );
}
