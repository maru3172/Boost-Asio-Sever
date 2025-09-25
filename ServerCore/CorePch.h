#pragma once

#include "Types.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "CoreMacro.h"
#include "Container.h"

#include <iostream>
#include <windows.h>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <boost/asio.hpp>
#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "TypeCast.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "Session.h"
#include "JobQueue.h"
#include "ConsoleLog.h"
#include "ObjectPool.h"