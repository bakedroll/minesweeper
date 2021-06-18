#pragma once

#include <QDialog>
#include <QPointer>
#include <QLabel>

#include <memory>

namespace Ui
{
    class HighScoreDialog;
}

template <typename T>
typename std::underlying_type<T>::type underlying(T t)
{
    return static_cast<typename std::underlying_type<T>::type>(t);
}

enum class DifficultyMode : int
{
    Beginner,
    Intermediate,
    Expert,
    CustomGame,
    DifficultyModeCount
};

QString getStringFromDifficultyMode(const DifficultyMode& mode);

class HighScoreDialog : public QDialog
{
    Q_OBJECT

public:
    HighScoreDialog(DifficultyMode mode, QWidget* parent = nullptr);
    virtual ~HighScoreDialog();

    void setDifficultyScoreText(DifficultyMode difficultyMode,
        const QString& text);

private:
    std::unique_ptr<Ui::HighScoreDialog> m_ui;
    std::map<DifficultyMode, QPointer<QLabel>> m_pages;

};