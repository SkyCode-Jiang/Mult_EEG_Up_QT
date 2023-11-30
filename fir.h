#ifndef FIR_H
#define FIR_H
#include <window.h>
#include <iostream>
#include <iomanip>
#include <QTextStream>
#include <dfd.h>


#define PI 3.1415926


namespace splab
{

    using std::setw;
    using std::ios;
    using std::setiosflags;
    using std::setprecision;


    class FIR : public DFD
    {

     public:

        FIR( const QString &select, const QString &win );
        FIR( const QString &select, const QString &win, double a );
        ~FIR();

        void    design();
        void    dispInfo() const;
        QVector<double> getCoefs() const;

   //private:

        void    orderEst();
        void    idealCoef();
        void    calcCoef();
        double  frqeResp( double freq );
        void    calcGain();
        bool    isSatisfy();

        QString  windType;           // window type

//        QVector<double>  wind,       // window function
//                        coefs,      // coefficients
//                        edgeGain;   // gain at edge frquency
        double  alpha;              // window parameter
        QVector<double> wind;
        QVector<double> coefs;
        QVector<double> edgeGain;

        double test = 0;
        int  test_f = 0;

    };


    // class FIR
    //构造FIR类


    /**
     * constructors and destructor
     */

    //声明滤波器形式+声明窗函数
    FIR::FIR( const QString &select, const QString &win )
            : DFD( select ), windType(win)
    {
        bool cond = ( filtType=="lowpass" || filtType=="highpass" ||
                      filtType=="bandpass" || filtType=="bandstop" );

        //assert如果它里面的条件返回错误，代码会终止运行，并且会把源文件，错误的代码，以及行号，都输出来
        assert(cond);

        cond = ( windType=="Rectangle" || windType=="Bartlett" ||
                 windType=="Blackman" || windType=="Hanning" ||
                 windType=="Hamming" || windType=="Gauss" );

        //assert如果它里面的条件返回错误，代码会终止运行，并且会把源文件，错误的代码，以及行号，都输出来
        assert(cond);
    }


    FIR::FIR( const QString &select, const QString &win, double a )
            : DFD( select ), windType(win), alpha(a)
    {
        bool cond = ( filtType=="lowpass" || filtType=="highpass" ||
                      filtType=="bandpass" || filtType=="bandstop" );
        assert(cond);

        cond = ( windType=="Kaiser" || windType=="Gauss" );
        assert(cond);
    }

    FIR::~FIR()
    {
    }


    /**
     * Design digital FIR filter.
     */
    void FIR::design()
    {
        // Estimate the length of filter.
        orderEst();
        calcCoef();
        calcGain();

//        while( !isSatisfy() )
//        {
//            order += 4;
//            calcCoef();
//            calcGain();
//        }
    }



//    /**
//     * Displays the design result of filter.
//     */
    void FIR::dispInfo() const
    {

    }


    /**
     * Get the FIR filter's coefficients.
     */
    inline QVector<double> FIR::getCoefs() const
    {
        return coefs;
    }


    /**
     * Estimates the length of FIR filter.
     */
    void FIR::orderEst()
    {
        double  deltaFreq,  // delta frequency
                lowerDF,    // delta freq of lower band
                upperDF,    // delta freq of upper band
                errSB,      // stopband error
                errPB,      // passband error
                errMin,     // minimum of sb/pb errors
                errDB;      // minimum error in dB

        // Determine frequency delta
        if( filtType=="lowpass" || filtType=="highpass" )
            deltaFreq = abs(wstop1-2) / fsamp;
        else
        {
            lowerDF = abs(wstop1-wpass1) / fsamp;
            upperDF = abs(wstop2-wpass2) / fsamp;

            if( lowerDF > upperDF )
                deltaFreq = upperDF;
            else
                deltaFreq = lowerDF;
        }

        // Determine stopband and passband errors
        //pow 计算x的y次幂

        errPB = 1 - pow( 10, 0.05*apass1 ); //passband errors
        errSB = pow( 10, 0.05*astop1 ); //stopband errors

        if( errSB < errPB )
            errMin = errSB;
        else
            errMin = errPB;
        errDB = -20 * log10(errMin);
        test = errDB;

        // Store filter length in pFilt and return beta.
        if( errDB > 21 )
        {
           order = int( ceil( 1 + (errDB-7.95) / (2.285*deltaFreq) ) );
           //test_f = 0;
        }

        else
        {
            order = int( ceil( 1 + (5.794/deltaFreq) ) );
            //test_f = 1;
        }

        order += 4 - order%4;
        //test = order;
    }



