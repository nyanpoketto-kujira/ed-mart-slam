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
    case LANG_KOREAN:    FileName = "Korean.xml"; break;
    case LANG_INDONESIAN: FileName = "Indonesian.xml"; break;
    case LANG_ENGLISH:   FileName = "English.xml"; break;
    default:             FileName = "English.xml"; break;
    }

    std::string FilePath = "SDKResource/Language/" + FileName;
    
    TiXmlDocument Doc;
    if (!Doc.LoadFile(FilePath.c_str(), TIXML_ENCODING_UTF8)) {
        // Fallback to Korean if file not found
        if (Lang != LANG_KOREAN) {
             Load(LANG_KOREAN);
             return;
        }
        return;
    }

    TiXmlElement* Root = Doc.FirstChildElement("Language");
    if (!Root) return;

    TiXmlElement* Category = Root.FirstChildElement();
    while (Category) {
        TiXmlAttribute* Attribute = Category->FirstAttribute();
        while (Attribute) {
            Strings[Attribute->Name()] = StringTool.Wstring(Attribute->Value());
            Attribute = Attribute->Next();
        }
        Category = Category->NextSiblingElement();
    }
}

const wchar_t* SDK::SDK_LANGUAGE_RESOURCE::Get(std::string Key) {
    auto It = Strings.find(Key);
    if (It != Strings.end()) {
        return It->second.c_str();
    }
    
    // Fallback: return key as wstring (should be rare)
    static std::wstring Fallback;
    Fallback = SDK::StringTool.Wstring(Key);
    return Fallback.c_str();
}
