//**************************************************************************/
// Copyright (c) 1998-2020 Autodesk, Inc.
// All rights reserved.
// 
// Use of this software is subject to the terms of the Autodesk license 
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Plugin Wizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "alignVertexInSpline2022.h"

#include "QtPluginRollup.h"
#include "qmessagebox.h"
#include "qobject.h"

#include "ISceneEventManager.h"
#include "Qt/QmaxMainWindow.h" 
#include "Qt/QmaxToolBar.h"
#include "Qt/QmaxDockingWinHost.h"
#include "Qt/QmaxDockingWinHostDelegate.h"


#define alignVertexInSpline2020_CLASS_ID Class_ID(0x92afcec9, 0x94bbcca4)

class ToolbarDockingDelegate;

class alignVertexInSpline2022 : public UtilityObj, public QObject{
public:
	// Constructor/Destructor
	alignVertexInSpline2022();
	virtual ~alignVertexInSpline2022();

	void DeleteThis() override {}

	void BeginEditParams(Interface* ip, IUtil* iu) override;
	void EndEditParams(Interface* ip, IUtil* iu) override;

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);

	// Singleton access
	static alignVertexInSpline2022* GetInstance()
	{
		static alignVertexInSpline2022 thealignVertexInSpline2020;
		return &thealignVertexInSpline2020;
	}

public:
	QtPluginRollup *mQtPluginRollup = nullptr;

private:
	ToolbarDockingDelegate* mDockingDelegate = nullptr;
	MaxSDK::QmaxToolBar* mToolBar = nullptr;
};


class alignVertexInSpline2020ClassDesc : public ClassDesc2 
{
public:
	int           IsPublic() override                               { return TRUE; }
	void*         Create(BOOL /*loading = FALSE*/) override         { return alignVertexInSpline2022::GetInstance(); }
	const TCHAR*  ClassName() override                              { return GetString(IDS_CLASS_NAME); }
	const TCHAR*  NonLocalizedClassName() override                  { return _T("alignVertexInSpline2020"); }
	SClass_ID     SuperClassID() override                           { return UTILITY_CLASS_ID; }
	Class_ID      ClassID() override                                { return alignVertexInSpline2020_CLASS_ID; }
	const TCHAR*  Category() override                               { return GetString(IDS_CATEGORY); }

	const TCHAR*  InternalName() override                           { return _T("alignVertexInSpline2020"); } // Returns fixed parsable name (scripter-visible name)
	HINSTANCE     HInstance() override                              { return hInstance; } // Returns owning module handle


};

ClassDesc2* GetalignVertexInSpline2020Desc()
{
	static alignVertexInSpline2020ClassDesc alignVertexInSpline2020Desc;
	return &alignVertexInSpline2020Desc; 
}




//--- alignVertexInSpline2020 -------------------------------------------------------
alignVertexInSpline2022::alignVertexInSpline2022()
{

}

alignVertexInSpline2022::~alignVertexInSpline2022()
{

}

/*===========================================================================*\
| Class ToolbarDockingDelegate is the custom docking delegate that will
| process the controls we add.
\*===========================================================================*/

class ToolbarDockingDelegate : public MaxSDK::QmaxProxyDockingWinHostDelegate
{
public:
	ToolbarDockingDelegate(MaxSDK::QmaxDockingWinHostDelegate* baseDelegate)
		: QmaxProxyDockingWinHostDelegate(baseDelegate) {}

	bool winEvent(MSG* msg, long* result) override;

private:
	bool processMessage(UINT message, WPARAM wParam, LPARAM lParam);
};

bool ToolbarDockingDelegate::winEvent(MSG* msg, long* result)
{
	return processMessage(msg->message, msg->wParam, msg->lParam);
}

