#pragma once

#include "HighScore.h"
#include "CustomGameDialog.h"

#include <QMainWindow>
#include <QPushButton>
#include <QPointer>
#include <QTimer>

#include <memory>
#include <functional>
#include <set>

namespace Ui
{
    class MainWindow;
}

enum class ButtonState
{
    Hidden,
    Uncovered,
    Flagged,
    Unclear
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

private Q_SLOTS:
    void incrementClock();
    void loadDifficultyMode(int index);
    void resetMineFieldRandomized();
    void resetMineField();
    void randomizeSeed();

private:
    enum class GameState
    {
        Running,
        Succeeded,
        Failed
    };

    struct MineField
    {
        QPointer<QPushButton> button = nullptr;
        ButtonState state = ButtonState::Hidden;
    };

    using Coordinate = std::pair<int, int>;
    using FieldFunction = std::function<void(const Coordinate&)>;
    using CoordinateSet = std::set<Coordinate>;

    std::unique_ptr<Ui::MainWindow> m_ui;

    void makeMinefield();
    void placeMines(int quantity, const Coordinate& excludedCoord);

    void setGameState(GameState state);

    void checkMineField(const Coordinate& coord);
    void toggleMineFieldFlag(const Coordinate& coord);
    void uncoverField(const Coordinate& coord, CoordinateSet& visited);
    void uncoverAll();
    void checkWinningCondition();

    void forEachField(FieldFunction func);
    void forEachAdjacentField(const Coordinate& coord, FieldFunction func);

    int countMinesAdjacentTo(const Coordinate& coord);

    void checkHighScore();

    int count3BV();
    void floodFillMark(const Coordinate& coord, CoordinateSet& marked);

    int m_difficultyIndex;
    DifficultyParams m_params;

    CoordinateSet m_mines;
    std::map<Coordinate, MineField> m_mineFields;
    std::map<HighScore::DifficultyMode, DifficultyParams> m_difficultyModes;

    GameState m_state;

    std::map<int, QColor> m_digitColors;

    QIcon m_iconFlag;
    QIcon m_iconMine;
    QIcon m_iconMineMissed;

    QIcon m_iconSmileyHappy;
    QIcon m_iconSmileyExcited;
    QIcon m_iconSmileyGlasses;
    QIcon m_iconSmileyAngry;

    bool m_minesPlaced;
    int m_numClicks;

    QTimer m_timer;

    HighScore m_highScore;
};