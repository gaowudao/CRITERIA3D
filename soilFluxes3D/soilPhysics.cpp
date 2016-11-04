/*-----------------------------------------------------------------------------------

    CRITERIA 3D
    Copyright (C) 2011 Fausto Tomei, Alberto Pistocchi, Marco Bittelli,
    Gabriele Antolini, Antonio Volta

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under a contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    ftomei@arpa.emr.it
    alberto.pistocchi@gecosistema.it
    marco.bittelli@unibo.it
    gantolini@arpa.emr.it
    avolta@arpa.emr.it

-----------------------------------------------------------------------------------*/

#include <math.h>
#include "header/types.h"
#include "header/soilPhysics.h"


	double theta_from_Se (unsigned long myIndex)
	//------------------------------------------------------------------------------
    // Compute volumetric water content from current degree of saturation
	//------------------------------------------------------------------------------
	{
		return ((myNode[myIndex].Se * (myNode[myIndex].Soil->Theta_s - myNode[myIndex].Soil->Theta_r)) + myNode[myIndex].Soil->Theta_r);
	}

	double theta_from_Se (double Se, unsigned long myIndex)
	//------------------------------------------------------------------------------
	// Compute volumetric water content from degree of saturation
	//------------------------------------------------------------------------------
	{
		return ((Se * (myNode[myIndex].Soil->Theta_s - myNode[myIndex].Soil->Theta_r)) + myNode[myIndex].Soil->Theta_r);
	}

    double theta_from_sign_Psi (double signPsi, unsigned long index)
	//---------------------------------------------------------------------------
	// Compute volumetric water content from water potential (with sign)
	//---------------------------------------------------------------------------
	{
        if (myNode[index].isSurface) return 1.;

        if (signPsi >= 0.0)
        {
            //saturated
            return myNode[index].Soil->Theta_s;
        }
		else
        {
            double Se = computeSefromPsi(fabs(signPsi),myNode[index].Soil);
            return theta_from_Se(Se, index);
        }
	}


	double Se_from_theta (unsigned long myIndex, double theta)
	//------------------------------------------------------------------------
	// Compute degree of saturation from volumetric water content
	//------------------------------------------------------------------------
	{
		// check range
		if (theta >= myNode[myIndex].Soil->Theta_s) return(1.);
		else if (theta <= myNode[myIndex].Soil->Theta_r) return(0.);
		else return ((theta - myNode[myIndex].Soil->Theta_r) / (myNode[myIndex].Soil->Theta_s - myNode[myIndex].Soil->Theta_r));
	}

    double computeSefromPsi(double myPsi, Tsoil *mySoil)
	//------------------------------------------------------------------------------
	// compute degree of saturation from matric potential (Van Genutchen)
	//
	// Se = [1+(alfa*|h|)^n]^-m
	//------------------------------------------------------------------------------
	{
		double Se = NODATA;

		if (myParameters.waterRetentionCurve == MODIFIEDVANGENUCHTEN)
			if (myPsi <=  mySoil->VG_he)
				Se = 1.;
			else
				{
                Se = pow(1. + pow(mySoil->VG_alpha * myPsi, mySoil->VG_n), - mySoil->VG_m);
				Se *= (1. / mySoil->VG_Sc);
				}
		else if (myParameters.waterRetentionCurve == VANGENUCHTEN)
            Se = pow(1. + pow(mySoil->VG_alpha * myPsi, mySoil->VG_n), - mySoil->VG_m);

		return Se;
	}

    double computeSe(unsigned long myIndex)
    //------------------------------------------------------------------------------
    // Compute current degree of saturation
    //------------------------------------------------------------------------------
    {
        // saturated
        if (myNode[myIndex].H >= myNode[myIndex].z) return 1.;

        double psi = fabs(myNode[myIndex].H - myNode[myIndex].z);   //[m]

        return computeSefromPsi(psi, myNode[myIndex].Soil);
    }


    double computeWaterConductivity(double Se, Tsoil *mySoil)
	//------------------------------------------------------------------------------
	// Compute hydraulic conductivity [m/sec]  (Mualem)
	// K(Se) = Ksat * Se^(L) * {1-[1-Se^(1/m)]^m}^2
    // WARNING: very low values are possible (es: 10^12)
	//------------------------------------------------------------------------------
	{
		if (Se >= 1.) return(mySoil->K_sat );

		double myTmp = NODATA;
		if (myParameters.waterRetentionCurve == MODIFIEDVANGENUCHTEN)
		{
			double myNumerator = 1. - pow(1. - pow(Se*mySoil->VG_Sc, 1./mySoil->VG_m), mySoil->VG_m);
			myTmp = myNumerator / (1. - pow(1. - pow(mySoil->VG_Sc, 1./mySoil->VG_m), mySoil->VG_m));
		}
		else if (myParameters.waterRetentionCurve == VANGENUCHTEN)
			myTmp = 1. - pow(1. - pow(Se, 1./mySoil->VG_m), mySoil->VG_m);

		return (mySoil->K_sat * pow(Se, mySoil->Mualem_L) * pow(myTmp , 2.));
	}


    double compute_K_Mualem(double Ksat, double Se, double VG_Sc, double, double VG_m, double Mualem_L)
	//------------------------------------------------------------------------------
	// Compute hydraulic conductivity [m/sec]  (Mualem)
	// K(Se) = Ksat * Se^(L) * {1-[1-Se^(1/m)]^m}^2
    // WARNING: very low values are possible (es: 10^12)
	//------------------------------------------------------------------------------
	{
		if (Se >= 1.) return(Ksat);
		double temp= NODATA;

		if (myParameters.waterRetentionCurve == MODIFIEDVANGENUCHTEN)
        {
            double num = 1. - pow(1. - pow(Se*VG_Sc, 1./VG_m), VG_m);
            temp = num / (1. - pow(1. - pow(VG_Sc, 1./VG_m), VG_m));
        }
		else if (myParameters.waterRetentionCurve == VANGENUCHTEN)
        {
			temp = 1. - pow(1. - pow(Se, 1./VG_m), VG_m);
        }

		return (Ksat * pow(Se, Mualem_L) * pow(temp , 2.));
	}


    double computeK(unsigned long myIndex)
	//------------------------------------------------------------------------------
    // Compute current soil water conductivity [m sec^-1]
	//------------------------------------------------------------------------------
    {
		return(compute_K_Mualem(myNode[myIndex].Soil->K_sat, myNode[myIndex].Se, myNode[myIndex].Soil->VG_Sc, myNode[myIndex].Soil->VG_n,  myNode[myIndex].Soil->VG_m, myNode[myIndex].Soil->Mualem_L));
    }


    double psi_from_Se(unsigned long myIndex)
	//------------------------------------------------------------------------------
	// compute Water Potential from degree of saturation
	//------------------------------------------------------------------------------
	{
		double m = myNode[myIndex].Soil->VG_m;
		double temp = NODATA;

		if (myParameters.waterRetentionCurve == MODIFIEDVANGENUCHTEN)
				temp = pow(1./ (myNode[myIndex].Se * myNode[myIndex].Soil->VG_Sc) , 1./ m ) - 1.;
        else if (myParameters.waterRetentionCurve == VANGENUCHTEN)
				temp = pow(1./ myNode[myIndex].Se, 1./ m ) - 1.;

        return((1./ myNode[myIndex].Soil->VG_alpha) * pow(temp, 1./ myNode[myIndex].Soil->VG_n));
	}


	double dTheta_dH(unsigned long myIndex)
	//---------------------------------------------------------------------------------
	// Compute dTheta/dH  (Van Genutchen)
	//
	// dTheta/dH = dSe/dH (Theta_s-Theta_r)
	//
	// dSe/dH = -sgn(H-z) alfa n m [1+(alfa|(H-z)|)^n]^(-m-1) (alfa|(H-z)|)^n-1
	//---------------------------------------------------------------------------------
	 {
     double alfa = myNode[myIndex].Soil->VG_alpha;
	 double n    = myNode[myIndex].Soil->VG_n;
	 double m    = myNode[myIndex].Soil->VG_m;
	 double dSe_dH;

     double psi = fabs(minValue(myNode[myIndex].H - myNode[myIndex].z, 0.));
     double psiPrevious = fabs(minValue(myNode[myIndex].oldH - myNode[myIndex].z, 0.));

	if (myParameters.waterRetentionCurve == MODIFIEDVANGENUCHTEN)
		{ if ((psi <= myNode[myIndex].Soil->VG_he) && (psiPrevious <= myNode[myIndex].Soil->VG_he)) return 0.;}
	else if (myParameters.waterRetentionCurve == VANGENUCHTEN)
		{ if ((psi == 0.) && (psiPrevious == 0.)) return 0.;}

	 if (psi == psiPrevious)
			{
			dSe_dH = alfa * n * m * pow(1. + pow(alfa * psi, n), -(m + 1.)) * pow(alfa * psi, n - 1.);
			if (myParameters.waterRetentionCurve == MODIFIEDVANGENUCHTEN)
					dSe_dH *= (1. / myNode[myIndex].Soil->VG_Sc);
			}
	 else
			{
            double theta = computeSefromPsi(psi, myNode[myIndex].Soil);
            double thetaPrevious = computeSefromPsi(psiPrevious, myNode[myIndex].Soil);
			double delta_H = myNode[myIndex].H - myNode[myIndex].oldH;
			dSe_dH = fabs((theta - thetaPrevious) / delta_H);
			}

	 return (dSe_dH * (myNode[myIndex].Soil->Theta_s - myNode[myIndex].Soil->Theta_r));
	 }


    double getThetaMean(long i)
	{
		double myHMean = myNode[i].oldH * 0.5 + myNode[i].H * 0.5;

		if (myNode[i].isSurface)
		{
            double mySurfaceWater = maxValue(myHMean - myNode[i].z, 0.);		//[m]
            return (minValue(mySurfaceWater / 0.01, 1.));
		}
		else
		{
			// sub-surface
			double myPsiMean = myHMean - myNode[i].z;
            return (theta_from_sign_Psi(myPsiMean, i ));
		}
	}


    double getPsiMean(long i)
	{
		double meanH = myNode[i].oldH * 0.5 + myNode[i].H * 0.5;

		if (myNode[i].isSurface)
		{
            double mySurfaceWater = maxValue(meanH - myNode[i].z, 0.);		//[m]
			if (mySurfaceWater > 0.000001)
				return (0.);
			else
                return (minValue(mySurfaceWater / 0.01, 1.));
		}
		else
            return minValue(0., meanH - myNode[i].z);
	}


    double getHMean(long i)
    { return (myNode[i].oldH * 0.5 + myNode[i].H * 0.5);}
