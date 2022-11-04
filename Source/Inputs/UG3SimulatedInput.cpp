//
//  UG3SimulatedInput.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#include "UG3SimulatedInput.h"

using namespace UG3Interface;

UG3SimulatedInput::UG3SimulatedInput(int channelsX, int channelsY): channelsX(channelsX), channelsY(channelsY){
    simulatedValues = (uint16_t *) malloc(channelsX * channelsY * 2);
    for(int y = 0; y < channelsY; y++) {
        for(int x = 0; x < channelsX; x++) {
            simulatedValues[x + y*channelsX] = 200/channelsX*x;
        }
    }
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
    memcpy((uint16_t*)destBuffer, simulatedValues, maxBytestoRead);
    for(int y = 0; y < channelsY; y++) {
        for(int x = 0; x < channelsX; x++) {
            simulatedValues[x + y*channelsX] = (simulatedValues[x + y*channelsX] + 200/channelsX) % 200;
        }
    }
    return true;
}
