#pragma once

//System
#include <cstddef>
#include <exception>
#include <iostream>

//Container
#include <array>
#include <vector>
#include <map>

//String
#include <string>
#include <format>

//Parallel
#include <thread>
#include <atomic>
#include <mutex>

//Util
#include <memory>
#include <algorithm>
#include <bit>
#include <chrono>
using namespace std::chrono_literals;

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "WS2_32.lib")
