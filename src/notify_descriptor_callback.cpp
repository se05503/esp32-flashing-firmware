#include <BLEDescriptor.h>

bool isNotifySubscribed = false;

class DescriptorCallbacks : public BLEDescriptorCallbacks
{
    void onWrite(BLEDescriptor *pDescriptor) override
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
};