    /**
     * Calculates ideal FIR coefficients.
     */
    void FIR::idealCoef()
    {
        int     i;
        double  t,
                tau,
                Wc1,
                Wc2;

        // Calculate tau as non-integer if order is even.
        tau =  ( order - 1 ) / 2.0;

        // Adjust cutoff frequency to midway point between stop
        // and pass edge frequency and convert to digital frequency.
        Wc1 = (wstop1 + wpass1) / (2*fsamp);
        Wc2 = (wstop2 + wpass2) / (2*fsamp);

        // Calc coefs based on selectivity of filter.
        if( filtType == "lowpass" )
            for( i=0; i<order; ++i )
            {
                if( i == tau )
                    coefs[i] = Wc1/PI;
                else
                {
                    t = i - tau;
                    coefs[i] = sin(Wc1*t) / (PI*t);
                }
            }
        else if( filtType == "highpass" )
            for( i=0; i<order; ++i )
            {
                if( i == tau )
                    coefs[i] = (PI-Wc1) / PI;
                else
                {
                    t = i - tau;
                    coefs[i] = ( sin(PI*t) - sin(Wc1*t) ) / (PI*t);
                }
            }
        else if( filtType == "bandpass" )
            for( i=0; i<order; ++i )
            {
                if( i == tau )
                    coefs[i] = ( Wc2-Wc1 ) / PI;
                else
                {
                    t = i - tau;
                    coefs[i] = ( sin(Wc2*t) - sin(Wc1*t) ) / (PI*t);
                }
            }
        else
            for( i=0; i<order; ++i )
            {
                if( i == tau )
                    coefs[i] = ( PI+Wc1-Wc2 ) / PI;
                else
                {
                    t = i - tau;
                    coefs[i] = ( sin(PI*t)-sin(Wc2*t)+sin(Wc1*t) ) / (PI*t);
                }
            }
    }


    /**
     * Calculates the digital FIR coefficients.
     */
    void FIR::calcCoef()
    {
        coefs.resize(order); //系数
        wind.resize(order); //窗函数

        //  Calculate the ideal FIR coefficients.
        idealCoef();

        // Get window function.
        if( windType=="Kaiser" || windType=="Gauss" )
            wind = windowx( windType, order, alpha, 1.0 );
        else
            wind = windowx( windType, order, 1.0 );

        //  Multiply window and ideal coefficients.
        for(auto i =0; i<order; ++i)
        {
            coefs[i] = coefs[i] * wind[i];
        }

    }


    /**
     * Calculate the response at edge freqency.
     */
    double FIR::frqeResp( double freq )
    {
        double  mag = 1.0,
                rea = 0.0,
                img = 0.0,
                omega = TWOPI*freq / fsamp;

        // Loop through all the coefficients.
        for( int i=0; i<order; ++i )
        {
            double  domega = i * omega;
            rea += coefs[i] * cos(domega);
            img += coefs[i] * sin(domega);
        }

        // Calculate final result and convert to degrees.
        mag = sqrt( rea*rea + img*img );
        if( mag < EPS )
            mag = EPS;
        mag = 20 * log10( mag );

        return mag;
    }


    /**
     * Calculate the response at edge freqency.
     */
    void FIR::calcGain()
    {
        // Determine the edge freqency.
        if( filtType=="lowpass" || filtType=="highpass" )
        {
            double  f1 = wpass1 / TWOPI,
                    f2 = wstop1 / TWOPI;

            //edgeGain.resize(2);
            edgeGain.append(frqeResp( f1 ));
            edgeGain.append(frqeResp( f2 ));
        }
        else
        {
            double  f1 = wpass1 / TWOPI,
                    f2 = wpass2 / TWOPI,
                    f3 = wstop1 / TWOPI,
                    f4 = wstop2 / TWOPI;

            //edgeGain.resize(4);
            edgeGain.append(frqeResp( f1 ));
            edgeGain.append(frqeResp( f2 ));
            edgeGain.append(frqeResp( f3 ));
            edgeGain.append(frqeResp( f4 ));
        }
    }


    /**
     * Design digital FIR filter.
     */
    bool FIR::isSatisfy()
    {
        if( edgeGain.size() == 2 )
        {
            if( edgeGain.at(0)<apass1 || edgeGain.at(1)>astop1 )
                return false;
        }
        else
        {
            if( edgeGain.at(0)<apass1 || edgeGain.at(1)<apass2 ||
                edgeGain.at(2)>astop1 || edgeGain.at(3)>astop2 )
                return false;
        }

        return true;
    }

}
#endif // FIR_H
