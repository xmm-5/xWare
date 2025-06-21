#pragma once

#include <Windows.h>
#include <thread>
#include <chrono>
#include <atomic>

inline std::atomic<bool> crouchSpamToggle = false;

inline void CrouchSpamLoop()
{
    while (true)
    {
        if (crouchSpamToggle.load() && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        {
            INPUT inputDown = { 0 };
            inputDown.type = INPUT_KEYBOARD;
            inputDown.ki.wVk = VK_CONTROL;

            INPUT inputUp = { 0 };
            inputUp.type = INPUT_KEYBOARD;
            inputUp.ki.wVk = VK_CONTROL;
            inputUp.ki.dwFlags = KEYEVENTF_KEYUP;

            SendInput(1, &inputDown, sizeof(INPUT));
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            SendInput(1, &inputUp, sizeof(INPUT));
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}
