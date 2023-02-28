#include <MaiSense/SerialController.hpp>
#include <tchar.h>

namespace MaiSense {
    SerialController::SerialController() : 
        callback()
    {
    }

    SerialController::~SerialController() {
        
    }

    void SerialController::SetCallback(std::function<void(SerialEvent)> cb)
    {
        callback = cb;
    }

    bool SerialController::Check(int evCode)
    {
        if(evCode == WM_MAI2TOUCH_MESSAGE){
            return true;
        }
        return false;
    }

    void SerialController::OnInput(int nCode, WPARAM wParam, LPARAM lParam)
    {
        // Parse keyboard hook event
        auto ev = SerialEvent();
        auto msg = (LPMSG)lParam;

        // Initialize key event
        // 将 message 传来的数据转为自定义结构体
        Mai2TouchData data = *reinterpret_cast<Mai2TouchData*>(lParam);

        // Pass event into callback
        if (callback) {
            std::vector<Mai2TouchId> allTouch{A1, A2, A3, A4, A5, A6, A7, A8, B1, B2, B3, B4, B5, B6, B7, B8, C1, C2};
            for (auto & d : allTouch) {
                ev.Id = d;
                ev.Active = (std::find(data.activeArea.begin(), data.activeArea.end(), d) != data.activeArea.end());
                callback(ev);
            }
        }
    }
}