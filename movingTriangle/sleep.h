#pragma once

#include<thread>
#include<chrono>

namespace ProjectName {


void sleep(unsigned int time) {
  std::this_thread::sleep_for( std::chrono::milliseconds(time) );
}

}

