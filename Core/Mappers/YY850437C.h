#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class YY850437C : public BaseMapper
{
    private:
        uint8_t innerBankReg;
        uint8_t outerBankReg;
	protected:
		virtual uint16_t GetPRGPageSize() override { return 0x4000; }
		virtual uint16_t GetCHRPageSize() override { return 0x2000; }

        void UpdateState()
        {
            SelectPRGPage(0, ((outerBankReg & 0x07) << 3) | (innerBankReg & 0x07));
            SelectPRGPage(1, ((outerBankReg & 0x07) << 3) | 0x07);

            SetMirroringType((outerBankReg & 0x40) ? MirroringType::Horizontal : MirroringType::Vertical);
        }

		void InitMapper() override 
		{
            // init regs
            innerBankReg = 0;
            outerBankReg = 0;

            // select fixed CHR page
			SelectCHRPage(0, 0);

            UpdateState();
		}

        void StreamState(bool saving) override
        {
            BaseMapper::StreamState(saving);
            Stream(innerBankReg, outerBankReg);
        }

        // NOTE: unknown if busconflicts happen while writing to inner bank reg (outer def. not affected)
		void WriteRegister(uint16_t addr, uint8_t value) override
		{
            if((addr & 0xE000) == 0xA000) { // unknown actual mask
                outerBankReg = value;
            }
            innerBankReg = value;
            UpdateState();
		}
};