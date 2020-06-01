#include "MainWindow.h"
#include "RightClickableButton.h"

#include "ui_MainWindow.h"

#include <QDesktopWidget>
#include <QInputDialog>
#include <QTime>

enum class FontWeight
{
    Normal,
    Bold
};

void setWidgetStyle(QWidget* widget, const QColor& color, FontWeight weight = FontWeight::Normal)
{
    widget->setStyleSheet(QString("color: %1; font-weight: %2;")
        .arg(color.name())
        .arg((weight == FontWeight::Bold) ? "bold" : "normal"));
}

void setButtonIcon(QPushButton* button, const QIcon& icon, const QSize& size = QSize(32, 32))
{
    button->setIcon(icon);
    button->setIconSize(size);
}

void resetButtonStyleAndText(QPushButton* button)
{
    button->setText("");
    button->setStyleSheet("");
}

void deleteLayout(QLayout* layout)
{
    if (!layout)
    {
        return;
    }

    QLayoutItem* item;
    QWidget* widget;

    while ((item = layout->takeAt(0)))
    {
        if (item->layout())
        {
            deleteLayout(item->layout());
        }
        else if ((widget = item->widget()) != 0)
        {
            widget->hide();
            delete widget;
        }
        else 
        {
            delete item;
        }
    }

    delete layout;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow())
    , m_state(GameState::Running)
    , m_iconFlag(":/Resources/flag_small.png")
    , m_iconMine(":/Resources/mine_small.png")
    , m_iconMineMissed(":/Resources/mine_missed_small.png")
    , m_iconSmileyHappy(":/Resources/smiley_happy_small.png")
    , m_iconSmileyExcited(":/Resources/smiley_excited_small.png")
    , m_iconSmileyGlasses(":/Resources/smiley_glasses_small.png")
    , m_iconSmileyAngry(":/Resources/smiley_angry_small.png")
    , m_difficultyIndex(0)
    , m_minesPlaced(false)
    , m_numClicks(0)
{
    m_ui->setupUi(this);

    setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));

    setButtonIcon(m_ui->buttonRandomize, QIcon(":/Resources/randomize_small.png"), QSize(24, 24));
    setButtonIcon(m_ui->buttonSeed, QIcon(":/Resources/seed_small.png"), QSize(24, 24));

    for (auto i=0; i<underlying(DifficultyMode::DifficultyModeCount); i++)
    {
        m_ui->comboBoxDifficultyMode->addItem(
            getStringFromDifficultyMode(static_cast<DifficultyMode>(i)));
    }

    m_difficultyIndex = underlying(DifficultyMode::Beginner);
    m_ui->comboBoxDifficultyMode->setCurrentIndex(m_difficultyIndex);

    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);

    m_digitColors[1] = QColor(Qt::blue);
    m_digitColors[2] = QColor(Qt::green);
    m_digitColors[3] = QColor(Qt::darkMagenta);
    m_digitColors[4] = QColor(Qt::darkBlue);
    m_digitColors[5] = QColor(Qt::darkRed);
    m_digitColors[6] = QColor(Qt::cyan);
    m_digitColors[7] = QColor(Qt::darkYellow);
    m_digitColors[8] = QColor(Qt::darkCyan);

    m_difficultyModes[DifficultyMode::Beginner] = DifficultyParams(QSize(8, 8), 10);
    m_difficultyModes[DifficultyMode::Intermediate] = DifficultyParams(QSize(16, 16), 40);
    m_difficultyModes[DifficultyMode::Expert] = DifficultyParams(QSize(16, 31), 99);

    connect(m_ui->buttonReset, &QPushButton::clicked, this, &MainWindow::resetMineFieldRandomized);
    connect(m_ui->buttonRandomize, &QPushButton::clicked, this, &MainWindow::randomizeSeed);
    connect(m_ui->buttonSeed, &QPushButton::clicked, this, &MainWindow::resetMineField);

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::incrementClock);
    connect(m_ui->comboBoxDifficultyMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::loadDifficultyMode);

    connect(m_ui->actionHighscore, &QAction::triggered, [this]()
    {
        m_highScore.displayScore(static_cast<DifficultyMode>(m_difficultyIndex), this);
    });

    connect(m_ui->actionExit, &QAction::triggered, this, &QMainWindow::close);

    loadDifficultyMode(m_difficultyIndex);
}

