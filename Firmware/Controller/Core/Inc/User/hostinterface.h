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

#include "usbd_vendor.h"

#include <array>

class HostInterface
{
public:
    class Owner
    {
        friend class HostInterface;

        virtual void onInitComplete() {}

        virtual void onControlOutReceived(uint8_t request, const uint8_t* data, uint16_t length) {}
        virtual void onControlInReceived(uint8_t request, uint8_t* data, uint16_t length) {}

        virtual void onBulkOutReceived(uint16_t transferred) {}
        virtual void onBulkInComplete() {}
    };

public:
    HostInterface(Owner* owner);
    ~HostInterface();

    void prepareBulkOutData(uint8_t* data, uint16_t length);

    void sendBulkInData(const uint8_t* data, uint16_t length);
    void sendBulkInDataWait(const uint8_t* data, uint16_t length);

private:
    static void onInitComplete(void*);
    void onInitComplete();

    static void onControlInReceived(uint8_t, uint16_t, void*);
    void onControlInReceived(uint8_t request, uint16_t length);

    static void onControlOutReceived(uint8_t, uint16_t, void*);
    void onControlOutReceived(uint8_t request, uint16_t length);

    static void onControlRxReady(void*);
    void onControlRxReady();

    static void onBulkInComplete(void*);
    void onBulkInComplete();

    static void onBulkOutReceived(uint16_t, void*);
    void onBulkOutReceived(uint16_t length);

private:
    Owner* m_owner;

    USBD_HandleTypeDef m_deviceHandle = {};
    USBD_VENDOR_HandleTypeDef m_classHandle = {};

    std::array<uint8_t, 128> m_buffer = {};

    uint8_t m_request = 0;
    uint8_t m_length = 0;

    volatile bool m_bulkInWaiting = false;
};
