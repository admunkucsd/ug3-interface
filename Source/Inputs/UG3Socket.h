//
//  UG3Socket.hpp
//  ug3-interface
//
//  Created by Allen Munk on 11/2/22.
//

#ifndef UG3Socket_h
#define UG3Socket_h

#include <stdio.h>
#include <DataThreadHeaders.h>
#include "../UG3InterfaceComponents.h"

namespace UG3Interface {
class UG3Socket : public UG3Input {
public:
    UG3Socket(int port);
    ~UG3Socket();
    bool connect() override;
    bool reconnect() override;
    bool loadBuffer(void * destBuffer, int maxBytestoRead) override;
    
private:
    ScopedPointer<DatagramSocket> socket;
    int port;
    
};
}
#endif /* UG3Socket_h */
