#include "RightClickableButton.h"

#include <QMouseEvent>

bool isLeftOrRightButton(QMouseEvent* event)
{
    return ((event->button() == Qt::LeftButton) || (event->button() == Qt::RightButton));
}

RightClickableButton::RightClickableButton()
    : QPushButton()
{
}

RightClickableButton::~RightClickableButton() = default;

void RightClickableButton::mousePressEvent(QMouseEvent* event)
{
    if (isLeftOrRightButton(event))
    {
        Q_EMIT mousePressed();
    }

    QPushButton::mousePressEvent(event);
}

void RightClickableButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (isLeftOrRightButton(event))
    {
        Q_EMIT mouseReleased();
    }

    if (event->button() == Qt::RightButton)
    {
        Q_EMIT rightClicked();
    }

    QPushButton::mouseReleaseEvent(event);
}