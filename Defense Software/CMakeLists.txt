cmake_minimum_required(VERSION 3.7)
project(Code)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ConsCtl/ApplicationScope.cpp
        ConsCtl/ApplicationScope.h
        ConsCtl/CallbackHandler.cpp
        ConsCtl/CallbackHandler.h
        ConsCtl/Common.h
        ConsCtl/ConsCtl.cpp
        ConsCtl/CustomThread.cpp
        ConsCtl/CustomThread.h
        ConsCtl/LockMgr.cpp
        ConsCtl/LockMgr.h
        ConsCtl/NtDriverController.cpp
        ConsCtl/NtDriverController.h
        ConsCtl/QueueContainer.cpp
        ConsCtl/QueueContainer.h
        ConsCtl/QueuedItem.h
        ConsCtl/RetrievalThread.cpp
        ConsCtl/RetrievalThread.h
        ConsCtl/ThreadMonitor.cpp
        ConsCtl/ThreadMonitor.h
        ConsCtl/WinUtils.h
        ProcObsrv/ProcObsrv.c)

add_executable(Code ${SOURCE_FILES})