#ifndef VOCABULARYDATA_H
#define VOCABULARYDATA_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>

struct VocabularyWord {
    QString japanese;
    QString romaji;
    QString english;
};

struct Vocabulary {
    QString name;
    std::vector<VocabularyWord> words;
};

class VocabularyData {
public:
    static bool loadVocabularies(const QString &filePath, std::vector<Vocabulary> &vocabularies);
    static bool saveVocabularies(const QString &filePath, const std::vector<Vocabulary> &vocabularies);
    static std::vector<VocabularyWord> getAllWords(const std::vector<Vocabulary> &vocabularies);
    
private:
    static Vocabulary parseVocabulary(const QJsonObject &vocabObj);
    static QJsonObject vocabularyToJson(const Vocabulary &vocab);
};

#endif // VOCABULARYDATA_H