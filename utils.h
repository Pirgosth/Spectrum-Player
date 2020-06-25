#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <string>
#include <cmath>
#include <complex>
#include <valarray>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

std::string getFormatedTime(int timeInSeconds);
std::string getFileName(std::string path);
void fft(CArray &bin);

#endif //UTILS_H_INCLUDED