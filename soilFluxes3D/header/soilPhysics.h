
struct Tsoil;

double computeWaterConductivity(double Se, Tsoil *mySoil);

double computeSefromPsi(double myPsi, Tsoil *mySoil);

double theta_from_Se(unsigned long myIndex);

double theta_from_Se (double Se, unsigned long myIndex);

double theta_from_sign_Psi (double myPsi, unsigned long myIndex);

double Se_from_theta (unsigned long myIndex, double myTheta);

double psi_from_Se(unsigned long myIndex);

double computeSe(unsigned long myIndex);

double dTheta_dH(unsigned long myIndex);

double computeK(unsigned long myIndex);

double compute_K_Mualem(double Ksat, double Se, double VG_Sc, double VG_m, double Mualem_L);

double getThetaMean(long i);

double getHMean(long i);

double getPsiMean(long i);
