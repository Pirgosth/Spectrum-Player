#include "utils.h"

std::string tolower_str(std::string str){
	std::string result(str);
	for(int i(0); i<result.length(); i++){
		result[i] = tolower(result[i]);
	}
	return result;
}

std::string getFormatedTime(int timeInSeconds){
    int hours = timeInSeconds/3600;
    timeInSeconds %= 3600;
    int minutes = timeInSeconds/60;
    timeInSeconds %= 60;
    int secondes = timeInSeconds;
    return (hours != 0 ? ((hours >= 10 ? std::to_string(hours) : "0" + std::to_string(hours)) + ":") : "") + (minutes >= 10 ? std::to_string(minutes) : ("0" + std::to_string(minutes))) + ":" + (secondes >= 10 ? std::to_string(secondes) : ("0" + std::to_string(secondes)));
}

std::string getFileName(std::string path){
    int lastSlashIndex = path.find_last_of(PATH_SEP);
    int pointIndex = path.find_last_of('.');
    return path.substr(lastSlashIndex + 1, pointIndex-1-lastSlashIndex);
}

std::string getFileExtension(std::string path){
	int pointIndex = path.find_last_of(".");
	return path.substr(pointIndex+1, path.length() - (pointIndex+1));
}

void fft(CArray &bin){
    const int N = bin.size();
    if(N <= 1) return;

    CArray even = bin[std::slice(0, N/2, 2)];
    CArray odd = bin[std::slice(1, N/2, 2)];

    fft(even);
    fft(odd);

    for(int i(0); i < N/2; i++){
        Complex t = std::polar(1.0, -2*PI*i/N)*odd[i];
        bin[i] = even[i] + t;
        bin[i+N/2] = even[i] - t;
    }
}