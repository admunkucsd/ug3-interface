//
//  UG3SimulatedInput.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#include "UG3SimulatedInput.h"

using namespace UG3Interface;

UG3SimulatedInput::UG3SimulatedInput(int channelsX, int channelsY, int updateInterval, unsigned long long maxValue): channelsX(channelsX), channelsY(channelsY), updateInterval(updateInterval), maxValue(maxValue), counter(0){
    simulatedValues = (uint16_t *) malloc(channelsX * channelsY * 2);
    memset(simulatedValues, 0, channelsX*channelsY*sizeof(*simulatedValues));
}
UG3SimulatedInput::~UG3SimulatedInput(){
    free(simulatedValues);
}
bool UG3SimulatedInput::connect(){
    return true;
}
bool UG3SimulatedInput::reconnect(){
    return true;
}
bool UG3SimulatedInput::loadBuffer(void * destBuffer, int maxBytestoRead){
    if(counter == updateInterval - 1)
        for(int y = 0; y < channelsY; y++) {
            for(int x = 0; x < channelsX; x++) {
                simulatedValues[x + y*channelsX] = (maxValue/channelsX * x + (waveIndex * maxValue/channelsX)) % maxValue;
        }
    }
    else if(counter == updateInterval) {
        memset(simulatedValues, 0, channelsX*channelsY*sizeof(*simulatedValues));
        counter = 0;
        waveIndex = (waveIndex + 1) % channelsX;
    }
    counter += 1;
    memcpy((uint16_t*)destBuffer, simulatedValues, maxBytestoRead);
    return true;
}
