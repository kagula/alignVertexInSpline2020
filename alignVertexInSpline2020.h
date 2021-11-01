#pragma once

//**************************************************************************/
// Copyright (c) 1998-2018 Autodesk, Inc.
// All rights reserved.
// 
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
//SIMPLE TYPE


#include <utilapi.h>

#include <max.h>

#include "GetCOREInterface.h"
#include "MaxIcon.h"

extern TCHAR* GetString(int id);

extern HINSTANCE hInstance;

// Here a large value is used so it won't conflict with the IDs used
// by MAX.
#define ID_TB_0 10000

#define TBITEM(type, pIcon, cmd) \
		ToolButtonItem(type,pIcon,GetCUIFrameMgr()->GetImageSize(),GetCUIFrameMgr()->GetImageSize(),GetCUIFrameMgr()->GetButtonWidth(),GetCUIFrameMgr()->GetButtonHeight(),cmd,0)