/**
 * @file Math/Common.h
 *
 * This contains some often used mathematical definitions and functions.
 *
 * @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
 * @author Max Risler
 */


#ifndef __Math_Common_h__
#define __Math_Common_h__

#include <cmath>
#include <cstdlib>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#define COLLISION_DISTANCE 1000.0f
#define CRASH_Z 20.0f
#define CRASH_INC 25.0f
#define CRASH_SPEED 10.0f
#define INIT_MAX_FLIGHTS 5


#define AIRPORT_DISTANCE_MAX 	20000.0f
#define FLIGHT_HEIGHT	 		2000.0f
#define LANDING_STRIP_L	 		750.0f
#define LANDING_STRIP_W	 		80.0f

//Conditions for landing

#define LANDING_POS_X		0.0f
#define LANDING_POS_Y		0.0f
#define LANDING_POS_Z		25.0f


#define LANDING_SPEED 			20.0f
#define LANDING_BEAR  			-180.0f
#define LANDING_BEAR_MAX_ERROR	20.0f
#define LANDING_DIST  			70.0f

#define INIT_FLIGHT_POINTS  200
#define INIT_POINTS 0
#define LANDING_POINTS 	 	(10)
#define COLLISION_POINTS 	(-50)
#define CRASH_HEIGHT_POINTS	(-25)
#define CRASH_INC_POINTS	(-25)
#define CRASH_SPEED_POINTS 	(-25)

#define INC_DIFF		30000000 //30 sec
#define INC_PEN			1 	 	//msec

#define INC_SIMTIME			0.1 

//MIS CAMBIOS
//Constantes Rutas
#define POS0_x			4000.0  //Posición antes de procedimiento de bajada
#define POS0_y			0.0
#define POS0_z			680.0
#define SPEED_0			120.0

#define POS1_x			2000.0   //Mitad de la bajada
#define POS1_y			0.0
#define POS1_z			300.0
#define SPEED_1			80.0


#define POS2_x			LANDING_POS_X 	 //Avión en suelo, principio de pista
#define POS2_y			LANDING_POS_Y
#define POS2_z			LANDING_POS_Z
#define SPEED_2			18.0

#define POS3_x			-750.0 	 //Final de pista
#define POS3_y			0.0
#define POS3_z			25.0
#define SPEED_3			10.0

#define DISROUTE		4000.0 	//Distancia entre cada punto de ruta fuera landing
#define SECTIME			100.0 	//Distancia en tiempo de seguridad que debe haber entre avión y avión
#define MAXSECTIME		50.0 	//Distancia en tiempo mínima, mínima a la que pueden
								//estar dos aviones sin ser uno redirigido
#define DIFFSPEED 		20.0 	//Differencia en velocidad entre cada punto de la ruta fuera landing
#define HEIGHTDIFFERENCEPOINTS 100.0 //Diferencia en altitud entre cada punto de ruta fuera landing

#define MAXACC 			10 		//Máxima acc o desaceleración que tiene el avion (m/s)


//Storm
#define MIN_ALTITUD_BASE_STORM 	1000	// m
#define MAX_ALTITUD_BASE_STORM 	2000	// m
#define MAX_HEIGHT_STORM		2000
#define MIN_SPEED_STORM			80 		// m/s
#define MAX_SPEED_STORM			300		// m/s
#define MIN_RADIO_STORM			100		// m
#define MAX_RADIO_STORM			2000	// m
#define PERIOD_BETWEEN_STORMS	30		// s

//Constantes de DEBUG
#define DEBUG_MODE				true
#define DEBUG_STORMS			true


/**
* defines the sign of a (-1, 0 or 1)
*/
#ifndef sgn
#define sgn(a)   ( (a) < 0 ? -1 : ((a)==0) ? 0 : 1 )
#endif

/**
* defines the sign of a (-1 or 1)
*/
#ifndef sign
#define sign(a)   ( (a) < 0 ? -1 : 1 )
#endif

/**
* defines the square of a value
*/
#ifndef sqr
#define sqr(a) ( (a) * (a) )
#endif

inline double sec(const double a){return 1/cos(a);}

inline double cosec(const double a){return 1/sin(a);}

/** @name constants for some often used angles */
///@{
/** constant for a half circle*/
const double pi = 3.1415926535897932384626433832795;
/** constant for a full circle*/
const double pi2 = 2.0*3.1415926535897932384626433832795;
/** constant for three quater circle*/
const double pi3_2 = 1.5*3.1415926535897932384626433832795;
/** constant for a quarter circle*/
const double pi_2 = 0.5*3.1415926535897932384626433832795;
/** constant for a 1 degree*/
const double pi_180 = 3.1415926535897932384626433832795/180;
/** constant for a 1/8 circle*/
const double pi_4 = 3.1415926535897932384626433832795*0.25;
/** constant for a 3/8 circle*/
const double pi3_4 = 3.1415926535897932384626433832795*0.75;
/** constant for an expression used by the gaussian function*/
const double sqrt2pi = sqrt(2.0*pi);
///@}


/**
 * Converts angle from rad to degrees.
 * \param angle code in rad
 * \return angle coded in degrees
 */
inline double toDegrees(double angle){return angle * 180.0 / pi;}

/** Converts angle from degrees to rad.
 * \param degrees angle coded in degrees
 * \return angle coded in rad
 */
inline double fromDegrees(double degrees){return degrees * pi_180;}

