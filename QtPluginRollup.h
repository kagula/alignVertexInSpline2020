#pragma once

#include <maxapi.h>

#include <Qt/QMaxParamBlockWidget.h>
#include "ui_plugin_form.h"
#include "ISceneEventManager.h"

namespace Ui
{
	class PluginRollup;
}

class QtPluginRollup : public MaxSDK::QMaxParamBlockWidget, public INodeEventCallback
{
	// This is a macro that connects this class to the Qt build system
	Q_OBJECT

public:
	explicit QtPluginRollup(QWidget* parent = nullptr);
	virtual ~QtPluginRollup(void);

	// Required by QMaxParamBlockWidget:
	void SetParamBlock(ReferenceMaker* /*owner*/, IParamBlock2* const /*param_block*/) override {};
	void UpdateUI(const TimeValue t) override
	{
		Interface* ip = GetCOREInterface();
		ip->RedrawViews(t, REDRAW_NORMAL);
	};
	void UpdateParameterUI(const TimeValue /*t*/, const ParamID /*param_id*/, const int /*tab_index*/) override {};
	
public:
	void SelectionChanged(NodeKeyTab &nodes) override;
	void SubobjectSelectionChanged(NodeKeyTab &nodes) override;

protected:
	void hideEvent(QHideEvent *event) override;
	void showEvent(QShowEvent *event) override;

	INode* setSelectedObjectName();

	enum class OPTYPE { X, Y, Z, MinX, MinY, MinZ, MaxX, MaxY, MaxZ };
	void alignVertexInSpline(OPTYPE optype);
protected slots:
	void on_pbX_clicked();
	void on_pbY_clicked();
	void on_pbZ_clicked();
	void on_pbMinX_clicked();
	void on_pbMinY_clicked();
	void on_pbMinZ_clicked();
	void on_pbMaxX_clicked();
	void on_pbMaxY_clicked();
	void on_pbMaxZ_clicked();

private:
	Ui::PluginRollup* ui;
	SceneEventNamespace::CallbackKey mCK;
};
