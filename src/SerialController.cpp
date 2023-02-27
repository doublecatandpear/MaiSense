#include <MaiSense/SerialController.hpp>

namespace MaiSense {
    SerialController::SerialController() : 
        callback()
    {
        HANDLE hCom; //ȫ�ֱ��������ھ��
        hCom = CreateFile("COM1",//COM1��
            GENERIC_READ, //�������д
            0, //��ռ��ʽ
            NULL,
            OPEN_EXISTING, //�򿪶����Ǵ���
            0, //ͬ����ʽ
            NULL);

        if (hCom == (HANDLE)-1) {
            return;
        }

        // ���û�������С
        if (!SetupComm(hCom, 1024, 1024)) {
            return;
        }

        // ���ò���
        DCB p;
        memset(&p, 0, sizeof(p));
        p.DCBlength = sizeof(p);
        p.BaudRate = 9600;
        p.ByteSize = 8;
        p.Parity = NOPARITY;
        p.StopBits = ONESTOPBIT;

        if (!SetCommState(hCom, &p)) {
            // ���ò���ʧ��
            return;
        }

        //��ʱ����,��λ������
        //�ܳ�ʱ��ʱ��ϵ��������д���ַ�����ʱ�䳣��
        COMMTIMEOUTS TimeOuts;
        TimeOuts.ReadIntervalTimeout = 1000; //�������ʱ
        TimeOuts.ReadTotalTimeoutMultiplier = 500; //��ʱ��ϵ��
        TimeOuts.ReadTotalTimeoutConstant = 5000; //��ʱ�䳣��
        TimeOuts.WriteTotalTimeoutMultiplier = 500; // дʱ��ϵ��
        TimeOuts.WriteTotalTimeoutConstant = 2000; //дʱ�䳣��
        SetCommTimeouts(hCom, &TimeOuts);

        PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//��մ��ڻ�����


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