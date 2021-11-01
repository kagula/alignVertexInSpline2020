#include "QtPluginRollup.h"

#include "ISceneEventManager.h"

#include "inode.h"
#include "object.h"

#include "spline3d.h"
#include "splshape.h"

#include <set>
#include <assert.h>

QtPluginRollup::QtPluginRollup(QWidget* /*parent*/)
	: QMaxParamBlockWidget()
	, ui(new Ui::PluginRollup())
{
	// At a minimum, we must call setupUi(). Other initialization can happen here.
	ui->setupUi(this);

	ui->label->setMaximumHeight(32);
	ui->leInput->setText("0");
}

void QtPluginRollup::alignVertexInSpline(OPTYPE optype)
{
	INode* pNode = setSelectedObjectName();
	if (pNode == nullptr)
	{
		return;
	}

	Object *pObj = pNode->GetObjectRef();
	assert(pObj != nullptr);

	DWORD subSelLevel = pObj->GetSubselState();//VERTEX = 8, SEGMENT = 4, and SPLINE = 2.
	if (subSelLevel != 8)
	{
		return;
	}

	SplineShape *pSS = (SplineShape *)(pObj);

	assert(pSS != nullptr);
	assert(pSS->shape.splineCount == 1);
	assert(pSS->GetSelSet().Count() == 0);
	assert(pSS->GetSplineSel().IsEmpty());
	assert(pSS->GetVertSel().IsEmpty());
	assert(pSS->GetSelLevel() == SS_VERTEX);
	assert(pSS->shape.selLevel == SHAPE_VERTEX);
	assert(pSS->GetSegmentSel().IsEmpty());


	BitArray *pBA = pSS->shape.vertSel.sel;
	std::set<int> setSelectedVertexIndex;
	for (size_t i = 0; i < pBA->GetSize(); i++)
	{
		if ((*pBA)[i])
		{
			setSelectedVertexIndex.insert(i / 3);
		}
	}

	Spline3D *pS = pSS->shape.splines[0];
	assert(pS != nullptr);
	assert(pS->KnotCount() == pSS->shape.vertSel.sel->GetSize() / 3);

	int nKC = pS->KnotCount();

	TimeValue t = GetCOREInterface()->GetTime();


	float scaleFactorFloat = []()->float
	{
		int unitType;
		float unitScale;
		GetSystemUnitInfo(&unitType, &unitScale);
		float masterScale = GetSystemUnitScale(unitType);
		float scaleFactorFloat = 1000.0f;

		switch (unitType)
		{
		case UNITS_INCHES:
			scaleFactorFloat *= 0.0254f * masterScale;
			break;
		case UNITS_FEET:
			scaleFactorFloat *= 0.3048f * masterScale;
			break;
		case UNITS_MILES:
			scaleFactorFloat *= 1609.34f * masterScale;
			break;
		case UNITS_MILLIMETERS:
			scaleFactorFloat *= 0.001f * masterScale;
			break;
		case UNITS_CENTIMETERS:
			scaleFactorFloat *= 0.01f * masterScale;
			break;
		case UNITS_METERS:
			scaleFactorFloat *= 1 * masterScale;
			break;
		case UNITS_KILOMETERS:
			scaleFactorFloat *= 1000 * masterScale;
			break;
		default:
			scaleFactorFloat *= 1 * masterScale;
			break;
		}
		return scaleFactorFloat;
	}();

	QString qsValue = ui->leInput->text();
	const float newValue = [scaleFactorFloat, optype, nKC, setSelectedVertexIndex, pS, pNode, t](float userValue)->float	 {
		float newValue = .0f;
		if ((optype == OPTYPE::X) || (optype == OPTYPE::Y) || (optype == OPTYPE::Z))
		{
			newValue = userValue / scaleFactorFloat;
		}
		else
		{
			for (size_t i = 0, index = 0; i < nKC; i++)
			{
				if (setSelectedVertexIndex.find(i) != setSelectedVertexIndex.end())
				{
					Point3 ptKnot = pS->GetKnotPoint(i);
					Matrix3 maxT = pNode->GetObjectTM(t);
					ptKnot = maxT.PointTransform(ptKnot);

					switch (optype)
					{
					case QtPluginRollup::OPTYPE::MinX:
						if (newValue > ptKnot.x)
						{
							newValue = ptKnot.x;
						}
						break;
					case QtPluginRollup::OPTYPE::MinY:
						if (newValue > ptKnot.y)
						{
							newValue = ptKnot.y;
						}
						break;
					case QtPluginRollup::OPTYPE::MinZ:
						if (newValue > ptKnot.z)
						{
							newValue = ptKnot.z;
						}
						break;
					case QtPluginRollup::OPTYPE::MaxX:
						if (newValue < ptKnot.x)
						{
							newValue = ptKnot.x;
						}
						break;
					case QtPluginRollup::OPTYPE::MaxY:
						if (newValue < ptKnot.y)
						{
							newValue = ptKnot.y;
						}
						break;
					case QtPluginRollup::OPTYPE::MaxZ:
						if (newValue < ptKnot.z)
						{
							newValue = ptKnot.z;
						}
						break;
					default:
						assert(NULL);
						break;
					}
				}
			}
		}

		return newValue;
	}(qsValue.toFloat());

	for (size_t i = 0, index = 0; i < nKC; i++)
	{
		if (setSelectedVertexIndex.find(i) != setSelectedVertexIndex.end())
		{
			Point3 ptKnot = pS->GetKnotPoint(i);
			Matrix3 maxT = pNode->GetObjectTM(t);
			ptKnot = maxT.PointTransform(ptKnot);

			{
				if ((optype == OPTYPE::X) || (optype == OPTYPE::MinX) || (optype == OPTYPE::MaxX))
				{
					ptKnot.x = newValue;
				}
				else if ((optype == OPTYPE::Y) || (optype == OPTYPE::MinY) || (optype == OPTYPE::MaxY))
				{
					ptKnot.y = newValue;
				}
				else
				{
					ptKnot.z = newValue;
				}

				maxT.Invert();
				ptKnot = maxT.PointTransform(ptKnot);

				pS->SetKnotPoint(i, ptKnot);
			}
		}
	}

	//update display
	static wchar_t bufDisplay[128] = { 0 };
	wsprintf(bufDisplay, L"%s, 当前有%d个顶点被选中", pNode->GetName(), setSelectedVertexIndex.size());
	QString qsDisp = QString::fromWCharArray(bufDisplay);
	ui->label->setText(qsDisp);

	//update shapes
	pSS->GetShape().InvalidateGeomCache();
	pNode->NotifyDependents(FOREVER, TOPO_CHANNEL, REFMSG_CHANGE);
	Interface* ip = GetCOREInterface();
	ip->RedrawViews(ip->GetTime());
}

