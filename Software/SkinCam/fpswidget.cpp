// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF SkinCam project.                                                 //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2024                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This program is free software: you can redistribute it and/or modify it under the terms       //
//  of the GNU General Public License as published by the Free Software Foundation, either        //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU General Public License for more details.                                          //
//                                                                                                //
//  You should have received a copy of the GNU General Public License along with this program.    //
//  If not, see <https://www.gnu.org/licenses/>.                                                  //
//                                                                                                //
// ============================================================================================== //

#include "fpswidget.h"

#include <QHBoxLayout>
#include <QPainter>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class FpsWidget::DisplayBar : public QWidget
{
public:
    DisplayBar(QWidget* parent = nullptr);

    auto sizeHint() const -> QSize override;

    void setMaximum(int value);
    auto maximum() const -> int;

    void setValues(int processed, int displayed);
    auto processed() const -> int;
    auto displayed() const -> int;

private:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_maximum = 100;
    int m_processed = 0;
    int m_displayed = 0;
};

// ---------------------------------------------------------------------------------------------- //

FpsWidget::FpsWidget(QWidget* parent)
    : QWidget(parent),
      m_displayBar(new DisplayBar),
      m_label(new QLabel("  0 /   0"))
{
    m_label->setAlignment(Qt::AlignRight);
    m_label->setToolTip("displayed / processed");

    auto layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel("Frames per second: "));
    layout->addWidget(m_displayBar);
    layout->addWidget(m_label);
}

// ---------------------------------------------------------------------------------------------- //

void FpsWidget::setMaximum(int value)
{
    // Prevent widget from jittering
    const QFontMetrics fm = fontMetrics();
    m_label->setMinimumWidth(fm.horizontalAdvance(QString(" %1 / %1").arg(value)));

    m_displayBar->setMaximum(value);
    m_fieldWidth = QString::number(value).length();

    updateLabel();
}

// ---------------------------------------------------------------------------------------------- //

void FpsWidget::setValues(int processed, int displayed)
{
    m_displayBar->setValues(processed, displayed);
    updateLabel();
}

// ---------------------------------------------------------------------------------------------- //

void FpsWidget::updateLabel()
{
    const int processed = std::min(m_displayBar->displayed(), m_displayBar->maximum());
    const int displayed = std::min(m_displayBar->processed(), m_displayBar->maximum());

    m_label->setText(QString("%1 / %2").arg(processed, m_fieldWidth)
                                       .arg(displayed, m_fieldWidth));
}

// ---------------------------------------------------------------------------------------------- //

FpsWidget::DisplayBar::DisplayBar(QWidget* parent)
    : QWidget(parent)
{
}

// ---------------------------------------------------------------------------------------------- //

auto FpsWidget::DisplayBar::sizeHint() const -> QSize
{
    const QFontMetrics fm = fontMetrics();
    return { 20 * fm.horizontalAdvance('x'), fm.height() };
}

// ---------------------------------------------------------------------------------------------- //

inline
void FpsWidget::DisplayBar::setMaximum(int value)
{
    m_maximum = std::max(value, 0);
    update();
}

// ---------------------------------------------------------------------------------------------- //

inline
auto FpsWidget::DisplayBar::maximum() const -> int
{
    return m_maximum;
}

// ---------------------------------------------------------------------------------------------- //

inline
void FpsWidget::DisplayBar::setValues(int processed, int displayed)
{
    m_processed = std::max(processed, 0);
    m_displayed = std::max(displayed, 0);
    update();
}

// ---------------------------------------------------------------------------------------------- //

inline
auto FpsWidget::DisplayBar::processed() const -> int
{
    return m_processed;
}

// ---------------------------------------------------------------------------------------------- //

inline
auto FpsWidget::DisplayBar::displayed() const -> int
{
    return m_displayed;
}

// ---------------------------------------------------------------------------------------------- //

void FpsWidget::DisplayBar::paintEvent(QPaintEvent* event)
{
    constexpr int FramePenWidth = 1;
    constexpr int BarPenWidth = 1;

    QWidget::paintEvent(event);

    QPainter painter(this);

    const QRect widgetRect(rect());

    QRect frameRect(0, 0, widgetRect.width() - 2 * FramePenWidth, widgetRect.height() * 3 / 5);
    frameRect.moveCenter(widgetRect.center());

    QRect barRect(0, 0, frameRect.width() - 2 * BarPenWidth, frameRect.height() - 2 * BarPenWidth);
    barRect.moveCenter(frameRect.center());

    const auto drawFrame = [&]
    {
        QPen pen(Qt::gray);
        pen.setWidth(FramePenWidth);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(frameRect);
    };

    const auto drawBar = [&](int value, double alpha)
    {
        constexpr int Offset = FramePenWidth + BarPenWidth;
        constexpr double PenDim = 0.8;

        double green = std::min(2.0 * value / m_maximum, 1.0);
        double red = std::min(2.0 * (m_maximum - value) / m_maximum, 1.0);

        QPen pen(QColor::fromRgbF(PenDim * red, PenDim * green, 0.0, alpha));
        pen.setWidth(BarPenWidth);

        QBrush brush(QColor::fromRgbF(red, green, 0.0, alpha));

        QRect rect(barRect);
        rect.setWidth(value * rect.width() / m_maximum);

        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawRect(rect);
    };

    const int processed = std::min(m_processed, m_maximum);
    const int displayed = std::min(m_displayed, m_maximum);

    drawFrame();
    drawBar(processed, 0.4);
    drawBar(displayed, 0.8);
}
