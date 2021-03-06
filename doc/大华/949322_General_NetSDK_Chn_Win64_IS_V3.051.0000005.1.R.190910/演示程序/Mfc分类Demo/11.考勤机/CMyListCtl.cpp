#include "stdafx.h"
#include "CMyListCtl.h"

CMyListCtl::CMyListCtl()
{

}

void CMyListCtl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC*   pDC   =   CDC::FromHandle(lpDrawItemStruct-> hDC);
	CRect   rcItem(lpDrawItemStruct->rcItem);
	int   nItem   =   lpDrawItemStruct->itemID;
	COLORREF   clrTextSave,   clrBkSave;
	static   _TCHAR   szBuff[2048];//这里是你要显示的字符串长度，想多长有多长
	LV_ITEM   lvi;
	lvi.mask   =   LVIF_TEXT   |   LVIF_STATE;//LVIF_IMAGE   |
	lvi.iItem   =   nItem;
	lvi.iSubItem   =   0;
	lvi.pszText   =   szBuff;
	lvi.cchTextMax   =   sizeof(szBuff);
	lvi.stateMask   =   0xFFFF;
	GetItem(&lvi);

	CFont MyTextFont;
	MyTextFont.CreatePointFont(90,"宋体");
	pDC->SelectObject(&MyTextFont);

	BOOL   bSelected   =   (lvi.state   &   LVIS_SELECTED);
	CRect   rcAllLabels;
	GetItemRect(nItem,   rcAllLabels,   LVIR_BOUNDS);
	if   (bSelected)
	{
		clrTextSave   =   pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave   =   pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels,&CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
	{
		clrTextSave=pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave=pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(rcAllLabels,&CBrush(::GetSysColor(COLOR_WINDOW)));
	}
	GetItemRect(nItem,   rcItem,   LVIR_LABEL);
	rcItem.left +=1; 
	pDC->DrawText(szBuff,-1,rcItem,DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP|DT_VCENTER|DT_END_ELLIPSIS);
	LV_COLUMN   lvc;
	lvc.mask   =   LVCF_FMT   |   LVCF_WIDTH;
	for(int   nColumn   =   1;   GetColumn(nColumn,   &lvc);   nColumn++)
	{
		rcItem.left   =   rcItem.right;
		rcItem.right   +=   lvc.cx;

		int   nRetLen   =   GetItemText(nItem,   nColumn,
		szBuff,   sizeof(szBuff));
		if   (nRetLen   ==   0)
		continue;
		UINT   nJustify   =   DT_LEFT ;
		switch(lvc.fmt   &   LVCFMT_JUSTIFYMASK)
		{
			case   LVCFMT_RIGHT:
			nJustify   =   DT_RIGHT;
			break;
			case   LVCFMT_CENTER:
			nJustify   =   DT_CENTER;
			break;
			default:
			break;
		}
		rcItem.left +=1; 
		pDC->DrawText(szBuff,   -1,   rcItem,
		nJustify   |   DT_SINGLELINE   |   DT_NOPREFIX   |   DT_NOCLIP   |   DT_VCENTER | DT_END_ELLIPSIS);// DT_END_ELLIPSIS可以实现文字显示不开后的...效果
	}
	if   (lvi.state   &   LVIS_FOCUSED)
		pDC->DrawFocusRect(rcAllLabels);
	if   (bSelected)
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}
BOOL CMyListCtl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(((NMHDR *)lParam)->code)
	{
	case HDN_BEGINTRACKW:
	case HDN_BEGINTRACKA:
	case HDN_DIVIDERDBLCLICKA:
	case HDN_DIVIDERDBLCLICKW:
	case HDN_ENDTRACKA:
	case HDN_ENDTRACKW:
	case HDN_TRACKA:
	case HDN_TRACKW:
		*pResult = TRUE;
		return TRUE;
	}
	int DWORD = ((NMHDR *)lParam)->code;
	return CListCtrl::OnNotify(wParam, lParam, pResult);
}
BEGIN_MESSAGE_MAP(CMyListCtl, CListCtrl)
//	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CMyListCtl::OnLvnColumnclick)
END_MESSAGE_MAP()

//void CMyListCtl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}
