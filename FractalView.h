
// FractalView.h : CFractalView ��Ľӿ�
//

#pragma once

#include <memory>
#include <vector>

typedef long double __Double;

class FractalImage {
    CBitmap bmp;
    BITMAP bmpInfo;
    std::vector<DWORD> adwBits;
    __Double xfrom, xto;
    __Double yfrom, yto;
public:
    explicit FractalImage(const CRect & rect);
    void setRangeX(__Double from, __Double to) { xfrom = from; xto = to; }
    void setRangeY(__Double from, __Double to) { yfrom = from; yto = to; }
    void Draw(CDC * pDC);
    void Update();
    void Update(CPoint start, CPoint end);
    void SaveToFile(LPCTSTR fname) const;
private:
    DWORD & bit(int x, int y) {
        return adwBits[x + y * bmpInfo.bmWidth];
    }
    DWORD bit(int x, int y) const {
        return adwBits[x + y * bmpInfo.bmWidth];
    }
    __Double delta() const;
};

class CFractalView : public CView
{
    std::shared_ptr<FractalImage> fractal_;
    bool mouseDown_ = false;
    CPoint start_, end_;
    void initFractal(const CRect & rect);
protected: // �������л�����
	CFractalView();
	DECLARE_DYNCREATE(CFractalView)

// ����
public:
	CFractalDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CFractalView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnFileNew();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnFileSave();
};

#ifndef _DEBUG  // FractalView.cpp �еĵ��԰汾
inline CFractalDoc* CFractalView::GetDocument() const
   { return reinterpret_cast<CFractalDoc*>(m_pDocument); }
#endif

