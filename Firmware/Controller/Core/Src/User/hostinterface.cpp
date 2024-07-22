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

#include "assert.h"
#include "hostinterface.h"

#include "usbd_desc.h"

#include <cstring>

// ---------------------------------------------------------------------------------------------- //

HostInterface::HostInterface(Owner* owner)
    : m_owner(owner)
{
    ASSERT(m_owner != nullptr);

    m_classHandle.InitComplete = &HostInterface::onInitComplete;
    m_classHandle.ControlInReceived = &HostInterface::onControlInReceived;
    m_classHandle.ControlOutReceived = &HostInterface::onControlOutReceived;
    m_classHandle.ControlRxReady = &HostInterface::onControlRxReady;
    m_classHandle.BulkInComplete = &HostInterface::onBulkInComplete;
    m_classHandle.BulkOutReceived = &HostInterface::onBulkOutReceived;
    m_classHandle.pUserData = this;

    USBD_Init(&m_deviceHandle, &FS_Desc, DEVICE_FS);

    USBD_RegisterClass(&m_deviceHandle, &USBD_VENDOR_ClassDriver);
    USBD_VENDOR_RegisterHandle(&m_deviceHandle, &m_classHandle);
    USBD_Start(&m_deviceHandle);
}

// ---------------------------------------------------------------------------------------------- //

HostInterface::~HostInterface()
{
    USBD_Stop(&m_deviceHandle);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::prepareBulkOutData(uint8_t* data, uint16_t length)
{
    auto status = USBD_VENDOR_BulkPrepareRx(&m_deviceHandle, data, length);

    ASSERT(status == USBD_OK);
    UNUSED(status);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::sendBulkInData(const uint8_t* data, uint16_t length)
{
    auto ptr = const_cast<uint8_t*>(data);
    auto status = USBD_VENDOR_BulkSendData(&m_deviceHandle, ptr, length);

    ASSERT(status == USBD_OK);
    UNUSED(status);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::sendBulkInDataWait(const uint8_t* data, uint16_t length)
{
    m_bulkInWaiting = true;

    auto ptr = const_cast<uint8_t*>(data);
    auto status = USBD_VENDOR_BulkSendData(&m_deviceHandle, ptr, length);

    ASSERT(status == USBD_OK);
    UNUSED(status);

    while (m_bulkInWaiting)
        continue;
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::onInitComplete(void* pUserData)
{
    ASSERT(pUserData != nullptr);

    auto instance = static_cast<HostInterface*>(pUserData);
    instance->onInitComplete();
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onInitComplete()
{
    m_owner->onInitComplete();
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::onControlInReceived(uint8_t request, uint16_t length, void* pUserData)
{
    ASSERT(pUserData != nullptr);

    auto instance = static_cast<HostInterface*>(pUserData);
    instance->onControlInReceived(request, length);
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onControlInReceived(uint8_t request, uint16_t length)
{
    m_owner->onControlInReceived(request, m_buffer.data(), length);

    auto status = USBD_CtlSendData(&m_deviceHandle, m_buffer.data(), length);

    ASSERT(status == USBD_OK);
    UNUSED(status);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::onControlOutReceived(uint8_t request, uint16_t length, void *pUserData)
{
    ASSERT(pUserData != nullptr);

    auto instance = static_cast<HostInterface*>(pUserData);
    instance->onControlOutReceived(request, length);
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onControlOutReceived(uint8_t request, uint16_t length)
{
    m_request = request;
    m_length = length;

    if (length == 0)
        onControlRxReady();
    else if (length <= m_buffer.size())
    {
        auto status = USBD_CtlPrepareRx(&m_deviceHandle, m_buffer.data(), length);

        ASSERT(status == USBD_OK);
        UNUSED(status);
    }
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::onControlRxReady(void* pUserData)
{
    ASSERT(pUserData != nullptr);

    auto instance = static_cast<HostInterface*>(pUserData);
    instance->onControlRxReady();
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onControlRxReady()
{
    m_owner->onControlOutReceived(m_request, m_buffer.data(), m_length);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::onBulkInComplete(void* pUserData)
{
    ASSERT(pUserData != nullptr);

    auto instance = static_cast<HostInterface*>(pUserData);
    instance->onBulkInComplete();
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onBulkInComplete()
{
    if (m_bulkInWaiting)
        m_bulkInWaiting = false;
    else
        m_owner->onBulkInComplete();
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::onBulkOutReceived(uint16_t length, void* pUserData)
{
    ASSERT(pUserData != nullptr);

    auto instance = static_cast<HostInterface*>(pUserData);
    instance->onBulkOutReceived(length);
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onBulkOutReceived(uint16_t length)
{
    m_owner->onBulkOutReceived(length);
}

// ---------------------------------------------------------------------------------------------- //
