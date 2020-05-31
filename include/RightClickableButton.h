#pragma once

#include <QPushButton>

class RightClickableButton : public QPushButton
{
    Q_OBJECT

public:
    RightClickableButton();
    virtual ~RightClickableButton();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

Q_SIGNALS:
    void rightClicked();
    void mousePressed();
    void mouseReleased();

};