//
//  UG3InterfaceComponents.hpp
//  ug3-interface
//
//  Created by Allen Munk on 11/2/22.
//

#ifndef UG3InterfaceComponents_h
#define UG3InterfaceComponents_h

#include <DataThreadHeaders.h>

namespace UG3Interface {

class UG3Input {
public:
    /** Returns true if the input is able to connect to the source */
    virtual bool connect() = 0;
    
    /** Returns true if the input is able to reconnect to the source */
    virtual bool reconnect() = 0;
    
    /** Returns true if the input is able to disconnect to the source */
    virtual bool loadBuffer(void * destBuffer, int maxBytestoRead) = 0;
};

}
#endif /* UG3InterfaceComponents_h */
