#include "StdAfx.h"
#include "DuiListCtrl.h"

#define	SCROLL_V	1	// �������ؼ�ID
#define	LISTBK_AREA	2	// ����Area�ؼ�ID

CDuiGridCtrl::CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject)
			: CDuiPanel(hWnd, pDuiObject)
{
	/*CRect rcScroll = CRect(0,0,0,0);
	rcScroll.top;
	rcScroll.left = rcScroll.right - 8;

 	CControlBase * pControlBase = NULL;
 	pControlBase = new CScrollV(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(pControlBase);
	m_pControScrollV = (CControlBaseFont*)pControlBase;*/

	CRect rcBk = CRect(0,0,0,0);
	CControlBase* pControlBase = new CArea(hWnd, this, LISTBK_AREA, rcBk, 100, 100);
 	m_vecControl.push_back(pControlBase);
	m_pControBkArea = (CControlBase*)pControlBase;

	m_strFontTitle = DuiSystem::GetDefaultFont();
	m_nFontTitleWidth = 12;
	m_fontTitleStyle = FontStyleRegular;

	m_clrText = Color(225, 64, 64, 64);
	m_clrTextHover = Color(128, 0, 0);
	m_clrTextDown = Color(0, 112, 235);
	m_clrTitle = Color(255, 32, 32, 32);
	m_clrSeperator = Color(200, 160, 160, 160);
	m_nRowHeight = 50;

	m_pImageSeperator = NULL;
	m_sizeSeperator = CSize(0, 0);
	m_pImageCheckBox = NULL;
	m_sizeCheckBox = CSize(0, 0);

	m_nBkTransparent = 30;

	m_nHoverRow = 0;
	m_nDownRow = 0;
	m_bSingleLine = TRUE;
	m_bTextWrap = FALSE;

	m_nFirstViewRow = 0;
	m_nLastViewRow = 0;
	m_nVirtualTop = 0;
}

CDuiGridCtrl::~CDuiGridCtrl(void)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}
}

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiGridCtrl::Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // ʹ��XML�ڵ��ʼ��div�ؼ�
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());

	// �����²��row�ڵ���Ϣ
	TiXmlElement* pRowElem = NULL;
	for (pRowElem = pXmlElem->FirstChildElement("row"); pRowElem != NULL; pRowElem=pRowElem->NextSiblingElement())
	{
		CStringA strIdA = pRowElem->Attribute("id");
		CStringA strCheckA = pRowElem->Attribute("check");
		CStringA strImageA = pRowElem->Attribute("image");
		CStringA strRightImageA = pRowElem->Attribute("right-img");
		CStringA strClrTextA = pRowElem->Attribute("crtext");

		int nCheck = -1;
		if(!strCheckA.IsEmpty())
		{
			nCheck = atoi(strCheckA);
		}

		// ���ͼƬ,ͨ��Skin��ȡ
		CStringA strSkin = "";
		if(strImageA.Find("skin:") == 0)
		{
			strSkin = DuiSystem::Instance()->GetSkin(strImageA);
		}else
		{
			strSkin = strImageA;
		}

		int nImageIndex = -1;
		CString strImage = _T("");
		if(strSkin.Find(".") != -1)
		{
			// ͼƬ�ļ�
			strImage = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		}else
		if(!strSkin.IsEmpty())
		{
			// ͼƬ����
			nImageIndex = atoi(strSkin);
		}

		// �ұ�ͼƬ,ͨ��Skin��ȡ
		CStringA strRightSkin = "";
		if(strRightImageA.Find("skin:") == 0)
		{
			strRightSkin = DuiSystem::Instance()->GetSkin(strRightImageA);
		}else
		{
			strRightSkin = strRightImageA;
		}

		int nRightImageIndex = -1;
		CString strRightImage = _T("");
		if(strRightSkin.Find(".") != -1)
		{
			// ͼƬ�ļ�
			strRightImage = DuiSystem::GetSkinPath() + CA2T(strRightSkin, CP_UTF8);
		}else
		if(!strRightSkin.IsEmpty())
		{
			// ͼƬ����
			nRightImageIndex = atoi(strRightSkin);
		}

		CString strId = CA2T(strIdA, CP_UTF8);
		Color clrText = Color(0,0,0,0);
		if(!strClrTextA.IsEmpty())
		{
			if(strClrTextA.Find(",") == -1)
			{
				clrText = CDuiObject::HexStringToColor(strClrTextA);
			}else
			{
				clrText = CDuiObject::StringToColor(strClrTextA);
			}
		}
		InsertRow(-1, strId, nImageIndex, clrText, strImage, nRightImageIndex, strRightImage, nCheck);
	}

    return TRUE;
}

