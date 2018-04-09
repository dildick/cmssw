#ifndef L1Trigger_L1TMuon_EndcapPtAssignmentHelper_h
#define L1Trigger_L1TMuon_EndcapPtAssignmentHelper_h

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

namespace EndcapTriggerPtAssignmentHelper {

/*eta partitions: 
*Position: 1.2-1.4,1.4-1.6,1.6-1.8, 1.8-2.0, 2.0-2.2, 2.2-2.4
*Direction:  1.2-1.4,1.4-1.6,1.6-1.8, 1.8-2.0, 2.0-2.1, 2.1-2.2, 2.2-2.4, eta > 2.1, ME0 will be used
*Hybrid:  1.2-1.4,1.4-1.6,1.6-1.8, 1.8-2.0, 2.0-2.1, eta > 2.1, ME0 will be used and LUT is different for ME0 region
*/
 enum {NEtabins_position = 7, NEtabins_direction = 8, NEtabins_hybrid=6, NParitybins=4, NPtbins=6, NHybridParas = 5};
 
 const double Etabins_position[NEtabins_position] = { 1.20,   1.40,   1.60,   1.80,   2.00,   2.20,   2.40};
 const double Etabins_direction[NEtabins_direction] = { 1.20,   1.40,   1.60,   1.80,   2.00,   2.10,   2.20,   2.40};
 const double Etabins_hybrid[NEtabins_hybrid] = { 1.20,   1.40,   1.60,   1.80,   2.00,   2.10};
 
 //const float PtBins[NPt] = {5,7,10,15,20,30,40};
 //const float PtBins2[NPt2] = {2,3,4,5,6,7,8,10,12,15,20,30,40};
 const float PtBins[NPtbins] = { 5,   7,   10,   12,   15,   20};

// parity cases when considering multiple station hits
 enum class EvenOdd{Odd=0, Even=1, Invalid=2};
 enum class EvenOdd12{OddEven, OddOdd, EvenEven, EvenOdd, Invalid};
 enum class EvenOdd123{OddEvenEven, OddOddOdd, EvenEvenEven, EvenOddOdd, Invalid};

// phi_momentum = phi_position+gem-csc_bending*slope
const double BendingAngleLUT[2][2] = {
	{28.71,12.86},//even,odd for ME11
	{39.11,18.31},
};



int GetEtaPartition_position(float eta);
int GetEtaPartition_direction(float eta);
int GetEtaPartition_hybrid(float eta);
//int GetEtaPartition(float eta);
template <size_t N>
int GetEtaPartition ( const double (& Etabins) [N], float  eta );

// equation of ellipse
float ellipse(float a, float b,float anlge, float x0, float y0, float x, float y);

float deltaYcalculation(const GlobalPoint& gp1, const GlobalPoint& gp2);

float deltadeltaYcalculation(const GlobalPoint& gp1, const GlobalPoint& gp2, const GlobalPoint& gp3, float eta, enum EvenOdd123 parity);

//float Ptassign_Direction(float bending_12, float eta, int par);

float PhiMomentum(float dphi, float phi_position, int st, bool evenodd);
float PhiMomentum_Radius(float dphi, float phi_position, float radius_csc, float radius_gem);
//float phiMomentum_Xfactor(float dphi, float phi_position, float x);
float phiMomentum_Xfactor(float phi_CSC, float phi_GEM, float x);


void calculateAlphaBeta(const std::vector<float>& v,
                        const std::vector<float>& w,
                        const std::vector<float>& ev,
                        const std::vector<float>& ew,
                        const std::vector<float>& status,
                        float& alpha, float& beta);

float normalizePhi(float phi);

