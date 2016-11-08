#ifndef GAMMAFUNCTION
#define GAMMAFUNCTION


    static long double xSmall_x(long double x, long double nu);
    static long double xMedium_x(long double x, long double nu);
    static long double xLarge_x(long double x, long double nu);

    double Entire_Incomplete_Gamma_Function(double x, double nu);
    long double xEntire_Incomplete_Gamma_Function(long double x, long double nu);

    double Factorial(int n);
    long double xFactorial(int n);
    int Factorial_Max_Arg( void );

    double Gamma_Function(double x);
    long double xGamma_Function(long double x);
    double Gamma_Function_Max_Arg( void );
    long double xGamma_Function_Max_Arg( void );

    static long double xGamma(long double x);
    static long double Duplication_Formula( long double two_x );

    double Incomplete_Gamma_Function(double x, double nu);
    long double xIncomplete_Gamma_Function(long double x, long double nu);

    double Ln_Gamma_Function(double x);
    long double xLn_Gamma_Function(long double x);

    static long double xLnGamma_Asymptotic_Expansion( long double x );


#endif // GAMMAFUNCTION

