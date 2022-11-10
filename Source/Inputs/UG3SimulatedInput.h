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
    UG3SimulatedInput(int channelsX = 64, int channelsY = 64, int updateInterval = 10);
    ~UG3SimulatedInput();
    bool connect() override;
    bool reconnect() override;
    bool loadBuffer(void * destBuffer, int maxBytestoRead) override;
    
private:
    uint16_t *simulatedValues;
    int channelsX;
    int channelsY;
    int updateInterval;
    int counter;
    uint8_t waveIndex = 0;
};
}
#endif /* UG3SimulatedInput_h */