void QtPluginRollup::QtPluginRollup::on_pbX_clicked()
{
	alignVertexInSpline(OPTYPE::X);
}
void QtPluginRollup::QtPluginRollup::on_pbY_clicked()
{
	alignVertexInSpline(OPTYPE::Y);
}
void QtPluginRollup::QtPluginRollup::on_pbZ_clicked()
{
	alignVertexInSpline(OPTYPE::Z);
}
void QtPluginRollup::QtPluginRollup::on_pbMinX_clicked()
{
	alignVertexInSpline(OPTYPE::MinX);
}
void QtPluginRollup::QtPluginRollup::on_pbMinY_clicked()
{
	alignVertexInSpline(OPTYPE::MinY);
}
void QtPluginRollup::QtPluginRollup::on_pbMinZ_clicked()
{
	alignVertexInSpline(OPTYPE::MinZ);
}
void QtPluginRollup::QtPluginRollup::on_pbMaxX_clicked()
{
	alignVertexInSpline(OPTYPE::MaxX);
}
void QtPluginRollup::QtPluginRollup::on_pbMaxY_clicked()
{
	alignVertexInSpline(OPTYPE::MaxY);
}
void QtPluginRollup::QtPluginRollup::on_pbMaxZ_clicked()
{
	alignVertexInSpline(OPTYPE::MaxZ);
}


