#ifndef SNOWPARAM_H
#define SNOWPARAM_H

    //Junsei Kondo, Hiromi Yamazawa, Measurement of snow surface emissivity
    #define SNOW_EMISSIVITY 0.97f               //[-]
    #define SOIL_EMISSIVITY 0.92f               //[-] soil (average)

    #define THERMO_WATER_VAPOR_CONST 0.4615f    //[J/(kg °K)]

    #define SOIL_SPECIFIC_HEAT 2.1f             //[KJ/kg/°C]

    #define DEFAULT_BULK_DENSITY 1300           //[kg/m^3]

// FAUSTO: Da qui in avanti devono diventare modificabili (es: variabili di snowPoint, con questi valori di default)

    #define SNOW_SKIN_THICKNESS 0.02f           //[m]   ??? VARIE VERSIONI IN BROOKS: 3mm (nel testo), 2-3cm (nel codice) ???
    #define SNOW_MINIMUM_HEIGHT 2               //[mm]  FAUSTO: verificare se doppione con questo parametro (ci guardo io)

    #define SOIL_DAMPING_DEPTH 0.3f             //[m]
    #define SNOW_DAMPING_DEPTH 0.05f            //[m]

    #define SOIL_ALBEDO 0.2f                    //[-] bare soil - 20%
    #define SNOW_VEGETATION_HEIGHT 1            //[m] height of vegetation

    // percentuale di acqua libera che il manto nevoso può trattenere
    #define SNOW_WATER_HOLDING_CAPACITY 0.05f   //[-]

    //acqua libera (torrenti, laghetti)
    #define SNOW_MAX_WATER_CONTENT 0.1f //[m]

    // Valore originale (tesi Brooks): 0.5 gradi, 3 gradi un anno (problema inv. termica?)
    #define TEMP_MAX_WITH_SNOW 2                //[°C]
    #define TEMP_MIN_WITH_RAIN -0.5f            //[°C]

#endif // SNOWPARAM_H
