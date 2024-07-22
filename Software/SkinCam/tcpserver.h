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

#pragma once

#include "namespace.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

SKINCAM_BEGIN_NAMESPACE();

class TcpServer : public QObject
{
    Q_OBJECT

public:
    explicit TcpServer(quint16 port);
    ~TcpServer() override;

public slots:
    void setSkinDetected(bool detected);

private slots:
    void onNewConnection();
    void onDisconnect();

private:
    void sendDetectionState();

    void sendData(QTcpSocket* socket, QString data);
    void sendError(QTcpSocket* socket, const QString& message);

    void sendData(const QString& data);
    void sendError(const QString& message);

private:
    QTcpServer m_server;
    QTcpSocket* m_socket = nullptr;

    bool m_cameraConnected = false;
    bool m_skinDetected = false;
};

SKINCAM_END_NAMESPACE();
