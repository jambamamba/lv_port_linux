#include <algorithm>
#include <debug_logger/debug_logger.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <openssl/sha.h>
#include <unordered_map>
#include <vector>

#include "tr.h"

LOG_CATEGORY(LVSIM, "LVSIM");

//sans vs serif, serif vs sans
//use serif fonts for printing
//use sans fonts for digitial media
//
//latin fonts: sudo apt-get install -y fonts-noto-core
//non-latin fonts can be found in Ubuntu in the path: /usr/share/fonts/truetype/noto/
//japanese fonts: sudo apt install -y fonts-noto-cjk
//  /repos/lv_port_linux/src/res/fonts/.ttc/noto/NotoSansCJK-*.ttc
//hindi/indian font: Devanagari
namespace {
const std::unordered_map<std::string, std::string> _language_map = {
    {"ab", "Abkhazian"},
    {"aa", "Afar"},
    {"af", "Afrikaans"},
    {"ak", "Akan"},
    {"sq", "Albanian"},
    {"am", "Amharic"},
    {"ar", "Arabic"},
    {"an", "Aragonese"},
    {"hy", "Armenian"},
    {"as", "Assamese"},
    {"av", "Avaric"},
    {"ae", "Avestan"},
    {"ay", "Aymara"},
    {"az", "Azerbaijani"},
    {"bm", "Bambara"},
    {"ba", "Bashkir"},
    {"eu", "Basque"},
    {"be", "Belarusian"},
    {"bn", "Bengali"},
    {"bi", "Bislama"},
    {"bs", "Bosnian"},
    {"br", "Breton"},
    {"bg", "Bulgarian"},
    {"my", "Burmese"},
    {"ca", "Catalan"},
    {"ch", "Chamorro"},
    {"ce", "Chechen"},
    {"ny", "Chichewa"},
    {"zh", "Chinese"},
    {"ic", "Church Slavon"},
    {"cv", "Chuvash"},
    {"kw", "Cornish"},
    {"co", "Corsican"},
    {"cr", "Cree"},
    {"hr", "Croatian"},
    {"cs", "Czech"},
    {"da", "Danish"},
    {"dv", "Divehi"},
    {"nl", "Dutch"},
    {"dz", "Dzongkha"},
    {"en", "English"},
    {"eo", "Esperanto"},
    {"et", "Estonian"},
    {"ee", "Ewe"},
    {"fo", "Faroese"},
    {"fj", "Fijian"},
    {"fi", "Finnish"},
    {"fr", "French"},
    {"an", "Western Frisi"},
    {"ff", "Fulah"},
    {"gd", "Gaelic"},
    {"gl", "Galician"},
    {"lg", "Ganda"},
    {"ka", "Georgian"},
    {"de", "German"},
    {"el", "Greek"},
    {"kl", "Kalaallisut"},
    {"gn", "Guarani"},
    {"gu", "Gujarati"},
    {"kl", "Haitian"},
    {"ha", "Hausa"},
    {"haw", "Hawaiian",},
    {"he", "Hebrew"},
    {"hz", "Herero"},
    {"hi", "Hindi"},
    {"ho", "Hiri Motu"},
    {"hu", "Hungarian"},
    {"is", "Icelandic"},
    {"io", "Ido"},
    {"ig", "Igbo"},
    {"id", "Indonesian"},
    {"iu", "Inuktitut"},
    {"ik", "Inupiaq"},
    {"ga", "Irish"},
    {"it", "Italian"},
    {"ja", "Japanese"},
    {"jv", "Javanese"},
    {"kn", "Kannada"},
    {"kr", "Kanuri"},
    {"ks", "Kashmiri"},
    {"kk", "Kazakh"},
    {"km", "Central Khmer"},
    {"ki", "Kikuyu"},
    {"rw", "Kinyarwanda"},
    {"ky", "Kyrgyz"},
    {"kv", "Komi"},
    {"kg", "Kongo"},
    {"ko", "Korean"},
    {"kj", "Kuanyama"},
    {"ku", "Kurdish"},
    {"lo", "Lao"},
    {"la", "Latin"},
    {"lv", "Latvian"},
    {"li", "Limburgan"},
    {"ln", "Lingala"},
    {"lt", "Lithuanian"},
    {"lu", "Luba-Katanga"},
    {"lb", "Luxembourgish" },
    {"mk", "Macedonian"},
    {"mg", "Malagasy"},
    {"ms", "Malay"},
    {"ml", "Malayalam"},
    {"mt", "Maltese"},
    {"gv", "Manx"},
    {"mi", "Maori"},
    {"mr", "Marathi"},
    {"mh", "Marshallese"},
    {"mn", "Mongolian"},
    {"na", "Nauru"},
    {"nv", "Navajo"},
    {"nd", "North Ndebele"},
    {"nr", "South Ndebele"},
    {"ng", "Ndonga"},
    {"ne", "Nepali"},
    {"no", "Norwegian"},
    {"nb", "Norwegian Bokm"},
    {"nn", "Norwegian Nynorsk"},
    {"oc", "Occitan"},
    {"oj", "Ojibwa"},
    {"or", "Oriya"},
    {"om", "Oromo"},
    {"os", "Ossetian"},
    {"pi", "Pali"},
    {"ps", "Pashto"},
    {"fa", "Persian"},
    {"pl", "Polish"},
    {"pt", "Portuguese"},
    {"pa", "Punjabi"},
    {"qu", "Quechua"},
    {"ro", "Romanian"},
    {"rm", "Romansh"},
    {"rn", "Rundi"},
    {"ru", "Russian"},
    {"se", "Northern Sami"},
    {"sm", "Samoan"},
    {"sg", "Sango"},
    {"sa", "Sanskrit"},
    {"sc", "Sardinian"},
    {"sr", "Serbian"},
    {"sn", "Shona"},
    {"sd", "Sindhi"},
    {"si", "Sinhala"},
    {"sk", "Slovak"},
    {"sl", "Slovenian"},
    {"so", "Somali"},
    {"st", "Southern Sotho",},
    {"es", "Spanish"},
    {"su", "Sundanese"},
    {"sw", "Swahili"},
    {"ss", "Swati"},
    {"sv", "Swedish"},
    {"tl", "Tagalog"},
    {"ty", "Tahitian"},
    {"tg", "Tajik"},
    {"ta", "Tamil"},
    {"tt", "Tatar"},
    {"te", "Telugu"},
    {"th", "Thai"},
    {"bo", "Tibetan"},
    {"ti", "Tigrinya"},
    {"to", "Tonga"},
    {"ts", "Tsonga"},
    {"tn", "Tswana"},
    {"tr", "Turkish"},
    {"tk", "Turkmen"},
    {"tw", "Twi"},
    {"ug", "Uighur"},
    {"uk", "Ukrainian"},
    {"ur", "Urdu"},
    {"uz", "Uzbek"},
    {"ve", "Venda"},
    {"vi", "Vietnamese"},
    {"vo", "Volapük"},
    {"wa", "Walloon"},
    {"cy", "Welsh"},
    {"wo", "Wolof"},
    {"xh", "Xhosa"},
    {"Yi", "Sichuan"},
    {"yi", "Yiddish"},
    {"yo", "Yoruba"},
    {"za", "Zhuang"},
    {"zu", "Zulu"}
};

std::string _default_language("en");
std::string _current_language("es");
std::vector<std::function<void()>> _all_text_objects = {};

bool setLanguage(const std::string &input_lang, std::string &toset) {
    for(const auto &[lang_code, lang_name] : _language_map) {
        std::string lowercase_lang_code(lang_code);
        for(char *c = (char*) lowercase_lang_code.c_str(); *c; c++) {
            *c = std::tolower(*c);
        }
        std::string lowercase_lang_name(lang_name);
        for(char *c = (char*) lowercase_lang_name.c_str(); *c; c++) {
            *c = std::tolower(*c);
        }
        if(input_lang == lowercase_lang_code || input_lang == lowercase_lang_name) {
            toset = lang_code;
            return true;
        }
    }
    return false;
}

std::string sha256(const std::string inputStr) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // SHA256 function calculates the hash of the data
    SHA256(reinterpret_cast<const unsigned char*>(inputStr.c_str()), inputStr.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::unordered_map<std::string, std::string> _translation_map = {};
}//namespace

LeleLanguage &LeleLanguage::getLeleLanguage() {
  static LeleLanguage _lele_translation;
  return _lele_translation;
}

const std::unordered_map<std::string, std::string> &LeleLanguage::getAvailableLanguages() {
    return _language_map;
    // std::vector<std::string> languages;
    // for(const auto &[lang_code, lang_name] : _language_map) {
    //     languages.emplace_back(lang_name);
    // }
    // return languages;
}

std::string LeleLanguage::getCurrentLanguage() {
    return _language_map.at(_current_language);
}

std::string LeleLanguage::getDefaultLanguage() {
    return _language_map.at(_default_language);
}

bool LeleLanguage::setCurrentLanguage(const std::string &language) {
    for(char *c = (char*) language.c_str(); *c; c++) {
        *c = std::tolower(*c);
    }

    if(setLanguage(language, _current_language)){
        _translation_map.clear();
        for(const auto &set_text_func: _all_text_objects) {
            set_text_func();
        }
        return true;
    }
    return false;
}

bool LeleLanguage::setDefaultLanguage(const std::string &language) {
    for(char *c = (char*) language.c_str(); *c; c++) {
        *c = std::tolower(*c);
    }
    return setLanguage(language, _default_language);
}

//global function:
std::string tr(const std::string &txt) {
    if(_current_language == _default_language || txt.size() == 0) {
        return txt;
    }
    std::string hash = sha256(txt);
    auto it = _translation_map.find(hash);
    if(it != _translation_map.end()){
        return it->second;
    }
    std::stringstream ss;
    std::string current_dir = std::filesystem::current_path();
    ss << current_dir << "/tr/" << _current_language << "/" << hash;
    std::ifstream file_in(ss.str(), std::ios_base::in | std::ios_base::binary);
    if (file_in.is_open()) {
        std::string content = std::string(std::istreambuf_iterator<char>(file_in), std::istreambuf_iterator<char>());
        _translation_map.insert({hash, content});
        return content;
    }
    LL(WARNING, LVSIM) << "Error opening file: " << ss.str();
    ss.str("");
    ss.clear();
    ss << current_dir << "/tr/" << _current_language << "/" << ".missing";
    std::filesystem::path nested_path = ss.str();
    if(!std::filesystem::exists(nested_path)) {
        if(!std::filesystem::create_directories(nested_path)){
            LL(WARNING, LVSIM) << "Error creating dir: " << ss.str();
            return txt;
        }
    }

    ss << "/" << hash;
    if(!std::filesystem::exists(ss.str())) {
        std::ofstream file_out(ss.str());
        file_out << txt;
    }

    return txt;
}

//global function:
void lele_set_translatable_text(std::function<void()> set_text_func) {
    _all_text_objects.emplace_back(set_text_func);
    set_text_func();
}