// ������
BOOL CDuiGridCtrl::InsertRow(int nItem, CString strId, int nImageIndex, Color clrText, CString strImage,
							 int nRightImageIndex, CString strRightImage, int nCheck)
{
	GridRowInfo rowInfo;
	rowInfo.strId = strId;
	rowInfo.nCheck = nCheck;
	rowInfo.nImageIndex = nImageIndex;
	rowInfo.sizeImage.SetSize(0, 0);
	rowInfo.nRightImageIndex = nRightImageIndex;
	rowInfo.sizeRightImage.SetSize(0, 0);
	rowInfo.bRowColor = FALSE;
	rowInfo.clrText = clrText;
	rowInfo.nHoverItem = -1;
	if(clrText.GetValue() != Color(0, 0, 0, 0).GetValue())
	{
		rowInfo.bRowColor = TRUE;
	}

	// ���ͼƬ
	if(!strImage.IsEmpty() && (strImage.Find(_T(":")) == -1))
	{
		strImage = DuiSystem::GetSkinPath() + strImage;
	}
	if(!strImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		rowInfo.pImage = Image::FromFile(strImage, TRUE);
		if(rowInfo.pImage->GetLastStatus() == Ok)
		{
			rowInfo.sizeImage.SetSize(rowInfo.pImage->GetWidth() / 1, rowInfo.pImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pImage = NULL;
		if((rowInfo.nImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	// �ұ�ͼƬ
	if(!strRightImage.IsEmpty() && (strRightImage.Find(_T(":")) == -1))
	{
		strRightImage = DuiSystem::GetSkinPath() + strRightImage;
	}
	if(!strRightImage.IsEmpty())
	{
		// ʹ��������ָ����ͼƬ
		rowInfo.pRightImage = Image::FromFile(strRightImage, TRUE);
		if(rowInfo.pRightImage->GetLastStatus() == Ok)
		{
			rowInfo.sizeRightImage.SetSize(rowInfo.pRightImage->GetWidth() / 1, rowInfo.pRightImage->GetHeight());
		}
	}else
	{
		// ʹ������ͼƬ
		rowInfo.pRightImage = NULL;
		if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL) && (m_pImage->GetLastStatus() == Ok))
		{
			rowInfo.sizeRightImage.SetSize(m_sizeImage.cx, m_sizeImage.cy);
		}
	}

	//rowInfo.rcRow.SetRect(m_rc.left, m_rc.top, m_rc.left, m_rc.bottom);

	return InsertRow(nItem, rowInfo);
}

// ������
BOOL CDuiGridCtrl::InsertRow(int nItem, GridRowInfo &rowInfo)
{
	if(m_vecRowInfo.size() > 0)
	{
		CRect rc;
	}
	if(nItem <= -1 || nItem >= m_vecRowInfo.size())
	{
		m_vecRowInfo.push_back(rowInfo);
	}
	else
	{
		m_vecRowInfo.insert(m_vecRowInfo.begin() + nItem, rowInfo);
	}

	int nXPos = 0;//m_rc.left;
	int nYPos = 0;//m_rc.top;

	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - 8;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		rowInfoTemp.rcCheck.SetRect(0,0,0,0);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());

	UpdateControl(true);
	return true;
}

// ɾ����
BOOL CDuiGridCtrl::DeleteRow(int nItem)
{
	if((nItem < 0) || (nItem >= m_vecRowInfo.size()))
	{
		return FALSE;
	}

	int nIndex = 0;
	vector<GridRowInfo>::iterator it;
	for(it=m_vecRowInfo.begin();it!=m_vecRowInfo.end();++it)
	{
		if(nIndex == nItem)
		{
			m_vecRowInfo.erase(it);
			break;
		}
		nIndex++;
	}

	UpdateControl(true);
	return true;
}

// ��ȡĳһ������Ϣ
GridRowInfo* CDuiGridCtrl::GetRowInfo(int nRow)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return NULL;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	return &rowInfo;
}

// ����ĳһ���е���ɫ
void CDuiGridCtrl::SetRowColor(int nRow, Color clrText)
{
	if((nRow < 0) || (nRow >= m_vecRowInfo.size()))
	{
		return;
	}

	GridRowInfo &rowInfo = m_vecRowInfo.at(nRow);
	rowInfo.bRowColor = TRUE;
	rowInfo.clrText = clrText;
}

// ����б�
void CDuiGridCtrl::ClearItems()
{
	m_vecRowInfo.clear();
	m_pControScrollV->SetVisible(FALSE);
	UpdateControl(true);
}

// ��XML����Font-title����
HRESULT CDuiGridCtrl::OnAttributeFontTitle(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiFontInfo fontInfo;
	BOOL bFindFont = DuiSystem::Instance()->GetFont(strValue, fontInfo);
	if (!bFindFont) return E_FAIL;

	m_strFontTitle = fontInfo.strFont;
	m_nFontTitleWidth = fontInfo.nFontWidth;	
	m_fontTitleStyle = fontInfo.fontStyle;

	return bLoading?S_FALSE:S_OK;
}

// ���÷ָ���ͼƬ��Դ
BOOL CDuiGridCtrl::SetSeperator(UINT nResourceID/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}

	if(ImageFromIDResource(nResourceID, strType, m_pImageSeperator))
	{
		m_sizeSeperator.SetSize(m_pImageSeperator->GetWidth(), m_pImageSeperator->GetHeight());
		return true;
	}
	return false;
}

