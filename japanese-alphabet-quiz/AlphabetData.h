#ifndef ALPHABETDATA_H
#define ALPHABETDATA_H
#include <vector>
#include <QString>

struct KanaEntry {
    QString kana;
    QString romaji;
};

namespace AlphabetData {
    extern const std::vector<std::vector<KanaEntry>> Hiragana;
    extern const std::vector<std::vector<KanaEntry>> Katakana;
    extern const std::vector<std::vector<KanaEntry>> Kanji;
}

#endif // ALPHABETDATA_H
