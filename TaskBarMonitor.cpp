#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

#define UPDATE_INTERVAL 1000 // Update interval in milliseconds

// Function to get CPU usage
float GetCPUUsage() {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        std::cerr << "Error: Unable to get system times." << std::endl;
        return -1.0f;
    }

    ULARGE_INTEGER totalTime, totalIdleTime;
    totalTime.QuadPart = ((ULONGLONG)kernelTime.dwHighDateTime << 32) + kernelTime.dwLowDateTime +
                         ((ULONGLONG)userTime.dwHighDateTime << 32) + userTime.dwLowDateTime;
    totalIdleTime.QuadPart = ((ULONGLONG)idleTime.dwHighDateTime << 32) + idleTime.dwLowDateTime;

    float cpuUsage = 100.0f - ((float)totalIdleTime.QuadPart / totalTime.QuadPart) * 100.0f;
    return cpuUsage;
}

// Function to get memory usage
float GetMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    if (!GlobalMemoryStatusEx(&memInfo)) {
        std::cerr << "Error: Unable to get memory status." << std::endl;
        return -1.0f;
    }

    float memoryUsage = ((float)(memInfo.ullTotalPhys - memInfo.ullAvailPhys) / memInfo.ullTotalPhys) * 100.0f;
    return memoryUsage;
}

// Function to update the taskbar icon with CPU and memory usage graphs
void UpdateTaskbarIcon(HWND hwnd) {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    if (!hInstance) {
        std::cerr << "Error: Unable to get module handle." << std::endl;
        return;
    }

    HICON hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    if (!hIcon) {
        std::cerr << "Error: Unable to load icon." << std::endl;
        return;
    }

    while (true) {
        float cpuUsage = GetCPUUsage();
        float memoryUsage = GetMemoryUsage();

        // Draw CPU and memory usage graphs on the icon
        int cpuHeight = static_cast<int>(ceil(cpuUsage / 100.0f * 16));
        int memoryHeight = static_cast<int>(ceil(memoryUsage / 100.0f * 16));
        std::vector<int> pixels(16 * 16, 0); // Initialize with black color
        for (int i = 0; i < cpuHeight; ++i) {
            pixels[i * 16 + 7] = 1; // Draw CPU graph in green
        }
        for (int i = 0; i < memoryHeight; ++i) {
            pixels[i * 16 + 15] = 2; // Draw memory graph in blue
        }

        ICONINFO iconInfo = {0};
        iconInfo.fIcon = TRUE;
        iconInfo.xHotspot = 0;
        iconInfo.yHotspot = 0;
        iconInfo.hbmMask = nullptr;
        iconInfo.hbmColor = CreateBitmap(16, 16, 1, 32, pixels.data());
        if (!iconInfo.hbmColor) {
            std::cerr << "Error: Unable to create bitmap." << std::endl;
            return;
        }

        HICON hNewIcon = CreateIconIndirect(&iconInfo);
        if (!hNewIcon) {
            std::cerr << "Error: Unable to create new icon." << std::endl;
            DeleteObject(iconInfo.hbmColor);
            return;
        }

        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hNewIcon);
        DestroyIcon(hNewIcon);
        DeleteObject(iconInfo.hbmColor);

        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL));
    }
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hwnd = CreateWindowEx(
        0,
        "STATIC",
        "System Monitor",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwnd) {
        std::cerr << "Error: Unable to create window." << std::endl;
        return 1;
    }

    std::thread taskbarThread(UpdateTaskbarIcon, hwnd);
    taskbarThread.detach();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