// ���÷ָ���ͼƬ�ļ�
BOOL CDuiGridCtrl::SetSeperator(CString strImage/* = TEXT("")*/)
{
	if(m_pImageSeperator != NULL)
	{
		delete m_pImageSeperator;
		m_pImageSeperator = NULL;
	}

	m_pImageSeperator = Image::FromFile(strImage, TRUE);

	if(m_pImageSeperator->GetLastStatus() == Ok)
	{
		m_sizeSeperator.SetSize(m_pImageSeperator->GetWidth(), m_pImageSeperator->GetHeight());
		return true;
	}
	return false;
}

// ��XML���÷ָ�ͼƬ��Ϣ����
HRESULT CDuiGridCtrl::OnAttributeImageSeperator(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			strImgFile = CA2T(strSkin, CP_UTF8);
		}
		if(!SetSeperator(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetSeperator(nResourceID, TEXT("PNG")))
		{
			if(!SetSeperator(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ���ü���ͼƬ��Դ
BOOL CDuiGridCtrl::SetCheckBoxImage(UINT nResourceID/* = 0*/, CString strType/*= TEXT("PNG")*/)
{
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}

	if(ImageFromIDResource(nResourceID, strType, m_pImageCheckBox))
	{
		m_sizeCheckBox.SetSize(m_pImageCheckBox->GetWidth() / 6, m_pImageCheckBox->GetHeight());
		return true;
	}
	return false;
}

// ���ü���ͼƬ�ļ�
BOOL CDuiGridCtrl::SetCheckBoxImage(CString strImage/* = TEXT("")*/)
{
	if(m_pImageCheckBox != NULL)
	{
		delete m_pImageCheckBox;
		m_pImageCheckBox = NULL;
	}

	m_pImageCheckBox = Image::FromFile(strImage, TRUE);

	if(m_pImageCheckBox->GetLastStatus() == Ok)
	{
		m_sizeCheckBox.SetSize(m_pImageCheckBox->GetWidth() / 6, m_pImageCheckBox->GetHeight());
		return true;
	}
	return false;
}

// ��XML���ü���ͼƬ��Ϣ����
HRESULT CDuiGridCtrl::OnAttributeImageCheckBox(const CStringA& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CStringA strSkin = "";
	if(strValue.Find("skin:") == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(".") != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = DuiSystem::GetSkinPath() + CA2T(strSkin, CP_UTF8);
		if(strSkin.Find(":") != -1)
		{
			strImgFile = CA2T(strSkin, CP_UTF8);
		}
		if(!SetCheckBoxImage(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = atoi(strSkin);
		if(!SetCheckBoxImage(nResourceID, TEXT("PNG")))
		{
			if(!SetCheckBoxImage(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

void CDuiGridCtrl::SetControlRect(CRect rc)
{
	m_rc = rc;
	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if(SCROLL_V == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.left = rcTemp.right - 8;
			}else
			if(LISTBK_AREA == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.right -= 8;
			}else
			{
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ���¼��������е�λ��
	int nXPos = 0;
	int nYPos = 0;
	for(size_t i = 0; i < m_vecRowInfo.size(); i++)
	{
		GridRowInfo &rowInfoTemp = m_vecRowInfo.at(i);
		int nItemWidth = m_rc.Width() - 8;
		rowInfoTemp.rcRow.SetRect(nXPos, nYPos, nXPos + nItemWidth, nYPos + m_nRowHeight);

		nYPos += m_nRowHeight;
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible((m_vecRowInfo.size() * m_nRowHeight) > m_rc.Height());
}

// �ж�ָ��������λ���Ƿ���ĳһ����
BOOL CDuiGridCtrl::PtInRow(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcRow;
	// rcRow�����ǲ����нڵ�ʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�еļ�����
BOOL CDuiGridCtrl::PtInRowCheck(CPoint point, GridRowInfo& rowInfo)
{
	CRect rc = rowInfo.rcCheck;
	// rcCheck�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	return rc.PtInRect(point);
}

// �ж�ָ��������λ���Ƿ���ĳһ�е�ĳ������,����������
int CDuiGridCtrl::PtInRowItem(CPoint point, GridRowInfo& rowInfo)
{
	/*CRect rc = rowInfo.rcLink1;
	// rcLink�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	if(rc.PtInRect(point))
	{
		return 0;
	}

	rc = rowInfo.rcLink2;
	// rcLink�����ǻ�ͼʱ�������İ��տؼ�������ʾ����Ϊ���յ�����,��Ҫת��Ϊ�������
	rc.OffsetRect(m_rc.left, m_rc.top-m_nVirtualTop);
	if(rc.PtInRect(point))
	{
		return 1;
	}*/

	return -1;
}

BOOL CDuiGridCtrl::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	int nOldHoverRow = m_nHoverRow;
	BOOL bHoverItemChange = FALSE;
	int nOldHoverItem = -1;

	if(m_rc.PtInRect(point))
	{
		if((m_nHoverRow != -1) && (m_nHoverRow < m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
					UpdateControl(TRUE);
				}
				return false;
			}
			m_nHoverRow = -1;		
		}

		BOOL bMousenDown = false;
		if((m_nDownRow != -1) && (m_nDownRow < m_vecRowInfo.size()))
		{
			GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
			nOldHoverItem = rowInfo.nHoverItem;
			if(PtInRow(point, rowInfo))
			{
				rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
				bMousenDown = true;
				m_nHoverRow = -1;
				if(nOldHoverItem != rowInfo.nHoverItem)
				{
					bHoverItemChange = TRUE;
				}
			}		
		}

		if(!bMousenDown)
		{
			for(size_t i = 0; i < m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				nOldHoverItem = rowInfo.nHoverItem;
				if(PtInRow(point, rowInfo))
				{
					rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
					m_nHoverRow = i;
					if(nOldHoverItem != rowInfo.nHoverItem)
					{
						bHoverItemChange = TRUE;
					}
					break;
				}
			}
		}
	}
	else
	{
		m_nHoverRow = -1;
	}

	if((nOldHoverRow != m_nHoverRow) || bHoverItemChange)
	{
		UpdateControl(TRUE);
		return true;
	}
	
	return false;
}

BOOL CDuiGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	// ���ô��ڽ���,��������޷����й����¼��Ĵ���
	SetWindowFocus();

	if(m_nHoverRow != -1)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo) && !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			if(m_nDownRow != m_nHoverRow)
			{
				m_nDownRow = m_nHoverRow;
				m_nHoverRow = -1;

				SendMessage(BUTTOM_DOWN, m_nDownRow, rowInfo.nHoverItem);

				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if(m_nDownRow != -1)
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo)&& !PtInRowCheck(point, rowInfo))	// �����¼�ֻ�����ſ�ʱ�򴥷�
		{
			rowInfo.nHoverItem = PtInRowItem(point, rowInfo);
			SendMessage(BUTTOM_DOWN, m_nDownRow, rowInfo.nHoverItem);
			return true;
		}
	}
	
	return false;
}

BOOL CDuiGridCtrl::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_vecRowInfo.size() == 0)
	{
		return false;
	}

	if(m_nHoverRow != -1)
	{
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nHoverRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}	
	}else
	if(m_nDownRow != -1)
	{
		// �������Ļ���֮ǰ������У�Ҳͬ���ᷢ��������¼�
		GridRowInfo &rowInfo = m_vecRowInfo.at(m_nDownRow);
		if(PtInRow(point, rowInfo))
		{
			if(PtInRowCheck(point, rowInfo))	// ����״̬�ı�
			{
				rowInfo.nCheck = ((rowInfo.nCheck == 1) ? 0 : 1);
				SendMessage(BUTTOM_UP, m_nDownRow, rowInfo.nCheck);
				UpdateControl(TRUE);

				return true;
			}
		}
	}

	return false;
}

// �����¼�����
BOOL CDuiGridCtrl::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if((m_vecRowInfo.size() * m_nRowHeight) <= m_rc.Height())
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CScrollV* pScroll = (CScrollV*)m_pControScrollV;
	if(pScroll->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CDuiGridCtrl::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if((uID == SCROLL_V) && (Msg == MSG_SCROLL_CHANGE))
	{
		// ����ǹ�������λ�ñ���¼�,��ˢ����ʾ
		UpdateControl(true);
	}else
	if((uID == GetID()) && (Msg == BUTTOM_DOWN) && (lParam != -1))
	{
		// ������е�ĳ������
		GridRowInfo* pRowInfo = GetRowInfo(wParam);
		if(pRowInfo)
		{
		}
	}

	return __super::OnMessage(uID, Msg, wParam, lParam); 
}

void CDuiGridCtrl::DrawControl(CDC &dc, CRect rcUpdate)
{
	// �б���ͼ����:
	// 1.�б�������߶�Ϊÿһ�и߶�*����
	// 2.�б���ʾ��top������scroll�ؼ���¼
	// 3.�ػ�ʱ��,����top����λ�ü������ʾ�ĵ�һ�е����,������ʾ�߶ȼ������ʾ�����һ�е����
	// 4.���ݼ��������ʾ����,����Ӧ�����ݵ��ڴ�dc��
	// 5.�������ʾ��top��������ڴ�dc�Ŀ���
	int nWidth = m_rc.Width() - 8;	// ��ȥ�������Ŀ���
	int nHeightAll = m_vecRowInfo.size()*m_nRowHeight; // �ܵ�����߶� //m_rc.Height();
	CScrollV* pScrollV = (CScrollV*)m_pControScrollV;
	int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRange = pScrollV->GetScrollMaxRange();

	m_nVirtualTop = nCurPos*(nHeightAll-m_rc.Height())/nMaxRange;	// ��ǰ������λ�ö�Ӧ�������topλ��
	m_nFirstViewRow = m_nVirtualTop / m_nRowHeight;					// ��ʾ�ĵ�һ�����
	m_nLastViewRow = (m_nVirtualTop + m_rc.Height()) / m_nRowHeight;	// ��ʾ�����һ�����
	if(m_nLastViewRow >= m_vecRowInfo.size())
	{
		m_nLastViewRow = m_vecRowInfo.size() - 1;
	}
	if(m_nLastViewRow < 0)
	{
		m_nLastViewRow = 0;
	}
	int nHeightView = (m_nLastViewRow - m_nFirstViewRow +1) * m_nRowHeight;	// ��ʾ�漰��������߶�
	int nYViewPos = m_nVirtualTop - (m_nFirstViewRow * m_nRowHeight);		// �ڴ�dc��ʾ����Ļʱ���topλ��
	if(nYViewPos < 0)
	{
		nYViewPos = 0;
	}

	if(!m_bUpdate)
	{
		UpdateMemDC(dc, nWidth, nHeightView);

		Graphics graphics(m_memDC);
		
		m_memDC.BitBlt(0, 0, nWidth, nHeightView, &dc, m_rc.left ,m_rc.top, WHITENESS);	// ����ɫ����
		DrawVerticalTransition(m_memDC, dc, CRect(0, nYViewPos, nWidth, m_rc.Height()+nYViewPos),	// ����͸����
				m_rc, m_nBkTransparent, m_nBkTransparent);
		
		FontFamily fontFamilyTitle(m_strFontTitle.AllocSysString());
		Font fontTitle(&fontFamilyTitle, (REAL)m_nFontTitleWidth, m_fontTitleStyle, UnitPixel);

		FontFamily fontFamily(m_strFont.AllocSysString());
		Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);

		SolidBrush solidBrush(m_clrText);			// �������ֻ�ˢ
		SolidBrush solidBrushH(m_clrTextHover);		// �ȵ����ֻ�ˢ
		SolidBrush solidBrushD(m_clrTextDown);		// ��ǰ�л�ˢ
		SolidBrush solidBrushT(m_clrTitle);			// �������ֻ�ˢ
		SolidBrush solidBrushS(m_clrSeperator);		// �ָ��߻�ˢ

		graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );

		// ��ͨ���ֵĶ��뷽ʽ
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentNear);	// �����
		if(m_uVAlignment == VAlign_Top)
		{
			strFormat.SetLineAlignment(StringAlignmentNear);	// �϶���
		}else
		if(m_uVAlignment == VAlign_Middle)
		{
			strFormat.SetLineAlignment(StringAlignmentCenter);	// �м����
		}else
		if(m_uVAlignment == VAlign_Bottom)
		{
			strFormat.SetLineAlignment(StringAlignmentFar);	// �¶���
		}
		strFormat.SetTrimming(StringTrimmingEllipsisWord);	// �Ե���Ϊ��λȥβ,��ȥ����ʹ��ʡ�Ժ�
		//strFormat.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}

		// ʱ���ֶβ����Ҷ���
		StringFormat strFormatRight;
		strFormatRight.SetAlignment(StringAlignmentFar);	// �Ҷ���
		if(m_uVAlignment == VAlign_Top)
		{
			strFormatRight.SetLineAlignment(StringAlignmentNear);	// �϶���
		}else
		if(m_uVAlignment == VAlign_Middle)
		{
			strFormatRight.SetLineAlignment(StringAlignmentCenter);	// �м����
		}else
		if(m_uVAlignment == VAlign_Bottom)
		{
			strFormatRight.SetLineAlignment(StringAlignmentFar);	// �¶���
		}
		//strFormatRight.SetFormatFlags( StringFormatFlagsNoClip | StringFormatFlagsMeasureTrailingSpaces);
		if(!m_bTextWrap)
		{
			strFormatRight.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);	// ������
		}
		
		if(m_vecRowInfo.size() > 0)
		{
			for(size_t i = m_nFirstViewRow; i <= m_nLastViewRow && i <= m_vecRowInfo.size(); i++)
			{
				GridRowInfo &rowInfo = m_vecRowInfo.at(i);
				SolidBrush solidBrushRow(rowInfo.clrText);	// �ж������ɫ

				int nXPos = 0;
				int nVI = i - m_nFirstViewRow;

				// ������
				int nCheckImgY = 3;
				if((m_sizeCheckBox.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
				{
					nCheckImgY = (m_nRowHeight - m_sizeCheckBox.cy) / 2 + 1;
				}
				if((rowInfo.nCheck != -1) && (m_pImageCheckBox != NULL))
				{
					int nCheckImageIndex = ((m_nHoverRow == i) ? ((rowInfo.nCheck==1) ? 4 : 1) : ((rowInfo.nCheck==1) ? 2 : 0));
					graphics.DrawImage(m_pImageCheckBox, Rect(nXPos, nVI*m_nRowHeight + nCheckImgY, m_sizeCheckBox.cx, m_sizeCheckBox.cy),
						nCheckImageIndex * m_sizeCheckBox.cx, 0, m_sizeCheckBox.cx, m_sizeCheckBox.cy, UnitPixel);
					rowInfo.rcCheck.SetRect(nXPos, i*m_nRowHeight + nCheckImgY, nXPos + m_sizeCheckBox.cx, i*m_nRowHeight + nCheckImgY + m_sizeCheckBox.cy);
					nXPos += (m_sizeCheckBox.cx + 3);
				}

				// �������ͼƬ
				int nImgY = 3;
				if(rowInfo.pImage != NULL)
				{
					if((rowInfo.sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
					graphics.DrawImage(rowInfo.pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy),
						0, 0, rowInfo.sizeImage.cx, rowInfo.sizeImage.cy, UnitPixel);
					nXPos += (rowInfo.sizeImage.cx + 3);
				}else
				if((rowInfo.nImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nXPos, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nXPos += (m_sizeImage.cx + 3);
				}

				// �����ұ�ͼƬ
				int nRightImageWidth = 0;
				nImgY = 3;
				if(rowInfo.pRightImage != NULL)
				{
					if((rowInfo.sizeRightImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - rowInfo.sizeRightImage.cy) / 2 + 1;
					}
					// ʹ��������ָ����ͼƬ
					graphics.DrawImage(rowInfo.pRightImage, Rect(nWidth-rowInfo.sizeRightImage.cx-1, nVI*m_nRowHeight + nImgY, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy),
						0, 0, rowInfo.sizeRightImage.cx, rowInfo.sizeRightImage.cy, UnitPixel);
					nRightImageWidth = rowInfo.sizeRightImage.cx + 1;
				}else
				if((rowInfo.nRightImageIndex != -1) && (m_pImage != NULL))
				{
					if((m_sizeImage.cy*2 > m_nRowHeight) || (m_uVAlignment == VAlign_Middle))
					{
						nImgY = (m_nRowHeight - m_sizeImage.cy) / 2 + 1;
					}
					// ʹ������ͼƬ
					graphics.DrawImage(m_pImage, Rect(nWidth-m_sizeImage.cx-1, nVI*m_nRowHeight + nImgY, m_sizeImage.cx, m_sizeImage.cy),
						rowInfo.nRightImageIndex*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, UnitPixel);
					nRightImageWidth = m_sizeImage.cx + 1;
				}

				// ������
				RectF rect(nXPos, nVI*m_nRowHeight + 1, nWidth-20, m_bSingleLine ? (m_nRowHeight - 2) : (m_nRowHeight / 2 - 2) );
				if(m_nHoverRow == i)
				{

				}else
				if(m_nDownRow == i)
				{

				}else
				{

				}

				// ���ָ���(��������ģʽ)
				if(m_pImageSeperator != NULL)
				{
					// ʹ������ģʽ���Ի�ͼ
					graphics.DrawImage(m_pImageSeperator, RectF(0, (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy),
							0, 0, m_sizeSeperator.cx, m_sizeSeperator.cy, UnitPixel);

					// ʹ��ƽ�̷�ʽ������(�ݲ�ʹ�����ַ�ʽ)
					//TextureBrush tileBrush(m_pImageSeperator, WrapModeTile);
					//graphics.FillRectangle(&tileBrush, RectF(0, (nVI+1)*m_nRowHeight, nWidth-2, m_sizeSeperator.cy));
				}else
				{
					// δָ��ͼƬ,�򻭾���
					graphics.FillRectangle(&solidBrushS, 0, (nVI+1)*m_nRowHeight, nWidth-2, 1);
				}
			}
		}
	}

	// ���������DC,ʹ����ķ�ʽ�ϲ�����
	dc.BitBlt(m_rc.left,m_rc.top, nWidth, m_rc.Height(), &m_memDC, 0, nYViewPos, SRCCOPY);//SRCAND);
}