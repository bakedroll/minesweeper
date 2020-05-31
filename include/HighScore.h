#pragma once

#include <QString>
#include <QWidget>

#include <vector>

class HighScore
{
public:
    enum class DifficultyMode : int
    {
        Beginner = 0,
        Intermediate = 1,
        Expert = 2,
        CustomGame = 3
    };

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

    bool hasReachedTopThree(int totalScore);
    void addScoreData(ScoreData& data);

    void displayScore(QWidget* parent = nullptr, HighscoreDisplayMode mode = HighscoreDisplayMode::NoHighlighting);

private:
    using TopList = std::vector<ScoreData>;

    TopList m_topList;
    int m_lastAddedId;

};