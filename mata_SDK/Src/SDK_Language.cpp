#define _CRT_SECURE_NO_WARNINGS
#include "SDK_Language.h"
#include "SDK_StringTool.h"
#include "Library/tinyxml/tinyxml.h"
#include <filesystem>

SDK::SDK_LANGUAGE_RESOURCE SDK::LANG;

void SDK::SDK_LANGUAGE_RESOURCE::Load(int Lang) {
    Strings.clear();
    CurrentLanguage = Lang;

    std::string FileName;
    switch (Lang) {
    case MATA_LANG_KOREAN:    FileName = "Korean.xml"; break;
    case MATA_LANG_INDONESIAN: FileName = "Indonesian.xml"; break;
    case MATA_LANG_ENGLISH:   FileName = "English.xml"; break;
    default:             FileName = "English.xml"; break;
    }

    std::string FilePath = "SDKResource/Language/" + FileName;
    
    TiXmlDocument Doc;
    if (!Doc.LoadFile(FilePath.c_str(), TIXML_ENCODING_UTF8)) {
        if (Lang != MATA_LANG_KOREAN) {
             Load(MATA_LANG_KOREAN);
             return;
        }
        return;
    }

    TiXmlElement* Root = Doc.FirstChildElement("Language");
    if (!Root) return;

    // Logika parsing yang bener:
    // Category (e.g., <Menu>) -> Entry (e.g., <Start Value="..."/>)
    TiXmlElement* Category = Root->FirstChildElement();
    while (Category) {
        TiXmlElement* Entry = Category->FirstChildElement();
        while (Entry) {
            const char* Key = Entry->Value(); // Ini dapet "Start", "Settings", dll
            const char* Val = Entry->Attribute("Value"); // Ini dapet isi terjemahannya
            
            if (Key && Val) {
                Strings[Key] = StringTool.Wstring(Val);
            }
            Entry = Entry->NextSiblingElement();
        }
        Category = Category->NextSiblingElement();
    }
}

const wchar_t* SDK::SDK_LANGUAGE_RESOURCE::Get(std::string Key) {
    auto It = Strings.find(Key);
    if (It != Strings.end()) {
        return It->second.c_str();
    }
    
    static std::wstring Fallback;
    Fallback = SDK::StringTool.Wstring(Key);
    return Fallback.c_str();
}
