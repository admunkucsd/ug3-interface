//
//  UG3SimulatedInput.hpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#ifndef UG3SimulatedInput_h
#define UG3SimulatedInput_h

#include <stdio.h>
#include <DataThreadHeaders.h>
#include "../UG3InterfaceComponents.h"

namespace UG3Interface {
class UG3SimulatedInput : public UG3Input {
public:
    UG3SimulatedInput(int channelsX = 64, int channelsY = 64, int samples = 10, unsigned long long maxValue = 0xffff);
    ~UG3SimulatedInput();
    bool connect() override;
    bool reconnect() override;
    
    /**loads buffer in such a way that a wave effect is created covering
     the entire color theme's spectrum**/
    bool loadBuffer(void * destBuffer, int maxBytestoRead) override;
    
private:
    uint16_t *simulatedValues;
    int channelsX;
    int channelsY;
    int counter;
    int samples;
    unsigned long long maxValue;
    uint8_t waveIndex = 0;
};
}
#endif /* UG3SimulatedInput_h */
