#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <complex>
#include <valarray>
#include <math.h>

// #include <cstring>
#include <string>

#ifdef _WIN32

#define PATH_SEP '\\'

#else

#define PATH_SEP '/'

#endif // _WIN32

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

std::string tolower_str(std::string str);

std::string getFormatedTime(int timeInSeconds);
std::string getFileName(std::string path);
std::string getFileExtension(std::string path);
void fft(CArray &bin);

#endif //UTILS_H_INCLUDED