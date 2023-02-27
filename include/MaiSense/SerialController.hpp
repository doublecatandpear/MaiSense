#ifndef MAISENSE_SERIAL_CONTROLLER_HPP
#define MAISENSE_SERIAL_CONTROLLER_HPP

#include <vector>
#include <functional>
#include <Windows.h>

#include <MaiSense/InputController.hpp>

namespace MaiSense
{
    enum Mai2TouchId {
        A1 = 0x41,
        A2 = 0x42,
        A3 = 0x43,
        A4 = 0x44,
        A5 = 0x45,
        A6 = 0x46,
        A7 = 0x47,
        A8 = 0x48,
        B1 = 0x49,
        B2 = 0x4A,
        B3 = 0x4B,
        B4 = 0x4C,
        B5 = 0x4D,
        B6 = 0x4E,
        B7 = 0x4F,
        B8 = 0x50,
        C1 = 0x51,
        C2 = 0x52,
        D1 = 0x53,
        D2 = 0x54,
        D3 = 0x55,
        D4 = 0x56,
        D5 = 0x57,
        D6 = 0x58,
        D7 = 0x59,
        D8 = 0x5A,
        E1 = 0x5B,
        E2 = 0x5C,
        E3 = 0x5D,
        E4 = 0x5E,
        E5 = 0x5F,
        E6 = 0x60,
        E7 = 0x61,
        E8 = 0x62
    };

    struct SerialEvent
    {
        Mai2TouchId Id;
        bool Active;
    };

    class SerialController : public InputController
    {
    private:
        int pHandle[16];
        std::function<void(SerialEvent)> callback;
        virtual void OnInput(int nCode, WPARAM wParam, LPARAM lParam);

    public:
        SerialController();
        virtual ~SerialController();

        virtual bool Check(int evCode);
        void SetCallback(std::function<void(SerialEvent)> cb);
    };
}

#endif