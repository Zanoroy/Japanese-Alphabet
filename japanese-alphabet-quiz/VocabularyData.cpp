#include "VocabularyData.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QDebug>

bool VocabularyData::loadVocabularies(const QString &filePath, std::vector<Vocabulary> &vocabularies) {
    vocabularies.clear();
    
    QFile file(filePath);
    if (!file.exists()) {
        // Create a default vocabulary file
        std::vector<Vocabulary> defaultVocabs;
        Vocabulary basicVocab;
        basicVocab.name = "Basic Words";
        basicVocab.words = {
            {"こんにちは", "konnichiwa", "hello"},
            {"ありがとう", "arigatou", "thank you"},
            {"さようなら", "sayounara", "goodbye"},
            {"はい", "hai", "yes"},
            {"いいえ", "iie", "no"}
        };
        defaultVocabs.push_back(basicVocab);
        
        saveVocabularies(filePath, defaultVocabs);
        vocabularies = defaultVocabs;
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open vocabulary file:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return false;
    }
    
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON structure - not an object";
        return false;
    }
    
    QJsonObject root = doc.object();
    if (!root.contains("Vocabularies") || !root["Vocabularies"].isArray()) {
        qDebug() << "Missing or invalid 'Vocabularies' array";
        return false;
    }
    
    QJsonArray vocabArray = root["Vocabularies"].toArray();
    for (const QJsonValue &value : vocabArray) {
        if (value.isObject()) {
            Vocabulary vocab = parseVocabulary(value.toObject());
            if (!vocab.name.isEmpty()) {
                vocabularies.push_back(vocab);
            }
        }
    }
    
    return true;
}

bool VocabularyData::saveVocabularies(const QString &filePath, const std::vector<Vocabulary> &vocabularies) {
    QJsonObject root;
    QJsonArray vocabArray;
    
    for (const Vocabulary &vocab : vocabularies) {
        vocabArray.append(vocabularyToJson(vocab));
    }
    
    root["Vocabularies"] = vocabArray;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << filePath;
        return false;
    }
    
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

std::vector<VocabularyWord> VocabularyData::getAllWords(const std::vector<Vocabulary> &vocabularies) {
    std::vector<VocabularyWord> allWords;
    
    for (const Vocabulary &vocab : vocabularies) {
        for (const VocabularyWord &word : vocab.words) {
            allWords.push_back(word);
        }
    }
    
    return allWords;
}

Vocabulary VocabularyData::parseVocabulary(const QJsonObject &vocabObj) {
    Vocabulary vocab;
    
    if (vocabObj.contains("Vocabulary") && vocabObj["Vocabulary"].isString()) {
        vocab.name = vocabObj["Vocabulary"].toString();
    }
    
    if (vocabObj.contains("words") && vocabObj["words"].isArray()) {
        QJsonArray wordsArray = vocabObj["words"].toArray();
        
        for (const QJsonValue &wordValue : wordsArray) {
            if (wordValue.isObject()) {
                QJsonObject wordObj = wordValue.toObject();
                VocabularyWord word;
                
                if (wordObj.contains("japanese")) {
                    word.japanese = wordObj["japanese"].toString();
                }
                if (wordObj.contains("romaji")) {
                    word.romaji = wordObj["romaji"].toString();
                }
                if (wordObj.contains("english")) {
                    word.english = wordObj["english"].toString();
                }
                
                if (!word.japanese.isEmpty() && !word.romaji.isEmpty() && !word.english.isEmpty()) {
                    vocab.words.push_back(word);
                }
            }
        }
    }
    
    return vocab;
}

QJsonObject VocabularyData::vocabularyToJson(const Vocabulary &vocab) {
    QJsonObject vocabObj;
    vocabObj["Vocabulary"] = vocab.name;
    
    QJsonArray wordsArray;
    for (const VocabularyWord &word : vocab.words) {
        QJsonObject wordObj;
        wordObj["japanese"] = word.japanese;
        wordObj["romaji"] = word.romaji;
        wordObj["english"] = word.english;
        wordsArray.append(wordObj);
    }
    
    vocabObj["words"] = wordsArray;
    return vocabObj;
}