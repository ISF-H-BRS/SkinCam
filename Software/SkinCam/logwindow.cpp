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

#include "logwindow.h"
#include "ui_logwindow.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class LogWindow::Listener : public Logger::Listener
{
public:
    Listener(LogWindow* owner) : m_owner(owner) {}

private:
    void onLogAppended(const char* msg) override
    {
        QMetaObject::invokeMethod(m_owner, "onLogAppended",
                                  Qt::QueuedConnection, Q_ARG(QString, msg));
    }

private:
    LogWindow* m_owner;
};

// ---------------------------------------------------------------------------------------------- //

LogWindow::LogWindow(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::LogWindow>())
{
    m_ui->setupUi(this);
    m_ui->messages->insertPlainText(Logger::log().c_str());

    m_listener = std::make_unique<Listener>(this);
    Logger::addListener(m_listener.get());
}

// ---------------------------------------------------------------------------------------------- //

LogWindow::~LogWindow()
{
    Logger::removeListener(m_listener.get());
}

// ---------------------------------------------------------------------------------------------- //

void LogWindow::onLogAppended(const QString& msg)
{
    m_ui->messages->insertPlainText(msg + "\n");
    m_ui->messages->moveCursor(QTextCursor::End);
}

// ---------------------------------------------------------------------------------------------- //

void LogWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit visibilityChanged(true);
}

// ---------------------------------------------------------------------------------------------- //

void LogWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit visibilityChanged(false);
}

// ---------------------------------------------------------------------------------------------- //
