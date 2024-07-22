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

#include "usbd_ctlreq.h"
#include "usbd_desc.h"
#include "usbd_vendor.h"

#define VENDOR_EP_BULK_OUT_ADDR  0x01
#define VENDOR_EP_BULK_IN_ADDR   0x81

#define VENDOR_EP_BULK_SIZE  64

#define USB_VENDOR_CONFIG_DESC_SIZE  32 /* 2*9 + 2*7 */

static uint8_t  USBD_VENDOR_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_VENDOR_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_VENDOR_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  USBD_VENDOR_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t  USBD_VENDOR_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_VENDOR_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t* USBD_VENDOR_GetCfgDesc(uint16_t *length);
static uint8_t* USBD_VENDOR_GetDeviceQualifierDesc(uint16_t *length);

USBD_ClassTypeDef USBD_VENDOR_ClassDriver =
{
    USBD_VENDOR_Init,
    USBD_VENDOR_DeInit,
    USBD_VENDOR_Setup,
    NULL,
    USBD_VENDOR_EP0_RxReady,
    USBD_VENDOR_DataIn,
    USBD_VENDOR_DataOut,
    NULL,
    NULL,
    NULL,
    USBD_VENDOR_GetCfgDesc,
    USBD_VENDOR_GetCfgDesc,
    USBD_VENDOR_GetCfgDesc,
    USBD_VENDOR_GetDeviceQualifierDesc,
};


/* USB VENDOR device Configuration Descriptor */
#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif
static uint8_t USBD_VENDOR_CfgDesc[USB_VENDOR_CONFIG_DESC_SIZE] =
{
    0x09,                                     /* bLength: Configuation Descriptor size */
    USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,  /* bDescriptorType: Configuration */
    USB_VENDOR_CONFIG_DESC_SIZE,              /* wTotalLength: Bytes returned */
    0x00,
    0x01,                                     /* bNumInterfaces: 1 interface */
    0x01,                                     /* bConfigurationValue: Configuration value */
    0x02,                                     /* iConfiguration: Index of string descriptor describing the configuration */
    0x80,                                     /* bmAttributes: Bus powered, doesn't support Remote Wakeup */
    0xfa,                                     /* MaxPower 500 mA: this current is used for detecting Vbus */
    /* 9 */

    /* Descriptor of VENDOR interface 0 Alternate setting 0 */
    0x09,                         /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,      /* bDescriptorType: Interface descriptor type */
    0x00,                         /* bInterfaceNumber: Number of Interface */
    0x00,                         /* bAlternateSetting: Alternate setting */
    0x02,                         /* bNumEndpoints */
    0xff,                         /* bInterfaceClass: VENDOR */
    0xff,                         /* bInterfaceSubClass : VENDOR */
    0xff,                         /* nInterfaceProtocol : VENDOR */
    0,                            /* iInterface: Index of string descriptor */
    /* 18 */

    /* Endpoint OUT Descriptor */
    0x07,                             /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT,           /* bDescriptorType: Endpoint */
    VENDOR_EP_BULK_OUT_ADDR,          /* bEndpointAddress */
    0x02,                             /* bmAttributes: Bulk */
    LOBYTE(VENDOR_EP_BULK_SIZE),      /* wMaxPacketSize: */
    HIBYTE(VENDOR_EP_BULK_SIZE),
    0x00,                             /* bInterval: ignore for Bulk transfer */
    /* 25 */

    /* Endpoint IN Descriptor */
    0x07,                             /* bLength: Endpoint Descriptor size */
    USB_DESC_TYPE_ENDPOINT,           /* bDescriptorType: Endpoint */
    VENDOR_EP_BULK_IN_ADDR,           /* bEndpointAddress */
    0x02,                             /* bmAttributes: Bulk */
    LOBYTE(VENDOR_EP_BULK_SIZE),      /* wMaxPacketSize: */
    HIBYTE(VENDOR_EP_BULK_SIZE),
    0x00,                             /* bInterval: ignore for Bulk transfer */
    /* 32 */
};


/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif
static uint8_t USBD_VENDOR_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};


