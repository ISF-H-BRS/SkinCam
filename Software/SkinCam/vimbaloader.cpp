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
#include "vimbaloader.h"

#include <VimbaCPP/Include/VimbaCPP.h>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class CameraObserver : public AVT::VmbAPI::ICameraListObserver
{
private:
    void CameraListChanged(AVT::VmbAPI::CameraPtr, AVT::VmbAPI::UpdateTriggerType) override {}
};

// ---------------------------------------------------------------------------------------------- //

VimbaLoader* VimbaLoader::s_instance = nullptr;

// ---------------------------------------------------------------------------------------------- //

VimbaLoader::VimbaLoader()
{
    ASSERT_NULL(s_instance);

    auto& system = AVT::VmbAPI::VimbaSystem::GetInstance();

    VmbErrorType result = system.Startup();

    if (result != VmbErrorSuccess)
    {
        throw CameraException("Unable to start up Vimba camera API. Please make sure the "
                              "GigE transport layer drivers are installed correctly.", result);
    }

    auto observer = AVT::VmbAPI::shared_ptr<CameraObserver>(new CameraObserver);
    result = system.RegisterCameraListObserver(observer);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to register camera-list observer.", result);

    s_instance = this;
}

// ---------------------------------------------------------------------------------------------- //

VimbaLoader::~VimbaLoader()
{
    ASSERT(s_instance == this);

    AVT::VmbAPI::VimbaSystem::GetInstance().Shutdown();
    s_instance = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

auto VimbaLoader::isInitialized() -> bool
{
    return s_instance != nullptr;
}

// ---------------------------------------------------------------------------------------------- //
