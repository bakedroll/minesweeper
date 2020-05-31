#include "HighScore.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

template <typename T>
typename std::underlying_type<T>::type underlying(T t)
{
    return static_cast<typename std::underlying_type<T>::type>(t);
}

QString getHighScoreFilename()
{
    return "./highscore.txt";
}

int getMaxHighscoreEntries()
{
    return 10;
}

QString getStringFromDifficultyMode(const HighScore::DifficultyMode& mode)
{
    switch (mode)
    {
    case HighScore::DifficultyMode::Beginner:
        return "Beginner";
    case HighScore::DifficultyMode::Intermediate:
        return "Intermediate";
    case HighScore::DifficultyMode::Expert:
        return "Expert";
    case HighScore::DifficultyMode::CustomGame:
        return "Custom game";
    default:
        break;
    };

    return QString();
}

HighScore::HighScore()
{
    loadScore();
}

HighScore::~HighScore() = default;

void HighScore::loadScore()
{
    QFile file(getHighScoreFilename());
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        auto line = stream.readLine();
        auto elements = line.split(';');

        if (elements.length() < 8)
        {
            continue;
        }

        ScoreData data;
        data.name = elements[0];
        data.score3bv = elements[1].toInt();
        data.score3bvPerTime = elements[2].toFloat();
        data.score3bvPerClicks = elements[3].toFloat();
        data.totalScore = elements[4].toFloat();
        data.clicks = elements[5].toInt();
        data.time = elements[6].toInt();
        data.mode = static_cast<DifficultyMode>(elements[7].toInt());

        m_topList.push_back(data);
    }

    file.close();
}

void HighScore::saveScore()
{
    QFile file(getHighScoreFilename());
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream stream(&file);
    for (const auto& data : m_topList)
    {
        auto line = QString("%1;%2;%3;%4;%5;%6;%7;%8\n")
            .arg(data.name)
            .arg(data.score3bv)
            .arg(data.score3bvPerTime)
            .arg(data.score3bvPerClicks)
            .arg(data.totalScore)
            .arg(data.clicks)
            .arg(data.time)
            .arg(underlying(data.mode));

        stream << line;
    }

    file.close();
}

bool HighScore::hasReachedTopThree(int totalScore)
{
    if (m_topList.size() < getMaxHighscoreEntries())
    {
        return true;
    }

    return (m_topList.rbegin()->totalScore < totalScore);
}

void HighScore::addScoreData(const ScoreData& data)
{
    m_topList.push_back(data);
    std::sort(m_topList.begin(), m_topList.end(), [](const ScoreData& lhs, const ScoreData& rhs)
    {
        return lhs.totalScore > rhs.totalScore;       
    });

    auto maxEntries = getMaxHighscoreEntries();
    if (m_topList.size() > maxEntries)
    {
        m_topList = TopList(m_topList.begin(), m_topList.begin()+maxEntries);
    }
}

void HighScore::displayScore(QWidget* parent)
{
    QString tableRows;
    for (const auto& data : m_topList)
    {
        tableRows.append(QString("<tr><td>%1</td><td><b>%2</b></td><td>%3</td><td>%4</td><td>%5 s</td><td>%6</td></tr>")
            .arg(data.name)
            .arg(data.totalScore)
            .arg(data.score3bv)
            .arg(data.clicks)
            .arg(data.time)
            .arg(getStringFromDifficultyMode(data.mode)));
    }

    auto html = QString("<h3><u>Highscore</u></h3><table>" \
        "<tr><th width=\"100\" align=\"left\">Name</th><th width=\"100\" align=\"left\"><b>Total score</b></th>" \
        "<th width=\"130\" align=\"left\">3BV score (min clicks)</th><th width=\"100\" align=\"left\">Clicks</th>" \
        "<th width=\"100\" align=\"left\">Time</th><th width=\"100\" align=\"left\">Difficulty</th></tr>%1</table>")
        .arg(tableRows);

    QMessageBox::information(parent, QString("Highscore TOP %1").arg(m_topList.size()), html);
}