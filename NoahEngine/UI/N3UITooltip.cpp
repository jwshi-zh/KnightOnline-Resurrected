#include "pch.h"
#include "N3UITooltip.h"
#include "N3UIString.h"
#include "N3UIStatic.h"

CN3UITooltip::CN3UITooltip()
{
	m_eType = UI_TYPE_TOOLTIP;

	m_fHoverTime = 0.0f;
	m_bVisible = false;
	m_bSetText = false;
	ZeroMemory(&m_ptCursor, sizeof(m_ptCursor));
}

CN3UITooltip::~CN3UITooltip()
{
}

void CN3UITooltip::Release()
{
	CN3UIBase::Release();
	m_fHoverTime = 0.0f;
	m_bVisible = false;
	m_bSetText = false;
	ZeroMemory(&m_ptCursor, sizeof(m_ptCursor));
}

void CN3UITooltip::Render()
{
	if (!m_bVisible || !m_bSetText) return;
	if (nullptr == m_pImageBkGnd)
	{	// If there is no image, draw it by default
		static __VertexTransformedColor	pVB[8];
		static const WORD	pIB[16] = { 0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4 };
		static const D3DCOLOR BkColor = 0x80000000;
		static const D3DCOLOR BorderColorOut = 0xff808080;
		static const D3DCOLOR BorderColorIn = 0xffc0c0c0;
		pVB[0].Set(m_rcRegion.left, m_rcRegion.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, BkColor);
		pVB[1].Set(m_rcRegion.right, m_rcRegion.top, UI_DEFAULT_Z, UI_DEFAULT_RHW, BkColor);
		pVB[2].Set(m_rcRegion.right, m_rcRegion.bottom, UI_DEFAULT_Z, UI_DEFAULT_RHW, BkColor);
		pVB[3].Set(m_rcRegion.left, m_rcRegion.bottom, UI_DEFAULT_Z, UI_DEFAULT_RHW, BkColor);
		pVB[4].Set(m_rcRegion.left + 1, m_rcRegion.top + 1, UI_DEFAULT_Z, UI_DEFAULT_RHW, BorderColorIn);
		pVB[5].Set(m_rcRegion.right - 1, m_rcRegion.top + 1, UI_DEFAULT_Z, UI_DEFAULT_RHW, BorderColorIn);
		pVB[6].Set(m_rcRegion.right - 1, m_rcRegion.bottom - 1, UI_DEFAULT_Z, UI_DEFAULT_RHW, BorderColorIn);
		pVB[7].Set(m_rcRegion.left + 1, m_rcRegion.bottom - 1, UI_DEFAULT_Z, UI_DEFAULT_RHW, BorderColorIn);

		// set texture stage state
		s_lpD3DDev->SetTexture(0, nullptr);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		// draw
		s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
		HRESULT hr = s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, pVB, sizeof(__VertexTransformedColor));	// paint the background color

		__VertexTransformedColor* pTemp = pVB;
		int i;
		for (i = 0; i < 4; ++i) pTemp++->color = BorderColorOut;	// Change the color of the outer border.
		s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 8,
			pIB, D3DFMT_INDEX16, pVB, sizeof(__VertexTransformedColor));	// paint the border

		// drawing letters
		m_pBuffOutRef->Render();
	}
	else CN3UIStatic::Render();
}

void CN3UITooltip::SetText(const std::string& szText)
{
	if (!m_bVisible || m_bSetText) return;

	// Resize the tooltip box.
	const int iStrLen = szText.size();
	if (0 == iStrLen || nullptr == m_pBuffOutRef) return;

	m_pBuffOutRef->ClearOnlyStringBuffer();	// First, delete the existing text (without sorting).
	SIZE size;
	if (m_pBuffOutRef->GetTextExtent(szText, iStrLen, &size))
	{
		if (std::string::npos != szText.find('\n'))
		{	// Since \n is included in the '\n' string, it is considered multi-line.
			m_pBuffOutRef->SetStyle(UISTYLE_STRING_ALIGNLEFT | UISTYLE_STRING_ALIGNTOP);
		}
		else if (iStrLen < 25)
		{	// If it is less than 25 bytes, it is just one line.
			m_pBuffOutRef->SetStyle(UISTYLE_STRING_SINGLELINE | UISTYLE_STRING_ALIGNCENTER | UISTYLE_STRING_ALIGNVCENTER);
		}
		else
		{	// Calculate an appropriate size because it is a single line.
			SIZE CharSize = { 0,0 };
			if (FALSE == m_pBuffOutRef->GetTextExtent("가", 2, &CharSize)) return;

			int iDiv = 25;
			int iLineCount = (iStrLen + iDiv - 1) / iDiv;
			while (iLineCount > (iDiv / 2))
			{
				iDiv += 5;
				iLineCount = (iStrLen + iDiv - 1) / iDiv;
			}
			size.cx = ((iDiv + 1) * CharSize.cx + 1) / 2;
			size.cy = iLineCount * CharSize.cy;
		}

		size.cx += 12;
		size.cy += 12;
		SetSize(size.cx, size.cy);
	}
	m_pBuffOutRef->SetString(szText);

	// reposition
	POINT	ptNew = m_ptCursor;
	ptNew.x -= (m_rcRegion.right - m_rcRegion.left) / 2;
	ptNew.y -= (m_rcRegion.bottom - m_rcRegion.top) + 10;

	const D3DVIEWPORT9& vp = s_CameraData.vp;
	const int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
	const int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;

	if (ptNew.x + iRegionWidth > vp.X + vp.Width)	// If you move to the right U of the screen
		ptNew.x = vp.X + vp.Width - iRegionWidth;
	if (ptNew.x < vp.X)	ptNew.x = vp.X;	// Off the left side of the screen

	if (ptNew.y + iRegionHeight > vp.Y + vp.Height)	// Off the bottom of the screen
		ptNew.y = vp.Y + vp.Height - iRegionHeight;
	if (ptNew.y < vp.Y) ptNew.y = vp.Y;	// Off the top of the screen

	SetPos(ptNew.x, ptNew.y);

	m_bSetText = true;
}

void CN3UITooltip::Tick()
{
	const int fOldTime = m_fHoverTime;
	m_fHoverTime += s_fSecPerFrm;
	static const float fDisplayTime = 0.3f;
	if (fOldTime < fDisplayTime && m_fHoverTime >= fDisplayTime)
	{
		SetVisible(true);	// Display tool tip
	}
}

DWORD CN3UITooltip::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	// Set m_fHoverTime to 0 when the mouse is moved
	if (ptCur.x != ptOld.x || ptCur.y != ptOld.y)
	{
		m_fHoverTime = 0.0f;
		m_bSetText = false;
		SetVisible(false);// Remove tool tip.
	}
	else
	{	// Save cursor position if not moved
		m_ptCursor = ptCur;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}
