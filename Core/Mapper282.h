#pragma once
#include "stdafx.h"
#include "JyCompany.h"
#include "BaseMapper.h"

class Mapper282 : public JyCompany
{
private:
    uint8_t _outerReg;
protected:
    void InitMapper() override
    {
        _outerReg = 0;
        JyCompany::InitMapper();
    }

    void SelectPRGPage(uint16_t slot, uint16_t page) override
    {
        BaseMapper::SelectPRGPage(slot, page);
    }

    void WriteRegister(uint16_t addr, uint8_t value) override
    {
        if((addr & 0xF003) == 0xD003) {
            _outerReg = 0;
        } else {
            JyCompany::WriteRegister(addr, value);
        }
    }
}