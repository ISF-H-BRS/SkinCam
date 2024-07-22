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

#include "cameraexception.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

CameraException::CameraException(const std::string& what, int error)
    : Exception(what + " The API call returned the error code " + std::to_string(error) + ".") {}

// ---------------------------------------------------------------------------------------------- //

CameraException::CameraException(const std::string& what, const std::string& serial, int error)
    : Exception(what + " The API call for the camera with the serial number " + serial
                     + " returned the error code " + std::to_string(error) + ".") {}

// ---------------------------------------------------------------------------------------------- //