/** Converts angle from degrees to rad.
 * \param degrees angle coded in degrees
 * \return angle coded in rad
 */
inline double toRadians(double degrees){return degrees * pi_180;}

/**
* reduce angle to [-pi..+pi[
* \param data angle coded in rad
* \return normalized angle coded in rad
*/
inline double normalizePi(double data)
{
  if (data < pi && data >= -pi) return data;
  double ndata = data - ((int )(data / pi2))*pi2;
  while (ndata >= pi)
  {
    ndata -= pi2;
  }
  while (ndata < -pi)
  {
    ndata += pi2;
  }
  return ndata;
}

/**
* The function returns a random number in the range of [0..1].
* @return The random number.
*/
inline double randomDouble() {return double(rand()) / RAND_MAX;}

/**
* The function returns a random integer number in the range of [0..n-1].
* @param n the number of possible return values (0 ... n-1)
* @return The random number.
*/
inline int random(int n) {return (int)(randomDouble()*n*0.999999);}

/*
 * Devuelve valor aleatorio entre dos numeros
 */
inline int randomBetween(int min, int max)
{
	int auxiliarNum;
	//En caso de que se pongan al reves
	if(min>max)
	{
		auxiliarNum = min;
		min = max;
		max = auxiliarNum;
	}

	return min + random(max-min+1);
}

/*
 * 	Devuelve un angulo aleatorio normalizado
 */
inline float randomAngle()
{
	return normalizePi(randomDouble()*2*M_PI);
}

inline float randomBetweenAnglesDeg(int angmin, int angmax)
{
	double angle;
	angle = randomBetween(angmin,angmax);
	angle = toRadians(angle);
	return normalizePi(angle);


}
/** constant, cast before execution*/
const double RAND_MAX_DOUBLE = static_cast<double>(RAND_MAX);


inline float saturate(float control, float min, float max)
{
	if(control<min)
	{
		return min;
	} else if(control>max)
	{
		return max;
	} else
		return control;


}


inline float timeBetweenTwoRoutes(float distance,float v0, float vf, float maxacc, float &velfin)
{
	//Para un vuelo de un punto a otro separados a una distancia d con v0 y vf distintos
	//se realiza en dicha trayectoria un movimiento accelerado en la primera sección
	//y de movimeinteo rectilineo uniforme en la segunda cuando la v es v=vf
	//tomando en cuenta la aceleración, se calcula el tiempo
	//float distance = r1.pos.distance(r2.pos);
	float differenceinspeed = vf-v0;
	float acc = sign(differenceinspeed)*maxacc;
	float timeacc = abs(differenceinspeed)/maxacc;
	float distanceacc = v0*timeacc + 0.5*acc*pow(timeacc,2);
	float distanceuniform = distance-distanceacc;
	float timeunif = distanceuniform / vf;
	float totaltime;
	if(distanceacc>distance)
	{
		//Ecuación de segundo grado con dist = v_0*t +0.5*acc*t^2 en el que t es la incognita
		float ec2entero = -v0/acc;
		float ec2raiz = sqrt(pow(v0,2)+2*distance*acc)/acc;
		if ((ec2entero+ec2raiz)>0)
			totaltime = ec2entero+ec2raiz;
		else
			totaltime = ec2entero-ec2raiz;
		velfin = v0 + acc*totaltime;

		return totaltime;
	} else {
		/*std::cout	<<"-------suficiente distancia-----"<<std::endl
					<<distanceacc <<" "<<distance<<" "<<distanceuniform<<std::endl
					<<"times "<<timeunif<<" "<<timeacc<<" "<<timeacc+timeunif<<std::endl;*/
		totaltime = timeunif + timeacc;
		velfin = vf;
		return totaltime;
	}


}



inline int dirofrotation(float actualangle, float desangle)
{
	if(desangle<0)
		desangle += 2*M_PI;
	if(actualangle<0)
		actualangle +=2*M_PI;
	if(desangle<actualangle)
		desangle += 2*M_PI;
	if(desangle - actualangle<=M_PI)
		return 1;
	else
		return -1;
}
/**
* The function returns a random integer number in the range of [0..n].
* @param n the number of possible return values (0 ... n)
* @return The random number.
*/
inline int randomFast(int n)
{
  return static_cast<int>((rand()*n) / RAND_MAX_DOUBLE);
}

/**
* Round to the next integer
* @param d A number
* @return The number as integer
*/
inline int roundNumberToInt(double d)
{
  return static_cast<int>(floor(d+0.5));
}

/**
* Round to the next integer but keep type
* @param d A number
* @return The number
*/
inline double roundNumber(double d)
{
  return floor(d+0.5);
}

inline float normalAng(float x, float mu, float st)
{
	float diff = normalizePi(x-mu);
	float d1 = st*sqrt(2.0*M_PI);
	float d2 = 2*st*st;

	if(d1==0.0) d1 = 0.00001;
	if(d2==0.0) d2 = 0.00001;

	return (1.0/d1)*exp(-(diff*diff)/d2);
}
inline float normalDist(float x, float mu, float st)
{
	float diff = x-mu;
	float d1 = st*sqrt(2.0*M_PI);
	float d2 = 2*st*st;

	if(d1==0.0) d1 = 0.00001;
	if(d2==0.0) d2 = 0.00001;

	return (1.0/d1)*exp(-(diff*diff)/d2);
}



#endif // __Math_Common_h__
