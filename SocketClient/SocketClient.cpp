#include <iostream>
#include <thread>
#include <chrono>
#include "Server.h"
#include "D3DOverlay.h"
#include "MemHelp.h"

//Scene Render

//Setup Overlay Window

//Window Entry

int main()
{
    ControlMem* memory = new ControlMem();
    const auto Connection = memory->ConnectSocket();
    if (Connection == INVALID_SOCKET)
    {
		std::cout << "Failed to connect to socket driver" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
		return 1;
	}
    else {
        std::cout << "Connected to socket driver" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
    }

}
