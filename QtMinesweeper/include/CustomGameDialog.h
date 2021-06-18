#pragma once

#include <QDialog>

#include <memory>

namespace Ui
{
	class CustomGameDialog;
}

struct DifficultyParams
{
    DifficultyParams();
    DifficultyParams(const QSize& s, int q);

    QSize size;
    int minesQuantity = 10;
};

class CustomGameDialog : public QDialog
{
	Q_OBJECT

public:
	CustomGameDialog(QWidget* parent = nullptr);
	virtual ~CustomGameDialog();

	DifficultyParams getParams() const;

private Q_SLOTS:
	void balanceMinesQuantity(int quantity);

private:
	std::unique_ptr<Ui::CustomGameDialog> m_ui;

};