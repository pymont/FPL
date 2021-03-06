#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdio.h>

#include <ceigen.h>
#include <cgeom/CGeom.h>
#include <cgeom/generate_scene.h>
#include <cgeom/objpose.h>
#include <cgeom/objpose_weighted.h>

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>

#include <Misc.h>

using namespace std;
using namespace CEIGEN;
using namespace CGEOM;

////////////////////////////////////////////////////////////////////////////////
string OPT_ID_TO_STRING( const int nOptID ) {
    switch( nOptID ) {
    case 0:
        return "LU";
    case 1:
        return "LU-DIST";
    case 2:
        return "LU-ROB";
    case 3:
        return "LU-DIST-ROB";
    default:
        cerr << "ERROR in OPT_ID_TO_STRING, unknown opt type";
        return "";
    }
}

////////////////////////////////////////////////////////////////////////////////
int main( int /*argv*/, char** /*argc*/ ) {
    int nSeed = 0;
    srand( nSeed );

    const string sDataPath = "./Data/";

    const int nNumOpts = 4;
    const int nNumTests = 7;
    const int nNumOutlierTests = 6;
    int aOutliers[nNumOutlierTests] = 
        {0,10,20,30,40,50};

    // Define simulation parameters
    SceneGeneratorOptions sc_opts;
    sc_opts.dMinZ = 3;
    sc_opts.dMaxZ = 4;
    const int nNumPoints = sc_opts.nNumPoints;
    Eigen::Vector2d mAdd = 100 * Eigen::VectorXd::Ones(2);
   
    Eigen::Matrix3d mKinv = sc_opts.MakeCameraMatrix().inverse();
    Eigen::MatrixXd mMeasNP; // normalised plane

    // Generate random image points
    Eigen::MatrixXd mP3D( 3, sc_opts.nNumPoints );
    Eigen::MatrixXd mMeasT( 2, sc_opts.nNumPoints );
    Eigen::MatrixXd mMeasN( 2, sc_opts.nNumPoints );

    Eigen::Vector3d mt;
    Eigen::Matrix3d mR;

    // Minimisation settings
    const int nMaxNumIters = 100;
    const double dTol      = 1e-5;
    const double dEpsilon  = 1e-8;
    const double dExpectedNoiseStd = 2. * mKinv(0,0);
    Eigen::Matrix3d mREst;
    Eigen::Vector3d vtEst;
    int nNumIterations = 0;
    double dObjError;
    vector<bool> vInliers( mP3D.cols(), false );

    for( int nOptID=0; nOptID<nNumOpts; nOptID++ ) {
        for( int nOutlierIt=0; nOutlierIt<nNumOutlierTests; nOutlierIt++ ) {
            printf( "\r%s: %i/%i",
                    OPT_ID_TO_STRING( nOptID ).c_str(),
                    nOutlierIt+1, nNumOutlierTests );
            fflush( stdout );

            int nOutlierPerc = aOutliers[nOutlierIt];

            stringstream ss;
            ss << sDataPath << "outlier_" 
               << OPT_ID_TO_STRING( nOptID ) << "_"
               << "z" << setw(4) << setfill( '0' ) << sc_opts.dMinZ 
               << "_"
               << "z" << setw(4) << setfill( '0' ) << sc_opts.dMaxZ 
               << "_"
               << "n" << setw(5) << setprecision(3) 
               << std::setiosflags( std::ios::fixed ) 
               << setfill( '0' ) << sc_opts.dNoise
               << "_"
               << "o" << setw(2) << setfill( '0' ) << nOutlierPerc
               << ".txt";
            ofstream f( ss.str().c_str() );
            if( !f.good() ) {
                cerr << endl << "Problem opening file: " << ss.str() << endl;
                return -1;
            }
            for( int nTest=0; nTest<nNumTests; nTest++ ) {
                if( !generate_scene_trans( sc_opts, mP3D, mMeasT, mMeasN,
                                           mR, mt ) ) {
                    cerr << "Problem generating scene" << endl;
                    return -1;
                }
                mMeasNP = 
                    CEIGEN::metric( mKinv * CEIGEN::projective( mMeasN ) );

                // Add outliers
                const int nNumOutliers = nOutlierPerc*nNumPoints/100;
                for( int ii=0; ii<nNumOutliers; ii++ ) {
                    mMeasNP.col(ii) = mMeasNP.col(ii) + mAdd;
                }

                mREst = Eigen::Matrix3d::Identity();
                vtEst = Eigen::Vector3d::Zero();

                switch( nOptID ) {
                case 0:
                    CGEOM::objpose_weighted<CGEOM::OneWeights>
                        ( mP3D, mMeasNP,
                          nMaxNumIters, dTol, dEpsilon,
                          dExpectedNoiseStd,
                          mREst, vtEst, nNumIterations, dObjError, 
                          vInliers, true, false );
                    break;
                case 1:
                    CGEOM::objpose_weighted<CGEOM::InvSqDistWeights>
                        ( mP3D, mMeasNP,
                          nMaxNumIters, dTol, dEpsilon,
                          dExpectedNoiseStd,
                          mREst, vtEst, nNumIterations, dObjError, 
                          vInliers, true, false );
                    break;
                case 2:
                    CGEOM::objpose_weighted<CGEOM::TukeyWeights>
                        ( mP3D, mMeasNP,
                          nMaxNumIters, dTol, dEpsilon,
                          dExpectedNoiseStd,
                          mREst, vtEst, nNumIterations, dObjError, 
                          vInliers, true, false );
                    break;
                case 3:
                    CGEOM::objpose_weighted<CGEOM::TukeyInvSqDistWeights>
                        ( mP3D, mMeasNP,
                          nMaxNumIters, dTol, dEpsilon,
                          dExpectedNoiseStd,
                          mREst, vtEst, nNumIterations, dObjError, 
                          vInliers, true, false );
                    break;
                default:
                    cerr << "ERROR in OPT_ID_TO_STRING, unknown opt type";
                    return -1;
                }

                f << (vtEst-mt).norm() << endl;
            }
            f.close();
        }
        cout << endl;
    }
    cout << endl;

    return 0;
}
