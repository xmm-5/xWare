#pragma once

#define NOMINMAX

#include <windows.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <atomic>

inline namespace TriggerBot
{
    inline std::atomic<bool> triggerBotEnabled = false;

    inline bool IsColorYellow(COLORREF color) {
        int r = GetRValue(color);
        int g = GetGValue(color);
        int b = GetBValue(color);

        float fr = r / 255.0f;
        float fg = g / 255.0f;
        float fb = b / 255.0f;

        float max = std::max({ fr, fg, fb });
        float min = std::min({ fr, fg, fb });
        float delta = max - min;

        float hue = 0.0f;
        float saturation = 0.0f;
        float brightness = max;

        if (delta != 0.0f) {
            if (max == fr) hue = 60.0f * fmod(((fg - fb) / delta), 6.0f);
            else if (max == fg) hue = 60.0f * (((fb - fr) / delta) + 2.0f);
            else if (max == fb) hue = 60.0f * (((fr - fg) / delta) + 4.0f);

            if (hue < 0.0f) hue += 360.0f;

            saturation = delta / max;
        }

        return (hue >= 40.0f && hue <= 95.0f && saturation >= 0.35f && brightness >= 0.4f);
    }

    inline bool IsEnemyYellow(int x, int y, int radius = 3) {
        HDC hdc = GetDC(NULL);

        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                COLORREF color = GetPixel(hdc, x + dx, y + dy);
                if (IsColorYellow(color)) {
                    ReleaseDC(NULL, hdc);
                    return true;
                }
            }
        }

        ReleaseDC(NULL, hdc);
        return false;
    }

    inline void LeftClick() {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }

    inline void TriggerBotLoop() {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;

        const int fireRateMs = 100;
        auto lastShot = std::chrono::steady_clock::now();

        while (!(GetAsyncKeyState(VK_INSERT) & 0x8000)) {
            if (triggerBotEnabled.load() && (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
                if (IsEnemyYellow(centerX, centerY)) {
                    auto now = std::chrono::steady_clock::now();
                    auto msSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastShot).count();

                    if (msSinceLastShot >= fireRateMs) {
                        LeftClick();
                        lastShot = now;
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
