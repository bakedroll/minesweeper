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

        if (elements.length() < 5)
        {
            continue;
        }

        ScoreData data;
        data.name = elements[0];
        data.score3bv = elements[1].toInt();
        data.score3bvs = elements[2].toInt();
        data.time = elements[3].toInt();
        data.mode = static_cast<DifficultyMode>(elements[4].toInt());

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
        auto line = QString("%1;%2;%3;%4;%5\n")
            .arg(data.name)
            .arg(data.score3bv)
            .arg(data.score3bvs)
            .arg(data.time)
            .arg(underlying(data.mode));

        stream << line;
    }

    file.close();
}

bool HighScore::hasReachedTopThree(int score3bv)
{
    if (m_topList.size() < 3)
    {
        return true;
    }

    return (m_topList.rbegin()->score3bv < score3bv);
}

void HighScore::addScoreData(const ScoreData& data)
{
    m_topList.push_back(data);
    std::sort(m_topList.begin(), m_topList.end(), [](const ScoreData& lhs, const ScoreData& rhs)
    {
        return lhs.score3bv > rhs.score3bv;       
    });

    if (m_topList.size() > 3)
    {
        m_topList = TopList(m_topList.begin(), m_topList.begin()+3);
    }
}

void HighScore::displayScore(QWidget* parent)
{
    QString tableRows;
    for (const auto& data : m_topList)
    {
        tableRows.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4 s</td><td>%5</td></tr>")
            .arg(data.name)
            .arg(data.score3bv)
            .arg(data.score3bvs)
            .arg(data.time)
            .arg(getStringFromDifficultyMode(data.mode)));
    }

    auto html = QString("<h3><u>Highscore</u></h3><table>" \
        "<tr><th width=\"100\" align=\"left\">Name</th><th width=\"100\" align=\"left\">3BV score</th>" \
        "<th width=\"100\" align=\"left\">3BV score/s</th><th width=\"100\" align=\"left\">Time</th>" \
        "<th width=\"100\" align=\"left\">Difficulty</th></tr>%1</table>")
        .arg(tableRows);

    QMessageBox::information(parent, "Highscore TOP 3", html);
}