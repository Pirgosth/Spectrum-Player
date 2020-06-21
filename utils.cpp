#include "utils.h"

std::string getFormatedTime(int timeInSeconds){
    int hours = timeInSeconds/3600;
    timeInSeconds %= 3600;
    int minutes = timeInSeconds/60;
    timeInSeconds %= 60;
    int secondes = timeInSeconds;
    return (hours != 0 ? ((hours >= 10 ? std::to_string(hours) : "0" + std::to_string(hours)) + ":") : "") + (minutes >= 10 ? std::to_string(minutes) : ("0" + std::to_string(minutes))) + ":" + (secondes >= 10 ? std::to_string(secondes) : ("0" + std::to_string(secondes)));
}

std::string getFileName(std::string path){
    int lastSlashIndex = path.find_last_of('/');
    int pointIndex = path.find_last_of('.');
    return path.substr(lastSlashIndex + 1, pointIndex-1-lastSlashIndex);
}