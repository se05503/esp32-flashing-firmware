#include "callbacks.h" // 헤더에 선언한 클래스와 함수 정보를 구현체가 알아야 함
#include <BLEServer.h>

void RxCallback::onWrite(BLECharacteristic *pCharacteristic)
{
    // 스마트폰이 쓴 바이트 데이터 가져오기
    uint8_t *pData = pCharacteristic->getData();
    size_t length = pCharacteristic->getLength();
    if (length > 0)
    {
        // 프로토콜 파싱 예시 (예: pData[0] = 헤더 명령)
        uint8_t command = pData[0];
        switch (command)
        {
        case 0x00:
            // 로봇 정지
            break;
        case 0x01:
            // 로봇 전진
            break;
        case 0x02:
            // 로봇 후진
            break;
        case 0x03:
            // 로봇 좌회전
            break;
        case 0x04:
            // 로봇 우회전
            break;
        default:
            // 잘못된 명령, 알 수 없는 명령
            break;
        }
    }
}

// 0x2902 Descriptor (CCCD) 콜백 → Notify 전용
bool isNotifySubscribed = false; // 변수 실제 정의(초기화) 및 메모리 할당

void DescriptorCallback::onWrite(BLEDescriptor *pDescriptor)
{
    // 스마트폰 UI 버튼 클릭 → Flutter의 setNotifyValue(bool) 호출 → 0x2902 Descriptor 메모리 값(16비트) 쓰기(Write) → 콜백함수 호출
    uint8_t *val = pDescriptor->getValue();
    size_t length = pDescriptor->getLength(); // 단위: 바이트

    if (length >= 2)
    {
        // BLE의 스위치 설정값(CCCD)은 2바이트 크기의 메모리 공간을 차지함
        // 2바이트 데이터를 전송할 때, 이를 1바이트씩 2개로 쪼개어 배열 형태로 보낸다 → [첫 번째 바이트, 두 번째 바이트]
        // val[0]: Bit0 ~ Bit7, val[1]: Bit8 ~ Bit15
        // Bit0 = Notify, Bit1 = Indicate(ACK 포함)
        // Bit0의 값만 확인하여 Notify 활성화 여부 체크
        if (val[0] & 0x01)
        {
            isNotifySubscribed = true; // 초기화가 아니라 이미 정의되어 있는 변수에 값 대입하여 상태 변경
            // setNotify(true) → 0x0001 Write → Switch ON
        }
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