#include "HighScore.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

QString getHighScoreFilename()
{
    return "./highscore.txt";
}

int getMaxHighscoreEntries()
{
    return 10;
}

QString formatHtmlColor(const QString& text, const QColor& color)
{
    return QString("<span style=\"color:%1;\">%2</span>").arg(color.name()).arg(text);
}

HighScore::HighScore()
    : QObject()
{
    loadScore();
}

HighScore::~HighScore() = default;

void HighScore::loadScore()
{
    for (auto i=0; i<underlying(DifficultyMode::DifficultyModeCount); i++)
    {
        m_highScores[static_cast<DifficultyMode>(i)].clear();
    }

    QFile file(getHighScoreFilename());
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto modeIndex = 0;

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        auto line = stream.readLine();

        if (line == "---")
        {
            modeIndex++;
            continue;
        }

        auto elements = line.split(';');

        if (elements.length() < 8)
        {
            continue;
        }

        auto& topList = m_highScores[static_cast<DifficultyMode>(modeIndex)];

        ScoreData data;
        data.id = static_cast<int>(topList.size());
        data.name = elements[0];
        data.score3bv = elements[1].toInt();
        data.score3bvPerTime = elements[2].toFloat();
        data.score3bvPerClicks = elements[3].toFloat();
        data.totalScore = elements[4].toInt();
        data.clicks = elements[5].toInt();
        data.time = elements[6].toInt();
        data.mode = static_cast<DifficultyMode>(elements[7].toInt());

        topList.push_back(data);
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

    for (const auto& highscore : m_highScores)
    {
        for (const auto& data : highscore.second)
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

        stream << "---\n";
    }

    file.close();
}

bool HighScore::hasReachedTopThree(DifficultyMode mode, int totalScore)
{
    auto& topList = m_highScores[mode];

    if (topList.size() < getMaxHighscoreEntries())
    {
        return true;
    }

    return (topList.rbegin()->totalScore < totalScore);
}

void HighScore::addScoreData(DifficultyMode mode, ScoreData& data)
{
    auto& topList = m_highScores[mode];

    data.id = static_cast<int>(topList.size());
    topList.push_back(data);

    m_lastAddedEntry.mode = mode;
    m_lastAddedEntry.id = data.id;

    std::sort(topList.begin(), topList.end(), [](const ScoreData& lhs, const ScoreData& rhs)
    {
        return lhs.totalScore > rhs.totalScore;       
    });

    auto maxEntries = getMaxHighscoreEntries();
    if (static_cast<int>(topList.size()) > maxEntries)
    {
        topList = TopList(topList.begin(), topList.begin()+maxEntries);
    }
}

void HighScore::displayScore(DifficultyMode difficultyMode, QWidget* parent,
    HighscoreDisplayMode mode)
{
    HighScoreDialog dialog(difficultyMode, parent);

    QPalette palette;
    auto defaultColor = palette.color(QPalette::Text);

    for (const auto& highscore : m_highScores)
    {
        auto dmode = highscore.first;
        auto& topList = highscore.second;

        if (topList.empty())
        {
            dialog.setDifficultyScoreText(dmode, QString("<h4>%1</h4>").arg(tr("No entries")));
            continue;
        }

        QString tableRows;
        for (const auto& data : topList)
        {
            QColor color(((mode == HighscoreDisplayMode::HighlightLastAdded)
                && (data.id == m_lastAddedEntry.id)
                && (data.mode == m_lastAddedEntry.mode))
                ? Qt::darkGreen : defaultColor);

            tableRows.append(QString("<tr><td>%1</td><td><b>%2</b></td><td>%3</td>" \
                "<td>%4</td><td>%5</td><td>%6</td></tr>")
                .arg(formatHtmlColor(data.name, color))
                .arg(formatHtmlColor(QString::number(data.totalScore), color))
                .arg(formatHtmlColor(QString::number(data.score3bv), color))
                .arg(formatHtmlColor(QString::number(data.clicks), color))
                .arg(formatHtmlColor(QString::number(data.time) + " s", color))
                .arg(formatHtmlColor(getStringFromDifficultyMode(data.mode), color)));
        }

        auto html = QString("<table>" \
            "<tr><th width=\"100\" align=\"left\">%2</th><th width=\"100\" align=\"left\"><b>%3</b></th>" \
            "<th width=\"130\" align=\"left\">%4</th><th width=\"100\" align=\"left\">%5</th>" \
            "<th width=\"100\" align=\"left\">%6</th><th width=\"100\" align=\"left\">%7</th></tr>%1</table>")
            .arg(tableRows)
            .arg(tr("Name"))
            .arg(tr("Total score"))
            .arg(tr("3BV score (min clicks)"))
            .arg(tr("Clicks"))
            .arg(tr("Time"))
            .arg(tr("Difficulty"));

        dialog.setDifficultyScoreText(dmode, html);
    }

    dialog.exec();
}
