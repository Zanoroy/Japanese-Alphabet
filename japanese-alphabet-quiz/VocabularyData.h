#ifndef VOCABULARYDATA_H
#define VOCABULARYDATA_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include <map>
#include <cstddef>
#include <algorithm>

// Forward declarations
class QString;
class QJsonObject;

struct VocabularyWord {
    QString romaji;
    QString japanese;
    QString kanji; // Optional kanji representation
    QString english;
    QString comment; // Optional comment for additional info
};

struct VocabularyScore {
    double bestRomajiPercent = 0.0;
    double bestEnglishPercent = 0.0;
};

struct Vocabulary {
    QString name;
    std::vector<VocabularyWord> words;
};

// Profile-based vocabulary scores
using ProfileScores = std::map<QString, std::map<QString, VocabularyScore>>; // [profileName][vocabularyName] -> scores

class VocabularyData {
public:
    static bool loadVocabularies(const QString &filePath, std::vector<Vocabulary> &vocabularies);
    static bool saveVocabularies(const QString &filePath, const std::vector<Vocabulary> &vocabularies);
    static std::vector<VocabularyWord> getAllWords(const std::vector<Vocabulary> &vocabularies);
    
    // Score management
    static bool loadProfileScores(const QString &filePath, ProfileScores &scores);
    static bool saveProfileScores(const QString &filePath, const ProfileScores &scores);
    static VocabularyScore getProfileVocabularyScore(const ProfileScores &scores, 
                                                    const QString &profileName, 
                                                    const QString &vocabularyName);
    static void updateProfileVocabularyScore(ProfileScores &scores,
                                           const QString &profileName,
                                           const QString &vocabularyName,
                                           double romajiPercent,
                                           double englishPercent);
    
private:
    static Vocabulary parseVocabulary(const QJsonObject &vocabObj);
    static QJsonObject vocabularyToJson(const Vocabulary &vocab);
    static QJsonObject scoreToJson(const VocabularyScore &score);
    static VocabularyScore parseScore(const QJsonObject &scoreObj);
};

#endif // VOCABULARYDATA_H