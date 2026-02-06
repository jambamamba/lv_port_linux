#include <algorithm>
#include <debug_logger/debug_logger.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <openssl/sha.h>
#include <vector>

#include "tr.h"

LOG_CATEGORY(LVSIM, "LVSIM");

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

std::string _default_lang("en");
std::string _current_lang("es");

bool setLanguage(const std::string &input, std::string &toset) {
    for(const auto &[lang_code, lang_name] : _language_map) {
        std::string rhs;
        std::transform(lang_name.begin(), lang_name.end(), rhs.begin(),
            [](unsigned char c){ return std::tolower(c); });

        if(input == rhs) {
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

namespace Translation {
std::vector<std::string> availableLanguages() {
    std::vector<std::string> languages;
    for(const auto &[lang_code, lang_name] : _language_map) {
        languages.emplace_back(lang_name);
    }
    return languages;
}

std::string getCurrentLanguage() {
    return _language_map.at(_current_lang);
}

std::string getDefaultLanguage() {
    return _language_map.at(_default_lang);
}

bool setCurrentLanguage(const std::string &language) {
    std::string input;
    std::transform(language.begin(), language.end(), input.begin(),
        [](unsigned char c){ return std::tolower(c); });

    if(setLanguage(input, _current_lang)){
        _translation_map.clear();
        return true;
    }
    return false;
}

bool setDefaultLanguage(const std::string &language) {
    std::string input;
    std::transform(language.begin(), language.end(), input.begin(),
        [](unsigned char c){ return std::tolower(c); });

    return setLanguage(input, _default_lang);
}

}//Translation

std::string tr(const std::string &txt) {
    if(_current_lang == _default_lang) {
        return txt;
    }
    std::string hash = sha256(txt);
    auto it = _translation_map.find(hash);
    if(it != _translation_map.end()){
        return it->second;
    }
    std::stringstream ss;
    ss << LELE_DATA_DIR << "/tr/" << _current_lang << "/" << hash;
    std::ifstream file_in(ss.str(), std::ios_base::in | std::ios_base::binary);
    if (file_in.is_open()) {
        std::string content = std::string(std::istreambuf_iterator<char>(file_in), std::istreambuf_iterator<char>());
        _translation_map.insert({hash, content});
        return content;
    }
    LL(WARNING, LVSIM) << "Error opening file: " << ss.str();
    ss.str("");
    ss.clear();
    ss << LELE_DATA_DIR << "/tr/" << _current_lang << "/" << "missing";
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