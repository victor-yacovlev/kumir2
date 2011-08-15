//
// C++ Implementation: integeroverflowchecker
//
// Description:
//
//
// Author: Виктор Яковлев <victor@lpm.org.ru>, (C) 2008-2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "integeroverflowchecker.h"
#include <cmath>

namespace StdLib {

IntegerOverflowChecker::Architecture IntegerOverflowChecker::m_architecture = Always32Bit;
uint IntegerOverflowChecker::n = 32;
int IntegerOverflowChecker::power2inNminus2 = 1073741824;

bool IntegerOverflowChecker::checkSumm(const int first, const int second)
{
        double u = first / 2.;
        double w = second / 2.;
        return (-power2inNminus2<u+w) && (u+w<power2inNminus2);
}

bool IntegerOverflowChecker::checkDiff(const int first, const int second)
{
        double u = first / 2.;
        double w = (-second) / 2.;
        return (-power2inNminus2<u+w) && (u+w<power2inNminus2);
}

bool IntegerOverflowChecker::checkProd(const int first, const int second)
{
        if ( first>0 && second>0 ) {
                qreal u = log_2(first);
                qreal w = log_2(second);
                return u+w<n-1;
        }
        else if ( first<0 && second<0 ) {
                qreal u = log_2(-first);
                qreal w = log_2(-second);
                return u+w<n-1;
        }
        else if ( first>0 && second<0 ) {
                qreal u = log_2(first);
                qreal w = log_2(-second);
                return u+w<n-1;
        }
        else if ( first<0 && second>0 ) {
                qreal u = log_2(-first);
                qreal w = log_2(second);
                return u+w<n-1;
        }
        else
                return true;
}

bool IntegerOverflowChecker::checkPower(const int first, const uint second)
{
        if ( first==0 || second==0 )
                return true;
        else if ( first>0 ) {
                qreal u = log_2(first);
                return u*second < n-1;
        }
        else {
                qreal u = log_2(-first);
                return u*second < n-1;
        }
}

bool IntegerOverflowChecker::checkFromReal(const qreal value)
{
  return ( value >= -pow(2.0,(double)(n-1)) ) && ( value <= pow(2.0,(double)(n-1))-1 );
}

bool IntegerOverflowChecker::checkFromString(const QString & value)
{
        QString tmp = value;
        bool isNegative = value.startsWith("-");
        if (isNegative)
                tmp.remove(0,1);
        bool isHex = tmp.startsWith("$") || tmp.startsWith("0x");
        if (tmp.startsWith("$"))
                tmp.remove(0,1);
        else if (tmp.startsWith("0x"))
                tmp.remove(0,2);
        while ( tmp.startsWith("0") )
                tmp.remove(0,1);
        if ( n==32 && isHex ) {
                // 2^31 = 80'000'000
                if (tmp.length()<8)
                        return true;
                if (tmp.length()>8)
                        return false;
                if ( isNegative )
                        return tmp < QString("80000000");
                else
                        return tmp < QString("80000000");
        }
        else if ( n==32 ) {
                // 2^31 = 2'147'483'648
                if (tmp.length()<10)
                        return true;
                if (tmp.length()>10)
                        return false;
                if ( isNegative )
// 			return tmp <= QString("2147483648");
                        return tmp < QString("2147483648");
                else
                        return tmp < QString("2147483648");
        }
        else if ( n==64 && isHex ) {
                // 2^63 = 8'000'000'000'000'000
                if (tmp.length()<16)
                        return true;
                if (tmp.length()>16)
                        return false;
                if ( isNegative )
                        return tmp < QString("8000000000000000");
                else
                        return tmp < QString("8000000000000000");
        }
        else /* n==64 */ {
                // 2^63 = 9'223'372'036'854'775'808
                if (tmp.length()<19)
                        return true;
                if (tmp.length()>19)
                        return false;
                if ( isNegative )
                        return tmp < QString("9223372036854775808");
                else
                        return tmp < QString("9223372036854775808");
        }
        return false;
}

void IntegerOverflowChecker::updateN()
{
        if ( m_architecture == Always32Bit ) {
                n = 32;
        }
        else {
                n = QSysInfo::WordSize;
        }
        power2inNminus2 = ipow(2,n);
}

qreal IntegerOverflowChecker::log_2(const qreal x)
{
        // log_a(b) = log_c(b) / log_c(a)
        return log(x) / log(2.0);
}


int IntegerOverflowChecker::ipow(int base, uint pwr)
{
        int result = 1;
        for ( uint i=0; i<pwr; i++ ) {
                result *= base;
        }
        return result;
}

}
