#pragma once
#include "SDK_Header.h"
#include <map>
#include <string>

namespace SDK {
    enum SDK_LANGUAGE_TYPE {
        MATA_LANG_KOREAN,
        MATA_LANG_INDONESIAN,
        MATA_LANG_ENGLISH
    };

    class SDK_LANGUAGE_RESOURCE {
    private:
        std::map<std::string, std::wstring> Strings{};
        int CurrentLanguage = MATA_LANG_KOREAN;

    public:
        void Load(int Lang);
        const wchar_t* Get(std::string Key);
        int GetCurrentLanguage() { return CurrentLanguage; }
    };

    extern SDK_LANGUAGE_RESOURCE LANG;
}

#define GET_STR(key) SDK::LANG.Get(key)
