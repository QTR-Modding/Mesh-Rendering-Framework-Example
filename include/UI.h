#pragma once
#include "SKSEMenuFramework.h"

namespace UI {
    void Register();
    namespace PlayerCharacter {
        void __stdcall Render();
    }
    namespace LydiaHead {
        void __stdcall Render();
    }
    namespace LydiaAnimated {
        void __stdcall Render();
    }
    namespace GoldIngot {
        void __stdcall Render();
    }
    namespace Tree {
        void __stdcall Render();
    }
    namespace DwarvenSpider {
        void __stdcall Render();
    }
    namespace LydiaWholeNpc {
        void __stdcall Render();
    }
    namespace LydiaWithoutArmour {
        void __stdcall Render();
    }
    namespace Debug {
        void __stdcall Render();
    }
};
