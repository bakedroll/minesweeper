#include "HighScoreDialog.h"

#include "ui_HighScoreDialog.h"

QString getStringFromDifficultyMode(const DifficultyMode& mode)
{
    switch (mode)
    {
    case DifficultyMode::Beginner:
        return QApplication::translate("Difficulty", "Beginner");
    case DifficultyMode::Intermediate:
        return QApplication::translate("Difficulty", "Intermediate");
    case DifficultyMode::Expert:
        return QApplication::translate("Difficulty", "Expert");
    case DifficultyMode::CustomGame:
        return QApplication::translate("Difficulty", "Custom game");
    default:
        break;
    };

    return QString();
}

HighScoreDialog::HighScoreDialog(DifficultyMode difficultyMode, QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::HighScoreDialog())
{
    m_ui->setupUi(this);

    for (auto i=0; i<underlying(DifficultyMode::DifficultyModeCount); i++)
    {
        auto mode = static_cast<DifficultyMode>(i);

        auto label = new QLabel();
        label->setStyleSheet("margin: 10px;");
        label->setAlignment(Qt::AlignTop);

        m_ui->tabWidget->addTab(label,
            getStringFromDifficultyMode(mode));

        if (mode == difficultyMode)
        {
            m_ui->tabWidget->setCurrentIndex(i);
        }

        m_pages[mode] = label;
    }
}

HighScoreDialog::~HighScoreDialog() = default;

void HighScoreDialog::setDifficultyScoreText(DifficultyMode mode, const QString& text)
{
    m_pages[mode]->setText(text);
}