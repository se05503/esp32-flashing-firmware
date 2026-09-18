#include "callbacks.h" // 헤더에 선언한 클래스와 함수 정보를 구현체가 알아야 함
#include <BLEServer.h>

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

bool isNotifySubscribed = false; // 변수 실제 정의(초기화) 및 메모리 할당

void DescriptorCallback::onWrite(BLEDescriptor *pDescriptor)
{
    // 스마트폰 UI 버튼 클릭 → Flutter의 setNotifyValue(bool) 호출 → 0x2902 Descriptor 메모리 값(16비트) 0x0000 ↔ 0x0001 쓰기(Write) → 콜백함수 호출
    uint8_t *value = pDescriptor->getValue();
    if (value[0] == 1)
    {
        isNotifySubscribed = true; // 초기화가 아니라 이미 정의되어 있는 변수에 값 대입하여 상태 변경
        // setNotify(true) → 0x0001 Write → Switch ON
    }
    else
    {
        isNotifySubscribed = false;
        // setNotify(false) → 0x0000 Write → Switch OFF
    }
}

bool isDeviceConnected = false;

void ServerCallback::onConnect(BLEServer *pServer)
{
    isDeviceConnected = true;
    // notify는 이후 선택 사항
    // BLE Stack이 광고 패킷을 더이상 뿌리지 않도록 알아서 처리
}

void ServerCallback::onDisconnect(BLEServer *pServer)
{
    isDeviceConnected = false;
    isNotifySubscribed = false; // 연결이 끊기면 구독 상태도 해제
    // 연결이 끊기면 다른 기기가 찾을 수 있도록 재광고
    pServer->startAdvertising();
}