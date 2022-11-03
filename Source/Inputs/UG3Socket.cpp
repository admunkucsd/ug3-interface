//
//  UG3Socket.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/2/22.
//

#include "UG3Socket.h"

using namespace UG3Interface;

UG3Socket::UG3Socket(int port) : port(port){}
UG3Socket::~UG3Socket(){}
bool UG3Socket::connect(){
    socket = new DatagramSocket();
    socket->bindToPort(port);
    return (socket->waitUntilReady(true, 500) == 1);
}
bool UG3Socket::reconnect(){
    socket->shutdown();
    socket = new DatagramSocket();
    bool bound = socket->bindToPort(port);
    bool connected = false;
    if (bound)
    {
        std::cout << "Socket bound to port " << port << std::endl;
        connected = (socket->waitUntilReady(true, 500) == 1);
    }
    else {
        std::cout << "Could not bind socket to port " << port << std::endl;
    }
    

    if (connected)
    {
        std::cout << "Socket connected." << std::endl;
        return true;

    }
    else {
        std::cout << "Socket failed to connect" << std::endl;
        return false;
    }
}
bool UG3Socket::loadBuffer(void * destBuffer, int maxBytestoRead){
    int rc = socket->read(destBuffer, maxBytestoRead, true);

    if (rc == -1)
    {
        CoreServices::sendStatusMessage("Ephys Socket: Data shape mismatch");
        return false;
    }
    return true;
}