 EvenOdd123 getParity(EvenOdd, EvenOdd, EvenOdd, EvenOdd);


const double PositionEpLUT[NParitybins][NEtabins_position][3] = {
   //prop_factor, slope, intercept
  {
    {1.279, 0.04784, 0.1122},
    {1.279, 0.65424, 0.09761},
    {0.640, 0.05527, 0.08944},
    {0.640, 0.08295, 0.1279},
    {0.640, 0.1660, 0.2158},
    {0.640, 0.4952, 0.7103},
  },
  {
    {0.6357, 0.0827, 0.2021},
    {0.6357, 0.0906, 0.1773},
    {0.3530, 0.1067, 0.1957},
    {0.3530, 0.1561, 0.2645},
    {0.3530, 0.3156, 0.4514},
    {0.3530, 0.8242, 1.0712},
  },
  {
    {1.001, 0.038, 0.008345},
    {1.001, 0.04157, 0.0617},
    {0.5550, 0.0562, 0.08417},
    {0.5550, 0.0870, 0.1426},
    {0.5550, 0.1676, 0.2198},
    {0.5550, 0.4953, 0.7272},
  },
  {
    {0.5252, 0.0739, 0.1714},
    {0.5252, 0.07838, 0.1307},
    {0.310, 0.1066, 0.2026},
    {0.310, 0.1435, 0.2118},
    {0.310, 0.2874, 0.4055},
    {0.310, 0.7625, 1.075},
  },
};

/*
//updated the LUT, by Tao, 20160724
const double DirectionEpLUT[NParitybins][NEtabins_Direction][2]={
    	       {{2.907, 5.906},
		{2.600, 5.191},
		{4.405, 8.809},
		{7.332, 14.478},
		{8.747, 12.265},
		//{11.02, 14.84},
	       },
	       {{2.409, 5.198},
		{2.467, 4.397},
		{4.411, 8.621},
		{7.333, 14.552},
		{8.955, 13.093},
		//{10.34, 11.02},
	       },
	       {{2.301, 4.929},
		{2.230, 3.111},
		{5.288, 10.987},
		{8.242, 16.698},
		{9.213, 13.111},
		//{11.23, 13.44},
	       },
	       {{2.401, 4.758},
		{2.383, 3.782},
		{5.222, 10.769},
		{8.292, 16.955},
		{9.074, 12.083},
		//{11.83, 17.66},
	       },
	};

}*/

/***********************************************************************
 * Position based LUT
* Ptbins  5,   7,   10,   12,   15,   20
* Etabins  1.20,   1.40,   1.60,   1.80,   2.00,   2.20,   2.40
* Npartiy odd,even odd,odd even,even even,odd
***********************************************************************/
const double PositionPtDDYLUT[NPtbins][NEtabins_position][NParitybins] = {
{/// pt=5
	{ 5.154, 2.724, 5.513, 3.103 },/// 1.20<|eta|<1.40
	{ 5.744, 2.747, 5.432, 3.219 },/// 1.40<|eta|<1.60
	{ 4.392, 2.661, 4.302, 2.719 },/// 1.60<|eta|<1.80
	{ 3.179, 1.928, 3.061, 1.877 },/// 1.80<|eta|<2.00
	{ 1.952, 1.194, 1.836, 1.153 },/// 2.00<|eta|<2.20
	{ 2.603, 2.603, 2.368, 2.603 },/// 2.20<|eta|<2.40
 					},/// pt=5
{/// pt=7
	{ 4.589, 2.144, 4.272, 2.517 },/// 1.20<|eta|<1.40
	{ 4.701, 2.073, 4.161, 2.290 },/// 1.40<|eta|<1.60
	{ 3.336, 2.030, 3.197, 1.986 },/// 1.60<|eta|<1.80
	{ 2.580, 1.507, 2.346, 1.516 },/// 1.80<|eta|<2.00
	{ 1.735, 1.008, 1.536, 1.004 },/// 2.00<|eta|<2.20
	{ 1.890, 2.603, 1.567, 1.069 },/// 2.20<|eta|<2.40
 					},/// pt=7
{/// pt=10
	{ 4.352, 1.662, 3.172, 1.944 },/// 1.20<|eta|<1.40
	{ 3.927, 1.587, 3.074, 1.738 },/// 1.40<|eta|<1.60
	{ 2.812, 1.554, 2.410, 1.554 },/// 1.60<|eta|<1.80
	{ 2.161, 1.251, 1.849, 1.283 },/// 1.80<|eta|<2.00
	{ 1.586, 0.875, 1.353, 0.925 },/// 2.00<|eta|<2.20
	{ 1.245, 1.011, 1.099, 0.776 },/// 2.20<|eta|<2.40
 					},/// pt=10
{/// pt=12
	{ 4.275, 1.479, 2.762, 1.623 },/// 1.20<|eta|<1.40
	{ 3.811, 1.396, 2.674, 1.598 },/// 1.40<|eta|<1.60
	{ 2.709, 1.397, 2.124, 1.399 },/// 1.60<|eta|<1.80
	{ 2.006, 1.153, 1.717, 1.206 },/// 1.80<|eta|<2.00
	{ 1.550, 0.829, 1.288, 0.894 },/// 2.00<|eta|<2.20
	{ 0.815, 1.011, 0.962, 0.571 },/// 2.20<|eta|<2.40
 					},/// pt=12
{/// pt=15
	{ 4.208, 1.293, 2.388, 1.479 },/// 1.20<|eta|<1.40
	{ 3.754, 1.231, 2.253, 1.502 },/// 1.40<|eta|<1.60
	{ 2.537, 1.289, 1.927, 1.317 },/// 1.60<|eta|<1.80
	{ 1.892, 1.080, 1.547, 1.131 },/// 1.80<|eta|<2.00
	{ 1.532, 0.799, 1.215, 0.876 },/// 2.00<|eta|<2.20
	{ 0.815, 1.011, 0.962, 0.571 },/// 2.20<|eta|<2.40
 					},/// pt=15
{/// pt=20
	{ 4.056, 1.147, 1.963, 1.214 },/// 1.20<|eta|<1.40
	{ 3.632, 1.049, 1.843, 1.453 },/// 1.40<|eta|<1.60
	{ 2.457, 1.190, 1.674, 1.204 },/// 1.60<|eta|<1.80
	{ 1.809, 1.026, 1.418, 1.083 },/// 1.80<|eta|<2.00
	{ 1.529, 0.777, 1.172, 0.867 },/// 2.00<|eta|<2.20
	{ 0.815, 1.011, 0.708, 0.571 },/// 2.20<|eta|<2.40
 					},/// pt=20
	}; /// positionLUT end


/***********************************************************************
 * Direction based LUT
* Ptbins  5,   7,   10,   12,   15,   20
* Etabins  1.20,   1.40,   1.60,   1.80,   2.00,   2.10,   2.20,   2.40
* Npartiy odd,even odd,odd even,even even,odd
***********************************************************************/
const double DirectionbasedLUT[NPtbins][NEtabins_direction][NParitybins] = {
{/// pt=5
	{ 0.141, 0.139, 0.137, 0.151 },/// 1.20<|eta|<1.40
	{ 0.130, 0.141, 0.140, 0.142 },/// 1.40<|eta|<1.60
	{ 0.099, 0.060, 0.111, 0.070 },/// 1.60<|eta|<1.80
	{ 0.072, 0.058, 0.085, 0.065 },/// 1.80<|eta|<2.00
	{ 0.097, 0.084, 0.101, 0.082 },/// 2.00<|eta|<2.10
	{ 0.166, 0.122, 0.110, 0.127 },/// 2.10<|eta|<2.20
	{ 0.122, 0.151, 0.104, 0.127 },/// 2.20<|eta|<2.40
 					},/// pt=5
{/// pt=7
	{ 0.133, 0.128, 0.125, 0.133 },/// 1.20<|eta|<1.40
	{ 0.120, 0.128, 0.126, 0.117 },/// 1.40<|eta|<1.60
	{ 0.092, 0.050, 0.105, 0.059 },/// 1.60<|eta|<1.80
	{ 0.067, 0.051, 0.079, 0.059 },/// 1.80<|eta|<2.00
	{ 0.095, 0.079, 0.101, 0.080 },/// 2.00<|eta|<2.10
	{ 0.122, 0.110, 0.089, 0.086 },/// 2.10<|eta|<2.20
	{ 0.122, 0.151, 0.104, 0.098 },/// 2.20<|eta|<2.40
 					},/// pt=7
{/// pt=10
	{ 0.131, 0.121, 0.116, 0.125 },/// 1.20<|eta|<1.40
	{ 0.114, 0.122, 0.119, 0.108 },/// 1.40<|eta|<1.60
	{ 0.087, 0.044, 0.096, 0.054 },/// 1.60<|eta|<1.80
	{ 0.065, 0.047, 0.077, 0.054 },/// 1.80<|eta|<2.00
	{ 0.095, 0.078, 0.100, 0.078 },/// 2.00<|eta|<2.10
	{ 0.066, 0.057, 0.081, 0.045 },/// 2.10<|eta|<2.20
	{ 0.078, 0.066, 0.072, 0.066 },/// 2.20<|eta|<2.40
 					},/// pt=10
{/// pt=12
	{ 0.131, 0.119, 0.114, 0.124 },/// 1.20<|eta|<1.40
	{ 0.113, 0.121, 0.117, 0.107 },/// 1.40<|eta|<1.60
	{ 0.087, 0.042, 0.090, 0.052 },/// 1.60<|eta|<1.80
	{ 0.065, 0.045, 0.077, 0.053 },/// 1.80<|eta|<2.00
	{ 0.095, 0.077, 0.101, 0.078 },/// 2.00<|eta|<2.10
	{ 0.078, 0.066, 0.057, 0.045 },/// 2.10<|eta|<2.20
	{ 0.054, 0.066, 0.072, 0.051 },/// 2.20<|eta|<2.40
 					},/// pt=12
{/// pt=15
	{ 0.131, 0.117, 0.113, 0.121 },/// 1.20<|eta|<1.40
	{ 0.108, 0.119, 0.114, 0.104 },/// 1.40<|eta|<1.60
	{ 0.089, 0.040, 0.090, 0.052 },/// 1.60<|eta|<1.80
	{ 0.064, 0.045, 0.076, 0.053 },/// 1.80<|eta|<2.00
	{ 0.095, 0.078, 0.102, 0.078 },/// 2.00<|eta|<2.10
	{ 0.054, 0.048, 0.072, 0.051 },/// 2.10<|eta|<2.20
	{ 0.054, 0.048, 0.069, 0.051 },/// 2.20<|eta|<2.40
 					},/// pt=15
{/// pt=20
	{ 0.130, 0.117, 0.111, 0.119 },/// 1.20<|eta|<1.40
	{ 0.108, 0.119, 0.114, 0.102 },/// 1.40<|eta|<1.60
	{ 0.089, 0.038, 0.085, 0.051 },/// 1.60<|eta|<1.80
	{ 0.064, 0.044, 0.076, 0.053 },/// 1.80<|eta|<2.00
	{ 0.096, 0.077, 0.103, 0.078 },/// 2.00<|eta|<2.10
	{ 0.054, 0.031, 0.069, 0.045 },/// 2.10<|eta|<2.20
	{ 0.051, 0.048, 0.054, 0.048 },/// 2.20<|eta|<2.40
 					},/// pt=20
	}; /// directionLUT end
/***********************************************************************
 * Hybrid based LUT
* Ptbins  5,   7,   10,   12,   15,   20
* Etabins  1.20,   1.40,   1.60,   1.80,   2.00,   2.10
* Npartiy odd,even odd,odd even,even even,odd
* (a, b, theta, x0, y0) in ellipe formula (x*cos(theta)+y*sin(theta)-x0)^2/a^2 + (x*sin(theta) - y*cos(theta)-y0)^2/b^2 <= 1
***********************************************************************/
const double HybridLUT[NPtbins][NEtabins_hybrid][NParitybins][NHybridParas] = {
{/// pt=5
	{/// 1.20<|eta|<1.40
		 {7.872, 0.364, 0.035, 0.000, -0.000 },/// npar=0,odd,even
		 {3.074, 0.412, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {5.828, 0.406, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {3.322, 0.446, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.20<|eta|<1.40
	{/// 1.40<|eta|<1.60
		 {6.546, 0.254, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {3.588, 0.330, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {8.219, 0.387, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {3.411, 0.422, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.40<|eta|<1.60
	{/// 1.60<|eta|<1.80
		 {6.395, 0.281, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {3.977, 0.172, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {5.121, 0.330, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {2.831, 0.208, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.60<|eta|<1.80
	{/// 1.80<|eta|<2.00
		 {4.107, 0.215, 0.035, -0.000, -0.000 },/// npar=0,odd,even
		 {2.277, 0.104, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {3.276, 0.248, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {2.146, 0.195, 0.035, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.80<|eta|<2.00
	{/// 2.00<|eta|<2.10
		 {2.271, 0.234, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.496, 0.155, 3.107, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.222, 0.175, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.432, 0.161, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 2.00<|eta|<2.10
								},/// pt=5
{/// pt=7
	{/// 1.20<|eta|<1.40
		 {5.473, 0.398, 0.035, 0.000, -0.000 },/// npar=0,odd,even
		 {2.747, 0.364, 0.070, -0.000, 0.000 },/// npar=1,odd,odd
		 {5.323, 0.374, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {2.670, 0.389, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.20<|eta|<1.40
	{/// 1.40<|eta|<1.60
		 {5.640, 0.187, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {3.084, 0.382, 0.105, -0.000, 0.000 },/// npar=1,odd,odd
		 {5.217, 0.378, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {2.414, 0.347, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.40<|eta|<1.60
	{/// 1.60<|eta|<1.80
		 {3.521, 0.245, 0.000, -0.000, 0.000 },/// npar=0,odd,even
		 {2.238, 0.104, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {3.378, 0.307, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {2.152, 0.167, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.60<|eta|<1.80
	{/// 1.80<|eta|<2.00
		 {3.068, 0.200, 0.035, -0.000, -0.000 },/// npar=0,odd,even
		 {1.664, 0.110, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.567, 0.206, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.750, 0.163, 0.035, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.80<|eta|<2.00
	{/// 2.00<|eta|<2.10
		 {1.961, 0.231, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.245, 0.133, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.842, 0.171, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.163, 0.203, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 2.00<|eta|<2.10
								},/// pt=7
{/// pt=10
	{/// 1.20<|eta|<1.40
		 {4.951, 0.386, 0.035, 0.000, -0.000 },/// npar=0,odd,even
		 {1.757, 0.309, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {3.803, 0.337, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {2.046, 0.354, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.20<|eta|<1.40
	{/// 1.40<|eta|<1.60
		 {4.576, 0.172, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {1.712, 0.319, 3.072, -0.000, 0.000 },/// npar=1,odd,odd
		 {3.824, 0.303, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {1.803, 0.316, 0.000, -0.000, -0.000 },/// npar=3,even,odd
							},/// 1.40<|eta|<1.60
	{/// 1.60<|eta|<1.80
		 {3.187, 0.225, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.652, 0.125, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {3.786, 0.153, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {1.673, 0.157, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.60<|eta|<1.80
	{/// 1.80<|eta|<2.00
		 {2.277, 0.192, 0.000, -0.000, -0.000 },/// npar=0,odd,even
		 {1.331, 0.124, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.489, 0.206, 0.070, -0.000, 0.000 },/// npar=2,even,even
		 {1.450, 0.121, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.80<|eta|<2.00
	{/// 2.00<|eta|<2.10
		 {1.812, 0.210, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.065, 0.134, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.592, 0.175, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.115, 0.167, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 2.00<|eta|<2.10
								},/// pt=10
{/// pt=12
	{/// 1.20<|eta|<1.40
		 {4.365, 0.372, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {1.587, 0.332, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.897, 0.309, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.654, 0.362, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.20<|eta|<1.40
	{/// 1.40<|eta|<1.60
		 {4.126, 0.229, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {1.637, 0.282, 3.037, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.924, 0.242, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.679, 0.304, 0.000, -0.000, -0.000 },/// npar=3,even,odd
							},/// 1.40<|eta|<1.60
	{/// 1.60<|eta|<1.80
		 {3.006, 0.255, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.638, 0.070, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.594, 0.251, 0.070, -0.000, 0.000 },/// npar=2,even,even
		 {1.599, 0.151, 0.035, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.60<|eta|<1.80
	{/// 1.80<|eta|<2.00
		 {2.164, 0.176, 0.000, -0.000, -0.000 },/// npar=0,odd,even
		 {1.252, 0.098, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.830, 0.197, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.284, 0.151, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.80<|eta|<2.00
	{/// 2.00<|eta|<2.10
		 {1.970, 0.159, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.038, 0.137, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.613, 0.150, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.016, 0.219, 3.107, -0.000, 0.000 },/// npar=3,even,odd
							},/// 2.00<|eta|<2.10
								},/// pt=12
{/// pt=15
	{/// 1.20<|eta|<1.40
		 {4.305, 0.351, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {1.336, 0.333, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.653, 0.330, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {1.527, 0.284, 0.000, 0.000, -0.000 },/// npar=3,even,odd
							},/// 1.20<|eta|<1.40
	{/// 1.40<|eta|<1.60
		 {4.993, 0.124, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {1.298, 0.349, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.452, 0.243, 0.000, -0.000, -0.000 },/// npar=2,even,even
		 {1.541, 0.307, 0.000, -0.000, -0.000 },/// npar=3,even,odd
							},/// 1.40<|eta|<1.60
	{/// 1.60<|eta|<1.80
		 {3.482, 0.229, 0.070, -0.000, 0.000 },/// npar=0,odd,even
		 {1.354, 0.114, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.176, 0.263, 0.070, -0.000, 0.000 },/// npar=2,even,even
		 {1.430, 0.138, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.60<|eta|<1.80
	{/// 1.80<|eta|<2.00
		 {2.035, 0.182, 0.000, -0.000, -0.000 },/// npar=0,odd,even
		 {1.148, 0.122, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.868, 0.207, 0.070, -0.000, 0.000 },/// npar=2,even,even
		 {1.298, 0.156, 3.107, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.80<|eta|<2.00
	{/// 2.00<|eta|<2.10
		 {1.899, 0.161, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.059, 0.122, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.409, 0.192, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.025, 0.206, 3.107, -0.000, 0.000 },/// npar=3,even,odd
							},/// 2.00<|eta|<2.10
								},/// pt=15
{/// pt=20
	{/// 1.20<|eta|<1.40
		 {4.514, 0.389, 0.035, 0.000, -0.000 },/// npar=0,odd,even
		 {1.248, 0.341, 0.070, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.038, 0.316, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.288, 0.353, 0.000, -0.000, -0.000 },/// npar=3,even,odd
							},/// 1.20<|eta|<1.40
	{/// 1.40<|eta|<1.60
		 {4.079, 0.184, 0.000, 0.000, -0.000 },/// npar=0,odd,even
		 {1.110, 0.302, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {2.006, 0.243, 0.000, -0.000, -0.000 },/// npar=2,even,even
		 {1.492, 0.299, 0.000, -0.000, -0.000 },/// npar=3,even,odd
							},/// 1.40<|eta|<1.60
	{/// 1.60<|eta|<1.80
		 {3.014, 0.260, 0.070, -0.000, 0.000 },/// npar=0,odd,even
		 {1.234, 0.103, 0.000, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.724, 0.253, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.322, 0.148, 0.035, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.60<|eta|<1.80
	{/// 1.80<|eta|<2.00
		 {2.385, 0.192, 0.070, -0.000, -0.000 },/// npar=0,odd,even
		 {1.116, 0.108, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.731, 0.128, 0.035, -0.000, 0.000 },/// npar=2,even,even
		 {1.170, 0.154, 0.000, -0.000, 0.000 },/// npar=3,even,odd
							},/// 1.80<|eta|<2.00
	{/// 2.00<|eta|<2.10
		 {1.957, 0.154, 0.035, -0.000, 0.000 },/// npar=0,odd,even
		 {1.006, 0.132, 0.035, -0.000, 0.000 },/// npar=1,odd,odd
		 {1.403, 0.184, 0.000, -0.000, 0.000 },/// npar=2,even,even
		 {1.060, 0.182, 3.107, -0.000, 0.000 },/// npar=3,even,odd
							},/// 2.00<|eta|<2.10
								},/// pt=20
	}; ///hybridLUT end

};//namespace
#endif
