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

#include "numberlabel.h"

#include <QApplication>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

NumberLabel::NumberLabel(QWidget* parent)
    : QLabel(parent)
{
    setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

// ---------------------------------------------------------------------------------------------- //

void NumberLabel::setPrefix(const QString& prefix)
{
    m_prefix = prefix;
}

// ---------------------------------------------------------------------------------------------- //

void NumberLabel::setSuffix(const QString& suffix)
{
    m_suffix = suffix;
}

// ---------------------------------------------------------------------------------------------- //

void NumberLabel::setPrecision(int precision)
{
    m_precision = precision;
}

// ---------------------------------------------------------------------------------------------- //

void NumberLabel::setMinimumTextWidth(const QString& text)
{
    QFontMetrics fm(QApplication::font());
    setMinimumWidth(fm.horizontalAdvance(text));
}

// ---------------------------------------------------------------------------------------------- //

void NumberLabel::setValue(int value)
{
    setText(m_prefix + QString::number(value) + m_suffix);
}

// ---------------------------------------------------------------------------------------------- //

void NumberLabel::setValue(double value)
{
    setText(m_prefix + QString::number(value, 'f', m_precision) + m_suffix);
}

// ---------------------------------------------------------------------------------------------- //
