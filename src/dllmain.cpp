#include <MaiSense/Sensor.hpp>
#include <MaiSense/InputManager.hpp>
#include <MaiSense/TouchController.hpp>
#include <MaiSense/KeyboardController.hpp>
#include <MaiSense/MouseController.hpp>
#include <MaiSense/SensorChecker.hpp>
#include <MaiSense/SensorProcessor.hpp>
#include <MaiSense/SerialController.hpp>

#include <string>
#include <tchar.h>

#pragma warning (disable : 4996)

using namespace MaiSense;

TouchController touchController;
KeyboardController keyboardController;
MouseController mouseController;
SerialController serialController;
SensorChecker sensorChecker;
SensorProcessor processor;

HANDLE GetSerialHandle() {
    HANDLE hCom; //全局变量，串口句柄
    hCom = CreateFile(_T("COM1:"),//COM1口
        GENERIC_READ, //允许读和写
        0, //独占方式
        NULL,
        OPEN_EXISTING, //打开而不是创建
        0, //同步方式
        NULL);

    if (hCom == (HANDLE)-1) {
        return NULL;
    }

    // 配置缓冲区大小
    if (!SetupComm(hCom, 1024, 1024)) {
        return NULL;
    }

    // 配置参数
    DCB p;
    memset(&p, 0, sizeof(p));
    p.DCBlength = sizeof(p);
    p.BaudRate = 9600;
    p.ByteSize = 8;
    p.Parity = NOPARITY;
    p.StopBits = ONESTOPBIT;

    if (!SetCommState(hCom, &p)) {
        // 设置参数失败
        return NULL;
    }

    //超时处理,单位：毫秒
    //总超时＝时间系数×读或写的字符数＋时间常量
    COMMTIMEOUTS TimeOuts;
    TimeOuts.ReadIntervalTimeout = 1000; //读间隔超时
    TimeOuts.ReadTotalTimeoutMultiplier = 500; //读时间系数
    TimeOuts.ReadTotalTimeoutConstant = 5000; //读时间常量
    TimeOuts.WriteTotalTimeoutMultiplier = 500; // 写时间系数
    TimeOuts.WriteTotalTimeoutConstant = 2000; //写时间常量
    SetCommTimeouts(hCom, &TimeOuts);

    PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空串口缓冲区


    return hCom;
}

BOOL ReadSerialData(HANDLE hSerial, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead) {
    BOOL bSuccess = ReadFile(hSerial, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, NULL);
    return bSuccess;
}

