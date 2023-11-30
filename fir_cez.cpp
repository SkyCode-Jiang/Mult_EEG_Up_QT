#include "fir_cez.h"


QVector<double> blackman( int N, double amp )
{
    QVector<double> win(N);

    for( int i=0; i<(N+1)/2; ++i )
    {
        win[i] = amp * double ( 0.42 - 0.50*cos(TWOPI*i/(N-1.0))
                              + 0.08*cos(2*TWOPI*i/(N-1.0)) );
        win[N-1-i] = win[i];
    }

    return win;
}