INode* QtPluginRollup::setSelectedObjectName()
{
	INode *pNode = GetCOREInterface()->GetSelNode(0);
	if (pNode == nullptr)
	{
		ui->label->setText(u8"未選擇任何對象");
		return nullptr;
	}

	//
	if (GetCOREInterface()->GetSelNodeCount() > 1)
	{
		ui->label->setText(u8"選中了多個對象");
		return nullptr;
	}

	//
	Object *pObj = pNode->GetObjectRef();

	if (pObj->SuperClassID() == SHAPE_CLASS_ID) {
		if (pObj->ClassID() != Class_ID(SPLINE3D_CLASS_ID, 0)) {
			ui->label->setText(u8"選中的不是Spline類型對象");
			return nullptr;
		}
	}
	else
	{
		ui->label->setText(u8"選中的不是Shaple類型對象");
		return nullptr;
	}

	MSTR sName(pNode->GetName());
	ui->label->setText(sName);

	return pNode;
}

void QtPluginRollup::SelectionChanged(NodeKeyTab &nodes)
{
	ui->label->setText(u8"还没有选中任何对象");
	ui->leInput->setFocus();//For user convenience, we should keep the control in focus status.

	setSelectedObjectName();
}

void QtPluginRollup::SubobjectSelectionChanged(NodeKeyTab &nodes)
{
	INode* pNode = setSelectedObjectName();

	if (pNode == nullptr)
	{
		return;
	}

	Object *pObj = pNode->GetObjectRef();
	assert(pObj != nullptr);

	DWORD subSelLevel = pObj->GetSubselState();//VERTEX = 8, SEGMENT = 4, and SPLINE = 2.
	if (subSelLevel != 8)
	{
		return;
	}

	SplineShape *pSS = (SplineShape *)(pObj);

	assert(pSS != nullptr);
	assert(pSS->shape.splineCount == 1);
	assert(pSS->GetSelSet().Count() == 0);
	assert(pSS->GetSplineSel().IsEmpty());
	assert(pSS->GetVertSel().IsEmpty());
	assert(pSS->GetSelLevel() == SS_VERTEX);
	assert(pSS->shape.selLevel == SHAPE_VERTEX);
	assert(pSS->GetSegmentSel().IsEmpty());


	BitArray *pBA = pSS->shape.vertSel.sel;
	int nCountSelectedVertice = 0;
	for (size_t i = 0; i < pBA->GetSize(); i++)
	{
		if ((*pBA)[i])
		{
			nCountSelectedVertice++;
		}
	}

	static wchar_t bufDisplay[128] = { 0 };
	wsprintf(bufDisplay, L"%s, 当前有%d个顶点被选中", pNode->GetName(), nCountSelectedVertice);
	QString qsDisp = QString::fromWCharArray(bufDisplay);
	ui->label->setText(qsDisp);
}

void QtPluginRollup::hideEvent(QHideEvent *event)
{
	GetCOREInterface()->UnRegisterDlgWnd((HWND)winId());//let 3dsmax take our window handle

	ISceneEventManager *isem = GetISceneEventManager();
	isem->UnRegisterCallback(mCK);
}

void QtPluginRollup::showEvent(QShowEvent *event)
{
	GetCOREInterface()->RegisterDlgWnd((HWND)winId());//take our window handle back from 3dsmax

	ui->leInput->setFocus();

	ISceneEventManager *isem = GetISceneEventManager();
	mCK = isem->RegisterCallback(this);
}

QtPluginRollup::~QtPluginRollup()
{
	delete ui;
	ui = nullptr;
}
