#include "callbacks.h" // 헤더에 선언한 클래스와 함수 정보를 구현체가 알아야 함

void PacketCallback::onWrite(BLECharacteristic *pCharacteristic)
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

bool isNotifySubscribed = false;

void DescriptorCallback::onWrite(BLEDescriptor *pDescriptor)
{
    // 스마트폰 UI 버튼 클릭 → Flutter의 setNotifyValue(bool) 호출 → 0x2902 Descriptor 메모리 값(16비트) 0x0000 ↔ 0x0001 쓰기(Write) → 콜백함수 호출
    uint8_t *value = pDescriptor->getValue();
    if (value[0] == 1)
    {
        isNotifySubscribed = true;
        // setNotify(true) → 0x0001 Write → Switch ON
    }
    else
    {
        isNotifySubscribed = false;
        // setNotify(false) → 0x0000 Write → Switch OFF
    }
}