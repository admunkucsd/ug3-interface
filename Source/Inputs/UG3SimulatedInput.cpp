//
//  UG3SimulatedInput.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#include "UG3SimulatedInput.h"
#include <cmath>

using namespace UG3Interface;

UG3SimulatedInput::UG3SimulatedInput(int channelsX, int channelsY, int samples, unsigned long long maxValue): channelsX(channelsX), channelsY(channelsY),  samples(samples), maxValue(maxValue), counter(0){
    simulatedValues = (uint16_t *) malloc(channelsX * channelsY * sizeof(uint16_t) * samples);
    memset(simulatedValues, 0, channelsX*channelsY*sizeof(*simulatedValues)*samples);
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
void UG3SimulatedInput::makeColorWave() {
    for(int sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        if(sampleIndex % 2) {
            for(int y = 0; y < channelsY; y++) {
                for(int x = 0; x < channelsX; x++) {
                    simulatedValues[x + y*channelsX + channelsX*channelsY * sampleIndex] = (maxValue/channelsX * x + (waveIndex * maxValue/channelsX)) % maxValue;
                }
            }
        }
        else {
            memset(simulatedValues+channelsX*channelsY*sizeof(simulatedValues)*sampleIndex, 0, channelsX*channelsY*sizeof(*simulatedValues));
        }
    }
    waveIndex = (waveIndex + 1) % channelsX;
}


void UG3SimulatedInput::makeSine(){
    for(int sampleIndex = 0; sampleIndex < samples; sampleIndex++) {
        for(int y = 0; y < channelsY; y++) {
            for(int x = 0; x < channelsX; x++) {
                float radian = simPi*2*sampleIndex/(samples*sinePhaseShiftConstant)+simPi*2*waveIndex/sinePhaseShiftConstant;
                float sinVal = sin(radian);
                simulatedValues[x + y*channelsX + channelsX*channelsY * sampleIndex] = sinVal * 5000 + 5000;
              
            }
        }
    }
    waveIndex = (waveIndex + 1) % sinePhaseShiftConstant;
}

bool UG3SimulatedInput::loadBuffer(void * destBuffer, int maxBytestoRead){
    makeColorWave();
    memcpy((uint16_t*)destBuffer, simulatedValues, maxBytestoRead);
    return true;
}
