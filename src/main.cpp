#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "callbacks.h"

// 기기간 연결 후 데이터 송수신을 위한 GATT 고유번호
#define SERVICE_UUID "4fa1c432-132e-4b77-b952-b91b34c25638"
#define RX_CHARACTERISTIC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e" // 앱 → ESP32 (명령 = Write)
#define TX_CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e" // ESP32 → 앱 (데이터 전송 = Transmit)

BLECharacteristic *pRxCharacteristic = nullptr; // RX 전용 특성 (Write)
BLECharacteristic *pTxCharacteristic = nullptr; // TX 전용 특성 (Notify)

void setup()
{
    Serial.begin(115200); // 1초에 115,200비트의 데이터 전송 (출력 깨짐 방지)

    // BLE 기기 이름 설정 및 초기화
    BLEDevice::init("Birdly_ESP32_Hardware");

    // 전파 송신 및 서비스 생성을 위한 BLE 서버 생성 및 이벤트 콜백 연결
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallback());

    // 서비스 생성
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // RX 특성 생성 및 콜백 등록
    pRxCharacteristic = pService->createCharacteristic(RX_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new RxCallback());

    // TX 특성 생성
    pTxCharacteristic = pService->createCharacteristic(TX_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);

    // 0x2902 Descriptor (CCCD 스위치) 생성 및 콜백 등록
    BLE2902 *pDescriptor = new BLE2902();
    pDescriptor->setCallbacks(new DescriptorCallback());

    // 특성에 구독 ON/OFF 스위치 달기
    pTxCharacteristic->addDescriptor(pDescriptor);

    // 서비스 시작
    pService->start();

    // 광고 세부 속성 설정(싱글톤 객체 관리)
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID); // 1차 브로드캐스트(Advertising Packet = 31바이트)
    pAdvertising->setScanResponse(true);        // 스마트폰이 추가 정보를 요청할 때 응답을 허용함 → 2차 추가 응답(Scan Response Packet = 31바이트)
    pAdvertising->setMinPreferred(0x06);        // 연결 주기 단위(7.5ms) 설정 (iOS BLE 디자인 가이드라인을 준수하지 않으면 연결이 불안정함 → 통신 규격에 맞춰 아이폰 호환성을 맞춤)

    // 광고 시작(공기 중에 전파를 쏴서 스마트폰 안테나가 탐지 가능하도록 함)
    BLEDevice::startAdvertising();
}

void loop()
{
    // 스마트폰 기기로부터 날라오는 전파는 loop가 아닌 백그라운드 이벤트가 처리함 (Write)
    // loop에서는 스마트폰으로 스트리밍 데이터를 실시간으로 송신함 (Notify)
    if (isDeviceConnected && isNotifySubscribed)
    {
        uint8_t sensorData = 50;                     // 임시 설정
        pTxCharacteristic->setValue(&sensorData, 1); // 데이터 주소와 바이트 크기 전달
        pTxCharacteristic->notify();                 // ESP32 → 스마트폰 앱 데이터 푸시

        delay(100); // 0.1초(100ms) 대기
    }
    else
    {
        // 구독하지 않는 동안에는 대기 시간을 부여하여 CPU 자원과 배터리를 절약함
        delay(10);
    }
}