bool ToolbarDockingDelegate::processMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	alignVertexInSpline2022 *pAVIS = alignVertexInSpline2022::GetInstance();
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TB_0: // Dock or Float
			if (pAVIS->mQtPluginRollup == nullptr)
			{
				pAVIS->mQtPluginRollup = new QtPluginRollup();

				auto mainWindow = GetCOREInterface()->GetQmaxMainWindow();
				pAVIS->mQtPluginRollup->setParent(mainWindow->topLevelWidget());
				pAVIS->mQtPluginRollup->setWindowModality(Qt::WindowModality::NonModal);
				pAVIS->mQtPluginRollup->move(128, 128);
				pAVIS->mQtPluginRollup->setWindowTitle(u8"设置Spline中节点的位置");
				//Becuase after setParent，title bar will disappear，so we need setWindowFlags。
				//Reference link https://doc.qt.io/qt-5/qt.html#WindowType-enum
				pAVIS->mQtPluginRollup->setWindowFlags(Qt::Dialog);
				pAVIS->mQtPluginRollup->setWindowFlags(pAVIS->mQtPluginRollup->windowFlags() & ~Qt::WindowContextHelpButtonHint);//remove help button over tile bar.
			}

			if (pAVIS->mQtPluginRollup->isHidden())
			{
				pAVIS->mQtPluginRollup->show();
			}
			else
			{
				pAVIS->mQtPluginRollup->hide();
			}
			return true;
		default: // ID not recognized -- use default CUI processing
			return false;
		}
	}
	return false;
}

void alignVertexInSpline2022::BeginEditParams(Interface* ip,IUtil* iu)
{
	if (mToolBar) {
		// We have the toolbar already, just show it...(EndEditParams() hid it)
		mToolBar->setVisible(true);
	}
	else {
		// Create Qt toolbar
		auto mainWindow = GetCOREInterface()->GetQmaxMainWindow();
		mToolBar = new MaxSDK::QmaxToolBar("Align Vertex In Spline", "Align Vertex In Spline", mainWindow);

		// Create docking win host for hosting the legacy CustToolbar
		MaxSDK::QmaxDockingWinHost* winHost = new MaxSDK::QmaxDockingWinHost(mToolBar);
		mToolBar->setWidget(winHost);

		// -- Now create the toolbar window
		HWND hToolbar = CreateWindow(
			CUSTTOOLBARWINDOWCLASS,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0, 0, 250, 100,
			reinterpret_cast<HWND>(winHost->winId()),
			NULL,
			hInstance,
			NULL);

		// -- Now link the toolbar to the CUI frame
		ICustToolbar *iToolBar = GetICustToolbar(hToolbar);

		// Get the 0th icon from the CUITest icon file.
		// Note that the index is 1-based.
		//copy C:\Program Files\Autodesk\3ds Max 2022 SDK\maxsdk\howto\ui\cuitest\*.bmp to C:\Program Files\Autodesk\3ds Max 2022\UI_ln\IconsDark and C:\Program Files\Autodesk\3ds Max 2022\UI_ln\Icons
		//we reuse cuitest sample's bmp files for our icon.
		MaxBmpFileIcon* pIcon = new MaxBmpFileIcon(_T("CUITest"), 0 + 1);
		// -- Toss in a few controls of various sorts...

		// Add a push button
		// This one docks and undocks the toolbar if clicked...
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, pIcon, ID_TB_0));

		//Create a custom docking delegate which wraps the default docking delegate.
		//This custom delegate will process the controls we added.
		mDockingDelegate = new ToolbarDockingDelegate(iToolBar->GetDockingDelegate());
		winHost->setDockingDelegate(mDockingDelegate);
		winHost->setHostedWindow(hToolbar);

		// need to be added once to the mainwindow toolbar layout before we can float it
		mainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mToolBar);
		MaxSDK::QmaxMainWindow::makeToolBarFloating(mToolBar, QCursor::pos());

		// We are done, release the toolbar
		ReleaseICustToolbar(iToolBar);
	}
}

void alignVertexInSpline2022::EndEditParams(Interface* ip, IUtil*)
{
}

void alignVertexInSpline2022::Init(HWND /*handle*/)
{

}

void alignVertexInSpline2022::Destroy(HWND /*handle*/)
{

}
