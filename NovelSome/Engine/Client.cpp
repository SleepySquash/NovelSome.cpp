//
//  Client.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 30/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "Client.hpp"

namespace HeavensGate
{
    int Client::port{ 13777 };
    bool Client::connected{ false };
    sf::IpAddress Client::address{ "127.0.0.1" };
    sf::TcpSocket Client::socket;
    sf::Packet Client::packet;
    
    void Client::Connect()
    {
        if ((connected = (socket.connect(address, port) == sf::Socket::Done)))
            cout << "HeavensGate :: Client :: Successfully connected to " << address << ":" << port << "." << endl;
        else cout << "Error :: HeavensGate :: Client :: Can't connect to " << address << ":" << port << "." << endl;
    }
    
    void Client::Send(const std::string& str)
    {
        if (!connected) return;
        packet << str;
        socket.send(packet);
        packet.clear();
    }
}
