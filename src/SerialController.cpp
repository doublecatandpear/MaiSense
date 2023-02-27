#include <MaiSense/SerialController.hpp>

namespace MaiSense {
    SerialController::SerialController() : 
        callback()
    {
        HANDLE hCom; //全局变量，串口句柄
        hCom = CreateFile("COM1",//COM1口
            GENERIC_READ, //允许读和写
            0, //独占方式
            NULL,
            OPEN_EXISTING, //打开而不是创建
            0, //同步方式
            NULL);

        if (hCom == (HANDLE)-1) {
            return;
        }

        // 配置缓冲区大小
        if (!SetupComm(hCom, 1024, 1024)) {
            return;
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
            return;
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


        memcpy(pHandle, &hCom, sizeof(hCom));
    }

    SerialController::~SerialController() {
        HANDLE hCom = *(HANDLE*)pHandle;
        if (hCom != (HANDLE)-1) {
            CloseHandle(hCom);
        }
    }

    void SerialController::SetCallback(std::function<void(SerialEvent)> cb)
    {
        callback = cb;
    }

    bool SerialController::Check(int evCode)
    {
        return true;
    }

    void SerialController::OnInput(int nCode, WPARAM wParam, LPARAM lParam)
    {
        // Parse keyboard hook event
        auto ev = SerialEvent();
        auto msg = (LPMSG)lParam;

        // Initialize key event
        HANDLE hCom = *(HANDLE*)pHandle;
        if (hCom != (HANDLE)-1) {

        }

        // Pass event into callback
        if (callback)
            callback(ev);
    }
}