DWORD WINAPI SerialCommThread(LPVOID lpParam) {
    //创建消息队列
    MSG msg;
    //PeekMessage(&msg, NULL, WM_USER, WM_MAI2TOUCH_MESSAGE, PM_NOREMOVE);
    PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

    HANDLE hSerial = GetSerialHandle();
    while (true) {
        // 读取串口数据
        BYTE buffer[9];
        DWORD bytesRead = 0;
        BOOL success = ReadSerialData(hSerial, buffer, 9, &bytesRead);
        if (success) {
            // 处理哪些区域被触发
            Mai2TouchData data;
            if (buffer[0] == '(' && buffer[8] == ')') {
                uint64_t touchData = 0;
                for (int i = 7; i >= 1; i--) {
                    touchData = (touchData << 5) | (buffer[i] - '0');
                }

                uint16_t mprA = touchData & 0xFFFF; //A1~B4
                uint16_t mprB = (touchData >> 12) & 0xFFFF; //B5~D6
                uint16_t mprC = (touchData >> 24) & 0xFFFF; //D7~E8

                //bool switchesB[12], switchesC[12];
                for (int i = 0; i < 12; i++) {
                    if (mprB & (1 << i) != 0) {
                        data.activeArea.push_back((Mai2TouchId)(B5 + i));
                    }
                    if (mprC & (1 << i) != 0) {
                        data.activeArea.push_back((Mai2TouchId)(A1 + i));
                    }
                }
            }

            // 打包自定义数据
            LPARAM lParam = reinterpret_cast<LPARAM>(&data);

            PostThreadMessage(GetCurrentThreadId(), WM_MAI2TOUCH_MESSAGE, 0, lParam);
        }
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD cause, LPVOID lpReserved)
{
    if (!InputManager::Ready())
        return TRUE;

    if (cause == DLL_PROCESS_ATTACH) 
    {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);

        serialController.SetCallback([&](SerialEvent ev) 
        {
            auto sensor = InputManager::GetSensor();
            switch (ev.Id)
            {
            case Mai2TouchId::A1:
                sensor->Queue(Sensor::A1, ev.Active);
                break;
            case Mai2TouchId::A2:
                sensor->Queue(Sensor::A2, ev.Active);
                break;
            case Mai2TouchId::A3:
                sensor->Queue(Sensor::A3, ev.Active);
                break;
            case Mai2TouchId::A4:
                sensor->Queue(Sensor::A4, ev.Active);
                break;
            case Mai2TouchId::A5:
                sensor->Queue(Sensor::A5, ev.Active);
                break;
            case Mai2TouchId::A6:
                sensor->Queue(Sensor::A6, ev.Active);
                break;
            case Mai2TouchId::A7:
                sensor->Queue(Sensor::A7, ev.Active);
                break;
            case Mai2TouchId::A8:
                sensor->Queue(Sensor::A8, ev.Active);
                break;
            case Mai2TouchId::B1:
                sensor->Queue(Sensor::B1, ev.Active);
                break;
            case Mai2TouchId::B2:
                sensor->Queue(Sensor::B2, ev.Active);
                break;
            case Mai2TouchId::B3:
                sensor->Queue(Sensor::B3, ev.Active);
                break;
            case Mai2TouchId::B4:
                sensor->Queue(Sensor::B4, ev.Active);
                break;
            case Mai2TouchId::B5:
                sensor->Queue(Sensor::B5, ev.Active);
                break;
            case Mai2TouchId::B6:
                sensor->Queue(Sensor::B6, ev.Active);
                break;
            case Mai2TouchId::B7:
                sensor->Queue(Sensor::B7, ev.Active);
                break;
            case Mai2TouchId::B8:
                sensor->Queue(Sensor::B8, ev.Active);
                break;
            case Mai2TouchId::C1:
            case Mai2TouchId::C2:
                sensor->Queue(Sensor::C, ev.Active);
                break;
            }
        });

        touchController.SetCallback([&](TouchEvent ev) 
        {
            processor.Handle(ev);
            if (sensorChecker.GetScreenWidth() == 0 || sensorChecker.GetScreenHeight() == 0)
            {
                RECT clientRect;
                GetClientRect(InputManager::GetGameWindow(), &clientRect);
                sensorChecker.SetScreenSize
                (
                    clientRect.left + clientRect.right,
                    clientRect.top + clientRect.bottom
                );
            }

            if (processor.GetChecker() == NULL)
                processor.SetChecker(&sensorChecker);

            if (processor.GetSensor() == NULL)
                processor.SetSensor(InputManager::GetSensor());
        });

        keyboardController.SetCallback([&](KeyEvent ev)
        {
            auto sensor = InputManager::GetSensor();
            switch (ev.KeyCode)
            {
            case 0x30: // 0
                sensor->Queue(Sensor::C, ev.Active);
                break;
            case 0x31: // 1
                sensor->Queue(Sensor::A1, ev.Active);
                break;
            case 0x32: // 2
                sensor->Queue(Sensor::A2, ev.Active);
                break;
            case 0x33: // 3
                sensor->Queue(Sensor::A3, ev.Active);
                break;
            case 0x34: // 4
                sensor->Queue(Sensor::A4, ev.Active);
                break;
            case 0x35: // 5
                sensor->Queue(Sensor::A5, ev.Active);
                break;
            case 0x36: // 6
                sensor->Queue(Sensor::A6, ev.Active);
                break;
            case 0x37: // 7
                sensor->Queue(Sensor::A7, ev.Active);
                break;
            case 0x38: // 8
                sensor->Queue(Sensor::A8, ev.Active);
                break;
            case VK_NUMPAD1: // NUMPAD 1
                sensor->Queue(Sensor::B1, ev.Active);
                break;
            case VK_NUMPAD2: // NUMPAD 2
                sensor->Queue(Sensor::B2, ev.Active);
                break;
            case VK_NUMPAD3: // NUMPAD 3
                sensor->Queue(Sensor::B3, ev.Active);
                break;
            case VK_NUMPAD4: // NUMPAD 4
                sensor->Queue(Sensor::B4, ev.Active);
                break;
            case VK_NUMPAD5: // NUMPAD 5
                sensor->Queue(Sensor::B5, ev.Active);
                break;
            case VK_NUMPAD6: // NUMPAD 6
                sensor->Queue(Sensor::B6, ev.Active);
                break;
            case VK_NUMPAD7: // NUMPAD 7
                sensor->Queue(Sensor::B7, ev.Active);
                break;
            case VK_NUMPAD8: // NUMPAD 8
                sensor->Queue(Sensor::B8, ev.Active);
                break;
            }
        });

        mouseController.SetCallback([&](MouseEvent ev)
        {
            if (ev.MButton)
                mouseController.EmulateTouch();
        });

        InputManager::Hook();
        InputManager::Install(&touchController);
        InputManager::Install(&keyboardController);
        InputManager::Install(&mouseController);

        HANDLE hThread = CreateThread(NULL, 0, SerialCommThread, NULL, 0, NULL);
        if (hThread == NULL) {
            return FALSE;
        }
        CloseHandle(hThread);
    }
    else if (cause == DLL_PROCESS_DETACH) 
    {
        InputManager::Unhook();
    }

    return TRUE;
}
