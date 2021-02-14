#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class N715021 : public BaseMapper // Super GUN board
{
protected:
    uint16_t GetPRGPageSize() override { return 0x4000; }
    uint16_t GetCHRPageSize() override { return 0x2000; }

    void InitMapper() override
    {
        SelectPRGPage(1, -1);
        SelectPRGPage(0, 0);
        SelectCHRPage(0, 0);
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        SelectPRGPage(0, ((addr & 0x000C0) >> 2));
        SelectCHRPage(0, (addr & 0x0003));
    }
};