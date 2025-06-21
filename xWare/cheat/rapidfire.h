#pragma once

#include <Windows.h>
#include <thread>
#include <chrono>
#include <atomic>

// Define this atomic **once**, globally:
inline std::atomic<bool> rapidFireToggle = false;

inline void RapidFireLoop()
{
    while (true)
    {
        if (rapidFireToggle.load() &&
            (GetAsyncKeyState(VK_LBUTTON) & 0x8000) &&
            (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
        {
            INPUT inputDown = { 0 };
            inputDown.type = INPUT_MOUSE;
            inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

            INPUT inputUp = { 0 };
            inputUp.type = INPUT_MOUSE;
            inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;

            SendInput(1, &inputDown, sizeof(INPUT));
            SendInput(1, &inputUp, sizeof(INPUT));

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
