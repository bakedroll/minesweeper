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
        QString name;
        int score3bv;
        float score3bvPerTime;
        float score3bvPerClicks;
        float totalScore;
        int time;
        DifficultyMode mode;
    };

    HighScore();
    virtual ~HighScore();

    void loadScore();
    void saveScore();

    bool hasReachedTopThree(int totalScore);
    void addScoreData(const ScoreData& data);

    void displayScore(QWidget* parent = nullptr);

private:
    using TopList = std::vector<ScoreData>;

    TopList m_topList;

};