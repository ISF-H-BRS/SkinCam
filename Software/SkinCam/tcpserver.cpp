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

#include "assertions.h"
#include "exception.h"
#include "logger.h"
#include "tcpserver.h"

#include <QCoreApplication>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

TcpServer::TcpServer(quint16 port)
{
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    if (!m_server.listen(QHostAddress::Any, port))
        throw Exception("Unable to start TCP server: " + m_server.errorString() + ".");

    Logger::info("TCP server started. Listening on port "
                 + std::to_string(m_server.serverPort()) + ".\n");
}

// ---------------------------------------------------------------------------------------------- //

TcpServer::~TcpServer()
{
    if (m_socket && m_socket->isOpen())
        m_socket->close();

    Logger::info("TCP server stopped.");
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::sendDetectionState()
{
    if (m_skinDetected)
        sendData("<CAMERA_ON>");
    else
        sendData("<CAMERA_OFF>");
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::setSkinDetected(bool detected)
{
    if (detected != m_skinDetected)
    {
        m_skinDetected = detected;
        sendDetectionState();
    }
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::onNewConnection()
{
    if (m_socket)
    {
        std::unique_ptr<QTcpSocket> socket(m_server.nextPendingConnection());
        sendError(socket.get(), "Another client is already connected.");
    }
    else
    {
        m_socket = m_server.nextPendingConnection();
        connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

        sendDetectionState();

        const auto peer = m_socket->peerAddress().toString().toStdString();
        Logger::info("Connection from " + peer + " established.");
    }
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::onDisconnect()
{
    ASSERT_NOT_NULL(m_socket);

    const auto peer = m_socket->peerAddress().toString().toStdString();
    Logger::info("Connection to " + peer + " closed.");

    m_socket->deleteLater();
    m_socket = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::sendData(QTcpSocket* socket, QString data)
{
    ASSERT_NOT_NULL(socket);
    ASSERT(socket->isOpen());

    data += "\r\n";

    socket->write(data.toUtf8());
    socket->waitForBytesWritten();
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::sendError(QTcpSocket* socket, const QString& message)
{
    sendData(socket, "<CAMERA_ERROR> " + message);
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::sendData(const QString& data)
{
    if (!m_socket || !m_socket->isOpen())
        return;

    sendData(m_socket, data);
}

// ---------------------------------------------------------------------------------------------- //

void TcpServer::sendError(const QString& message)
{
    if (!m_socket || !m_socket->isOpen())
        return;

    sendError(m_socket, message);
}

// ---------------------------------------------------------------------------------------------- //