MainWindow::~MainWindow() = default;

void MainWindow::incrementClock()
{
    m_ui->displayTime->display(m_ui->displayTime->intValue() + 1);
}

void MainWindow::loadDifficultyMode(int index)
{
    auto mode = static_cast<DifficultyMode>(index);
    if (mode == DifficultyMode::CustomGame)
    {
        CustomGameDialog dialog(this);
        if (!dialog.exec())
        {
            QSignalBlocker blocker(m_ui->comboBoxDifficultyMode);
            m_ui->comboBoxDifficultyMode->setCurrentIndex(m_difficultyIndex);
            return;
        }

        m_params = dialog.getParams();
    }
    else
    {
        m_params = m_difficultyModes[mode];
    }

    m_difficultyIndex = index;
    resetMineFieldRandomized();

    QTimer::singleShot(0, [this]()
    {
        adjustSize();

        auto screenGeo = QApplication::desktop()->availableGeometry();

        auto geo = geometry();
        geo.moveCenter(screenGeo.center());

        setGeometry(geo);

        setFixedSize(sizeHint());
    });
}

void MainWindow::resetMineFieldRandomized()
{
    randomizeSeed();
    resetMineField();
}

void MainWindow::resetMineField()
{
    setGameState(GameState::Running);

    m_numClicks = 0;
    m_minesPlaced = false;

    m_ui->displayMinesLeft->display(m_params.minesQuantity);
    m_ui->displayTime->display(0);

    makeMinefield();
}

void MainWindow::randomizeSeed()
{
    auto time = QTime ::currentTime();
    srand(time.msecsSinceStartOfDay());

    m_ui->spinBoxSeed->setValue(rand() % m_ui->spinBoxSeed->maximum());
}

void MainWindow::makeMinefield()
{
    m_mineFields.clear();

    deleteLayout(m_ui->widgetContent->layout());

    auto layout = new QGridLayout();
    m_ui->widgetContent->setLayout(layout);

    forEachField([this, layout](const Coordinate& coord)
    {
        auto button = new RightClickableButton();
        button->setFixedSize(QSize(48, 48));
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);

        layout->addWidget(button, coord.first, coord.second);

        MineField field;
        field.button = button;

        m_mineFields[coord] = field;

        connect(button, &QPushButton::clicked, [this, coord]()
        {
            checkMineField(coord);
        });

        connect(button, &RightClickableButton::rightClicked, [this, coord]()
        {
            toggleMineFieldFlag(coord);
        });

        connect(button, &RightClickableButton::mousePressed, [this]()
        {
            if (m_state == GameState::Running)
            {
                setButtonIcon(m_ui->buttonReset, m_iconSmileyExcited, QSize(64, 64));
            }
        });

        connect(button, &RightClickableButton::mouseReleased, [this]()
        {
            if (m_state == GameState::Running)
            {
                setButtonIcon(m_ui->buttonReset, m_iconSmileyHappy, QSize(64, 64));
            }
        });
    });
}

void MainWindow::placeMines(int quantity, const Coordinate& excludedCoord)
{
    srand(m_ui->spinBoxSeed->value());

    m_mines.clear();

    for (auto i = 0; i<quantity; i++)
    {
        auto isExistingOrExcluded = false;
        do
        {
            Coordinate coord(rand() % m_params.size.width(), rand() % m_params.size.height());
            isExistingOrExcluded = ((m_mines.count(coord) > 0) || (coord == excludedCoord));

            if (!isExistingOrExcluded)
            {
                m_mines.insert(coord);
            }
        } while (isExistingOrExcluded);
    }

    m_minesPlaced = true;
}

void MainWindow::setGameState(GameState state)
{
    m_timer.stop();

    m_state = state;
    switch (m_state)
    {
    case GameState::Running:
        setButtonIcon(m_ui->buttonReset, m_iconSmileyHappy, QSize(64, 64));
        return;
    case GameState::Succeeded:
        setButtonIcon(m_ui->buttonReset, m_iconSmileyGlasses, QSize(64, 64));        
        checkHighScore();

        break;
    case GameState::Failed:
        setButtonIcon(m_ui->buttonReset, m_iconSmileyAngry, QSize(64, 64));
        break;
    default:
        break;
    };
}

