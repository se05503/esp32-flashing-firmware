#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "callbacks.h"

// 기기간 연결 후 데이터 송수신을 위한 GATT 고유번호
#define SERVICE_UUID "4fa1c432-132e-4b77-b952-b91b34c25638"
#define CHARACTERISTIC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

void setup()
{
    // 1. BLE 기기 이름 설정 및 초기화
    BLEDevice::init("Birdly_ESP32_Hardware");

    // 2. 전파 송신 및 서비스 생성을 위한 BLE 서버 생성
    BLEServer *pServer = BLEDevice::createServer();

    // 3. 서버를 통한 서비스 생성
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // 4. 서비스를 이용한 특성 생성 및 갖가지 속성 설정
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

    // Write
    pCharacteristic->setCallbacks(new PacketCallback());

    // Notify
    BLE2902 *pDescriptor = new BLE2902();
    pCharacteristic->addDescriptor(pDescriptor);         // 특성에 데이터 구독 ON/OFF 스위치 달기
    pDescriptor->setCallbacks(new DescriptorCallback()); // ON/OFF 스위치 상태를 실시간으로 받을 콜백 등록

    // 5. 서비스 개시
    pService->start();

    // 6. 공기 중에 전파를 쏴서 스마트폰 안테나가 탐지 가능하도록 함
    pServer->getAdvertising()->start();
}

void loop()
{
    // 스마트폰 기기로부터 날라오는 전파는 loop가 아닌 백그라운드 이벤트가 처리함 (Write)
    // loop에서는 스마트폰으로 스트리밍 데이터를 송신함 (Notify)
    bool isDeviceConnected = false; // 나중 구현

    if (isDeviceConnected && isNotifySubscribed)
    {
        uint8_t sensorData = 50;                   // 임시 설정
        pCharacteristic->setValue(&sensorData, 1); // 데이터 주소와 바이트 크기 전달
        pCharacteristic->notify();                 // ESP32 → 클라이언트 데이터 푸시

        delay(100); // 0.1초(100ms) 대기
    }
}
