#ifndef SUNPOSITION_H
#define SUNPOSITION_H

/*--------------------------------------------------------------------------------------
                              SUN POSITION
                            OUTPUT variables
----------------------------------------------------------------------------------------
float amass;            Relative optical airmass
float ampress;          Pressure-corrected airmass
float azim;             Solar azimuth angle:  N=0, E=90, S=180, W=270
float cosinc;           Cosine of solar incidence angle on panel
float coszen;           Cosine of refraction corrected solar zenith angle
float elevetr;          Solar elevation, no atmospheric correction (= ETR)
float elevref;          Solar elevation angle, deg. from horizon, refracted
float etr;              Extraterrestrial (top-of-atmosphere)
                        W/sq m global horizontal solar irradiance
float etrn;             Extraterrestrial (top-of-atmosphere)
                        W/sq m direct normal solar irradiance
float etrtilt;          Extraterrestrial (top-of-atmosphere)
                        W/sq m global irradiance on a tilted surface
float prime;            Factor that normalizes Kt, Kn, etc.
float sbcf;             Shadow-band correction factor
float sunrise;          Sunrise time, minutes from midnight, local, WITHOUT refraction
float sunset;           Sunset time, minutes from midnight, local, WITHOUT refraction
float unprime;          Factor that denormalizes Kt', Kn', etc.
float zenref;           Solar zenith angle, deg. from zenith, refracted
----------------------------------------------------------------------------------------*/


long RSUN_compute_solar_position (
                    float longitude, float latitude, int timezone,
                    int year, int month, int day, int hour, int minute, int second,
                    float temp, float press, float aspect, float tilt,
                    float sbwid, float sbrad, float sbsky);

void RSUN_get_results (
                    float *amass, float *ampress, float *azim,
                    float *cosinc, float *coszen, float *elevetr, float *elevref,
                    float *etr, float *etrn, float *etrtilt, float *prime, float *sbcf,
                    float *sunrise, float *sunset, float *unprime, float *zenref);

#endif // SUNPOSITION_H
