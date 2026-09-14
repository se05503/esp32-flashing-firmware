#include <Arduino.h>
#include <BLECharacteristic.h>

class PacketCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic) override
    {
        // 스마트폰이 쓴 바이트 데이터 가져오기
        uint8_t *pData = pCharacteristic->getData();
        size_t length = pCharacteristic->getLength();
        if (length > 0)
        {
            // 수신된 데이터가 있는 경우 첫번째 바이트 데이터(명령) 꺼내오기
            uint8_t command = pData[0];
        }
    }
};