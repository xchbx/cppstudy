#include "RtspPlayer.hpp"
#include <iostream>
#include <string>

using namespace RK;

int main(int argc, char **argv) {
	RtspPlayer::Ptr player = std::make_shared<RtspPlayer>();
    player->Play("rtsp://172.19.0.222/live");

	getchar();
	return 0;
}