static uint8_t USBD_VENDOR_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    USBD_LL_OpenEP(pdev,
                   VENDOR_EP_BULK_IN_ADDR,
                   USBD_EP_TYPE_BULK,
                   VENDOR_EP_BULK_SIZE);

    USBD_LL_OpenEP(pdev,
                   VENDOR_EP_BULK_OUT_ADDR,
                   USBD_EP_TYPE_BULK,
                   VENDOR_EP_BULK_SIZE);

    USBD_VENDOR_HandleTypeDef *handle = (USBD_VENDOR_HandleTypeDef*)pdev->pUserData;

    if (handle)
    {
        if (handle->InitComplete)
            handle->InitComplete(handle->pUserData);
    }

    return USBD_OK;
}


static uint8_t USBD_VENDOR_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    USBD_LL_CloseEP(pdev, VENDOR_EP_BULK_IN_ADDR);
    USBD_LL_CloseEP(pdev, VENDOR_EP_BULK_OUT_ADDR);

    return USBD_OK;
}


static uint8_t USBD_VENDOR_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    static uint8_t ifalt = 0;

    uint8_t type = req->bmRequest & USB_REQ_TYPE_MASK;

    if (type == USB_REQ_TYPE_CLASS)
    {
        USBD_VENDOR_HandleTypeDef *handle = (USBD_VENDOR_HandleTypeDef*)pdev->pUserData;

        if (handle)
        {
            if (req->bmRequest & 0x80)
            {
                if (handle->ControlInReceived)
                    handle->ControlInReceived(req->bRequest, req->wLength, handle->pUserData);
            }
            else
            {
                if (handle->ControlOutReceived)
                    handle->ControlOutReceived(req->bRequest, req->wLength, handle->pUserData);
            }
        }
    }
    else if (type == USB_REQ_TYPE_STANDARD)
    {
        if (req->bRequest == USB_REQ_GET_INTERFACE)
            USBD_CtlSendData(pdev, &ifalt, 1);
    }

    return USBD_OK;
}


static uint8_t USBD_VENDOR_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
    USBD_VENDOR_HandleTypeDef *handle = (USBD_VENDOR_HandleTypeDef*)pdev->pUserData;

    if (handle && handle->ControlRxReady)
        handle->ControlRxReady(handle->pUserData);

    return USBD_OK;
}


static uint8_t USBD_VENDOR_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    USBD_VENDOR_HandleTypeDef *handle = (USBD_VENDOR_HandleTypeDef*)pdev->pUserData;

    if (handle && handle->BulkInComplete)
        handle->BulkInComplete(handle->pUserData);

    return USBD_OK;
}


static uint8_t USBD_VENDOR_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    USBD_VENDOR_HandleTypeDef *handle = (USBD_VENDOR_HandleTypeDef*)pdev->pUserData;

    if (handle && handle->BulkOutReceived)
        handle->BulkOutReceived(USBD_LL_GetRxDataSize(pdev, epnum), handle->pUserData);

    return USBD_OK;
}


static uint8_t *USBD_VENDOR_GetCfgDesc(uint16_t *length)
{
    *length = sizeof(USBD_VENDOR_CfgDesc);
    return USBD_VENDOR_CfgDesc;
}


uint8_t* USBD_VENDOR_GetDeviceQualifierDesc(uint16_t *length)
{
    *length = sizeof(USBD_VENDOR_DeviceQualifierDesc);
    return USBD_VENDOR_DeviceQualifierDesc;
}


uint8_t USBD_VENDOR_RegisterHandle(USBD_HandleTypeDef *pdev, USBD_VENDOR_HandleTypeDef *handle)
{
    if (handle)
    {
        pdev->pUserData = handle;
        return USBD_OK;
    }

    return USBD_FAIL;
}


uint8_t USBD_VENDOR_BulkSendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t length)
{
    USBD_LL_FlushEP(pdev, VENDOR_EP_BULK_IN_ADDR);
    return USBD_LL_Transmit(pdev, VENDOR_EP_BULK_IN_ADDR, data, length);
}


uint8_t USBD_VENDOR_BulkPrepareRx(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t length)
{
    return USBD_LL_PrepareReceive(pdev, VENDOR_EP_BULK_OUT_ADDR, data, length);
}