void MainWindow::checkMineField(const Coordinate& coord)
{
    if (!m_minesPlaced)
    {
        placeMines(m_params.minesQuantity, coord);
        m_timer.start();
    }

    auto& field = m_mineFields[coord];
    auto isUncovered = (field.state != ButtonState::Hidden);
    auto isUnclear = (field.state == ButtonState::Unclear);

    if ((m_state != GameState::Running) || (isUncovered && !isUnclear))
    {
        field.button->setChecked(isUncovered);
        return;
    }

    if (m_mines.count(coord) > 0)
    {
        setWidgetStyle(field.button, QColor(Qt::red));

        uncoverAll();
        return;
    }

    m_numClicks++;

    CoordinateSet visited;
    uncoverField(coord, visited);

    checkWinningCondition();
}

void MainWindow::toggleMineFieldFlag(const Coordinate& coord)
{
    if (m_state != GameState::Running)
    {
        return;
    }

    auto& field = m_mineFields[coord];
    auto minesLeft = m_ui->displayMinesLeft->intValue();

    if (field.state == ButtonState::Hidden)
    {
        m_ui->displayMinesLeft->display(minesLeft - 1);

        field.button->setChecked(true);
        field.state = ButtonState::Flagged;

        setButtonIcon(field.button, m_iconFlag);

        checkWinningCondition();
    }
    else if (field.state == ButtonState::Flagged)
    {
        m_ui->displayMinesLeft->display(minesLeft + 1);

        field.button->setIcon(QIcon());
        field.button->setText("?");

        setWidgetStyle(field.button, QColor(Qt::gray), FontWeight::Bold);

        field.state = ButtonState::Unclear;
    }
    else if (field.state == ButtonState::Unclear)
    {
        resetButtonStyleAndText(field.button);
        field.button->setChecked(false);

        field.state = ButtonState::Hidden;
    }
}

void MainWindow::uncoverField(const Coordinate& coord, CoordinateSet& visited)
{
    if (visited.count(coord) > 0)
    {
        return;
    }

    visited.insert(coord);

    auto& field = m_mineFields[coord];
    auto isFlagged = (field.state == ButtonState::Flagged);
    auto isUnclear = (field.state == ButtonState::Unclear);

    auto numAdjacentMines = countMinesAdjacentTo(coord);

    if (!isFlagged)
    {
        if (isUnclear)
        {
            resetButtonStyleAndText(field.button);
        }

        field.state = ButtonState::Uncovered;
        field.button->setChecked(true);

        if (numAdjacentMines > 0)
        {
            field.button->setText(QString::number(numAdjacentMines));
            setWidgetStyle(field.button, m_digitColors[numAdjacentMines], FontWeight::Bold);
        }
        else
        {
            forEachAdjacentField(coord, [this, &visited](const Coordinate& adjCoord)
            {
                uncoverField(adjCoord, visited);
            });
        }
    }
}

void MainWindow::uncoverAll()
{
    forEachField([this](const Coordinate& coord)
    {
        auto& field = m_mineFields[coord];

        auto isUnclear = (field.state == ButtonState::Unclear);
        auto isFlagged = (field.state == ButtonState::Flagged);

        if (m_mines.count(coord) > 0)
        {
            field.button->setChecked(true);

            if (isUnclear)
            {
                resetButtonStyleAndText(field.button);
            }

            if (!isFlagged)
            {
                if (!isUnclear)
                {
                    field.state = ButtonState::Uncovered;
                }

                setButtonIcon(field.button, m_iconMine);
            }
            else
            {
                setWidgetStyle(field.button, QColor(Qt::green));
            }
        }
        else if (isFlagged)
        {
            setButtonIcon(field.button, m_iconMineMissed);
        }
    });

    setGameState(GameState::Failed);
}

