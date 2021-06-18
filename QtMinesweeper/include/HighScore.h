#pragma once

#include "HighScoreDialog.h"

#include <QString>
#include <QWidget>

#include <vector>

class HighScore : QObject
{
    Q_OBJECT

public:
    struct ScoreData
    {
        int id = -1;
        QString name;
        int score3bv = 0;
        float score3bvPerTime = 0.0f;
        float score3bvPerClicks = 0.0f;
        int totalScore = 0;
        int time = 0;
        int clicks = 0;
        DifficultyMode mode;
    };

    enum class HighscoreDisplayMode
    {
        HighlightLastAdded,
        NoHighlighting
    };

    HighScore();
    virtual ~HighScore();

    void loadScore();
    void saveScore();

    bool hasReachedTopThree(DifficultyMode mode, int totalScore);
    void addScoreData(DifficultyMode mode, ScoreData& data);

    void displayScore(DifficultyMode difficultyMode, QWidget* parent = nullptr,
        HighscoreDisplayMode mode = HighscoreDisplayMode::NoHighlighting);

private:
    using TopList = std::vector<ScoreData>;
    using HighScoresMap = std::map<DifficultyMode, TopList>;

    HighScoresMap m_highScores;

    struct LastAddedEntry
    {
        DifficultyMode mode = DifficultyMode::Beginner;
        int id = -1;
    };

    LastAddedEntry m_lastAddedEntry;

};