//
//  Client.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 30/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Client_hpp
#define Client_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

#include "EntitySystem.hpp"
#include "Settings.hpp"
#include "Collectors.hpp"

using namespace ns;
using std::endl;
using std::cout;
using std::list;
using std::vector;
using ns::base::utf8;
using ns::base::utf16;

namespace HeavensGate
{
    class Client
    {
        static sf::Packet packet;
        
    public:
        static int port;
        static bool connected;
        static sf::IpAddress address;
        static sf::TcpSocket socket;
        
        static void Connect();
        static void Send(const std::string& str = "Hi!");
    };
}

#endif /* Client_hpp */
