#pragma once // 여러 파일에서 같은 헤더를 include해도 클래스가 중복 정의되는것 방지

#include <Arduino.h>
#include <BLECharacteristic.h>
#include <BLEDescriptor.h>

class PacketCallback : public BLECharacteristicCallbacks
{
    // 클래스 멤버 함수의 접근 권한 설정 (기본값: private)
    // 구현부를 작성하지 않음 (인터페이스와 구현 분리)
public:
    void onWrite(BLECharacteristic *pCharacteristic) override;
};

class DescriptorCallback : public BLEDescriptorCallbacks
{
public:
    void onWrite(BLEDescriptor *pDescriptor) override;
};

extern bool isNotifySubscribed; // 실제 변수는 다른 .cpp에서 실제 정의되어 있음을 알림 (선언, 메모리 할당 X)