void MainWindow::checkWinningCondition()
{
    auto conditionAllUncoveredMet = true;
    auto conditionAllMinesFlaggedMet = true;

    forEachField([this, &conditionAllUncoveredMet, &conditionAllMinesFlaggedMet](const Coordinate& coord)
    {
        auto& field = m_mineFields[coord];
        auto isUncovered = (field.state == ButtonState::Uncovered);
        auto isFlagged = (field.state == ButtonState::Flagged);
        auto hasMine = (m_mines.count(coord) > 0);

        if (!hasMine && !isUncovered)
        {
            conditionAllUncoveredMet = false;
        }

        if (hasMine != isFlagged)
        {
            conditionAllMinesFlaggedMet = false;
        }
    });

    if ((!conditionAllUncoveredMet && !conditionAllMinesFlaggedMet))
    {
        return;
    }

    setGameState(GameState::Succeeded);
}

void MainWindow::forEachField(FieldFunction func)
{
    for (auto y = 0; y < m_params.size.height(); y++)
    {
        for (auto x = 0; x < m_params.size.width(); x++)
        {
            func(Coordinate(x, y));
        }
    }
}

void MainWindow::forEachAdjacentField(const Coordinate& coord, FieldFunction func)
{
    for (auto y = coord.second - 1; y < coord.second + 2; y++)
    {
        for (auto x = coord.first - 1; x < coord.first + 2; x++)
        {
            Coordinate adjCoord(x, y);

            if ((x < 0) || (y < 0) || (x >= m_params.size.width()) ||
                (y >= m_params.size.height()) || (coord == adjCoord))
            {
                continue;
            }

            func(adjCoord);
        }
    }
}

int MainWindow::countMinesAdjacentTo(const Coordinate& coord)
{
    auto numAdjacentMines = 0;
    forEachAdjacentField(coord, [this, &numAdjacentMines](const Coordinate& adjCoord)
    {
        if (m_mines.count(adjCoord) > 0)
        {
            numAdjacentMines++;
        }
    });

    return numAdjacentMines;
}

void MainWindow::checkHighScore()
{
    auto time = std::max<int>(1, m_ui->displayTime->intValue());

    auto score3bv = count3BV();
    auto score3bvPerTime = static_cast<float>(score3bv) / time;
    auto score3bvPerClicks = static_cast<float>(score3bv) / m_numClicks;
    auto totalScore = static_cast<int>((score3bvPerTime + score3bvPerClicks) * 100.0f);

    auto mode = static_cast<DifficultyMode>(m_difficultyIndex);

    if (m_highScore.hasReachedTopThree(mode, totalScore))
    {
        QString name;

        do
        {
            name = QInputDialog::getText(this, tr("Highscore"), tr("Please enter your name for the high score:"));
        }
        while (name.isEmpty());

        HighScore::ScoreData data;
        data.name = name;
        data.totalScore = totalScore;
        data.score3bv = score3bv;
        data.score3bvPerTime = score3bvPerTime;
        data.score3bvPerClicks = score3bvPerClicks;
        data.clicks = m_numClicks;
        data.time = time;
        data.mode = mode;

        m_highScore.addScoreData(mode, data);
        m_highScore.saveScore();
    }

    m_highScore.displayScore(mode, this, HighScore::HighscoreDisplayMode::HighlightLastAdded);
}

// see https://gamedev.stackexchange.com/questions/63046/how-should-i-calculate-the-score-in-minesweeper-3bv-or-3bv-s
int MainWindow::count3BV()
{
    CoordinateSet marked;
    int score = 0;

    forEachField([this, &marked, &score](const Coordinate& coord)
    {
        auto numAdjMines = countMinesAdjacentTo(coord);
        if (numAdjMines == 0)
        {
            if (marked.count(coord) > 0)
            {
                return;
            }

            marked.insert(coord);
            score++;
            floodFillMark(coord, marked);
        }
    });

    forEachField([this, &marked, &score](const Coordinate& coord)
    {
        if ((marked.count(coord) == 0) && (m_mines.count(coord) == 0))
        {
            score++;
        }
    });

    return score;
}

void MainWindow::floodFillMark(const Coordinate& coord, CoordinateSet& marked)
{
    forEachAdjacentField(coord, [this, &marked](const Coordinate& adjCoord)
    {
        if (marked.count(adjCoord) == 0)
        {
            marked.insert(adjCoord);
        }
        else
        {
            return;
        }

        if (countMinesAdjacentTo(adjCoord) == 0)
        {
            floodFillMark(adjCoord, marked);
        }
    });
}