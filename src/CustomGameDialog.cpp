#include "CustomGameDialog.h"

#include "ui_CustomGameDialog.h"

DifficultyParams::DifficultyParams() = default;

DifficultyParams::DifficultyParams(const QSize& s, int q)
    : size(s)
    , minesQuantity(q)
{
}

CustomGameDialog::CustomGameDialog(QWidget* parent)
	: QDialog(parent)
	, m_ui(new Ui::CustomGameDialog())
{
	m_ui->setupUi(this);

	connect(m_ui->spinBoxMines, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &CustomGameDialog::balanceMinesQuantity);
}

CustomGameDialog::~CustomGameDialog() = default;

DifficultyParams CustomGameDialog::getParams() const
{
	return DifficultyParams(
		QSize(m_ui->spinBoxHeight->value(), m_ui->spinBoxWidth->value()),
		m_ui->spinBoxMines->value());
}

void CustomGameDialog::balanceMinesQuantity(int quantity)
{
    const auto width = m_ui->spinBoxWidth->value();
    const auto height = m_ui->spinBoxHeight->value();

    const auto maxMines = (width * height) / 2;
    const auto mines = std::min<int>(quantity, maxMines);

    QSignalBlocker blocker(m_ui->spinBoxMines);
    m_ui->spinBoxMines->setValue(mines);
}