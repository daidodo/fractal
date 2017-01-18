
// FractalView.cpp : CFractalView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Fractal.h"
#endif

#include "FractalDoc.h"
#include "FractalView.h"

#include <complex>
#include <thread>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// https://en.wikipedia.org/wiki/Mandelbrot_set
static DWORD MandelbrotSet(__Double x, __Double y)
{
    __Double a = 0, b = 0;
    for (int i = 0; i < 1000; ++i) {
        auto aa = a*a - b*b + x;
        auto bb = 2 * a*b + y;
        if (aa*aa + bb*bb > 4)
            return RGB(0xFF - i, i*3, i*7);
        a = aa;
        b = bb;
    }
    return 0;
}

// https://en.wikipedia.org/wiki/Julia_set
static DWORD JuliaSet(__Double x, __Double y)
{
    typedef std::complex<__Double> __Compl;
    __Compl z(x, y), c(0.01, 0);
    for (int i = 0; i < 500; ++i) {
        z = z*std::exp(z) + c;
        if(z.real()*z.real() + z.imag()*z.imag() > 1e10)
            return RGB(0xFF - i*11, i * 3, i * 7);
    }
    return 0;
}

// https://en.wikipedia.org/wiki/Burning_Ship_fractal
static DWORD BurningShip(__Double x, __Double y)
{
    __Double a = 0, b = 0;
    for (int i = 0; i < 1000; ++i) {
        auto aa = a*a - b*b + x;
        auto bb = 2 * std::abs(a*b) + y;
        if (aa*aa + bb*bb > 4)
            return RGB(0xFF - i, i * 3, i * 7);
        a = aa;
        b = bb;
    }
    return 0;
}

static DWORD calc(__Double x, __Double y)
{
    //return MandelbrotSet(x, y);
    //return JuliaSet(x, y);
    return BurningShip(x, y);
}

FractalImage::FractalImage(const CRect & rect) {
    bmp.CreateBitmap(rect.Width(), rect.Height(), 1, 32, 0);
    bmp.GetBitmap(&bmpInfo);
    adwBits.resize(bmpInfo.bmWidth * bmpInfo.bmHeight);
}

void FractalImage::Draw(CDC * pDC)
{
    ASSERT_VALID(pDC);
    bmp.SetBitmapBits(bmpInfo.bmWidth * bmpInfo.bmHeight * sizeof(DWORD), &adwBits[0]);
    CDC dcMemory;
    dcMemory.CreateCompatibleDC(pDC);
    CBitmap* pOldBitmap = dcMemory.SelectObject(&bmp);
    pDC->BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMemory,
        0, 0, SRCCOPY);
    dcMemory.SelectObject(pOldBitmap);
}

void FractalImage::Update()
{
    const auto D = delta();
    if (D == 0)
        return;
    const auto x = (xfrom + xto - D * bmpInfo.bmWidth) / 2;
    const auto y = (yfrom + yto - D * bmpInfo.bmHeight) / 2;
    const int TC = std::thread::hardware_concurrency();
    const int Y = (bmpInfo.bmHeight + TC - 1) / TC;
    std::vector<std::thread> th(TC);
    for (int t = 0; t < TC; ++t) {
        auto f = [&](int j, int s) {
            for (s = min(j + s, bmpInfo.bmHeight); j < s; ++j)
                for (int i = 0; i < bmpInfo.bmWidth; ++i)
                    bit(i, j) = calc(x + D * i, y + D * j);
        };
        th[t] = std::thread(f, t*Y, Y);
    }
    for (auto & t : th)
        t.join();
}

void FractalImage::Update(CPoint start, CPoint end)
{
    const auto D = delta();
    if (D == 0)
        return;
    if (start.x > end.x)
        std::swap(start.x, end.x);
    if (start.y > end.y)
        std::swap(start.y, end.y);
    const auto cx = (xfrom + xto) / 2, cy = (yfrom + yto) / 2;
    const auto x1 = cx + (start.x - bmpInfo.bmWidth / 2) * D, x2 = x1 + (end.x - start.x) * D;
    const auto y1 = cy + (start.y - bmpInfo.bmHeight / 2) * D, y2 = y1 + (end.y - start.y) * D;
    xfrom = x1;
    xto = x2;
    yfrom = y1;
    yto = y2;
    Update();
}

__Double FractalImage::delta() const
{
    __Double r = 0;
    if (bmpInfo.bmWidth > 0)
        r = (xto - xfrom) / bmpInfo.bmWidth;
    if (bmpInfo.bmHeight > 0)
        r = max(r, yto - yfrom) / bmpInfo.bmHeight;
    return (r < 0 ? 0 : r);
}

void FractalImage::SaveToFile(LPCTSTR fname) const
{
    CBitmap tb;
    tb.CreateBitmap(bmpInfo.bmWidth, bmpInfo.bmHeight, 1, 32, 0);
    tb.SetBitmapBits(bmpInfo.bmWidth * bmpInfo.bmHeight * sizeof(DWORD), &adwBits[0]);
    CImage image;
    image.Attach(tb.operator HBITMAP());
    image.Save(fname);
}

// CFractalView

IMPLEMENT_DYNCREATE(CFractalView, CView)

BEGIN_MESSAGE_MAP(CFractalView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_COMMAND(ID_FILE_NEW, &CFractalView::OnFileNew)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_FILE_SAVE, &CFractalView::OnFileSave)
END_MESSAGE_MAP()

// CFractalView ����/����

CFractalView::CFractalView()
{
	// TODO: �ڴ˴���ӹ������

}

CFractalView::~CFractalView()
{
}

void CFractalView::initFractal(const CRect & rect)
{
    fractal_.reset(new FractalImage(rect));
    fractal_->setRangeX(-20, 20);
    fractal_->setRangeY(-20, 20);
    fractal_->Update();
}

BOOL CFractalView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CFractalView ����

void CFractalView::OnDraw(CDC* pDC)
{
	CFractalDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
    if (fractal_)
        fractal_->Draw(pDC);
}


// CFractalView ��ӡ

BOOL CFractalView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CFractalView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CFractalView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CFractalView ���

#ifdef _DEBUG
void CFractalView::AssertValid() const
{
	CView::AssertValid();
}

void CFractalView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFractalDoc* CFractalView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFractalDoc)));
	return (CFractalDoc*)m_pDocument;
}
#endif //_DEBUG


// CFractalView ��Ϣ�������


void CFractalView::OnFileNew()
{
    // TODO: �ڴ���������������
    CRect stRect;
    GetClientRect(&stRect);
    initFractal(stRect);
    Invalidate();
}


void CFractalView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    start_ = end_ = point;
    mouseDown_ = true;

    CView::OnLButtonDown(nFlags, point);
}


void CFractalView::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    mouseDown_ = false;
    end_ = point;
    if (fractal_)
        fractal_->Update(start_, end_);
    CView::OnLButtonUp(nFlags, point);
    Invalidate();
}

void CFractalView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if (fractal_ && mouseDown_) {
        CClientDC dc(this);
        CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
        CBrush *POldBrush = dc.SelectObject(pBrush);
        int nOldMode = dc.SetROP2(R2_NOTXORPEN);
        dc.Rectangle(&CRect(start_, end_));
        dc.Rectangle(&CRect(start_, point));
        end_ = point;
        dc.SelectObject(POldBrush);
        dc.SetROP2(nOldMode);
    }

    CView::OnMouseMove(nFlags, point);
}


void CFractalView::OnFileSave()
{
    // TODO: �ڴ���������������
    if (!fractal_)
        return;
    BOOL isOpen = FALSE;        //�Ƿ��(����Ϊ����)  
    CString defaultDir = L"C:\\";   //Ĭ�ϴ򿪵��ļ�·��  
    CString fileName = L"fractal.bmp";         //Ĭ�ϴ򿪵��ļ���  
    CString filter = L"�ļ� (*.bmp)||";   //�ļ����ǵ�����  
    CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
    openFileDlg.GetOFN().lpstrInitialDir = L"E:\\fractal.bmp";
    INT_PTR result = openFileDlg.DoModal();
    CString filePath = defaultDir + "\\" + fileName;
    if (result != IDOK)
        return;
    filePath = openFileDlg.GetPathName();
    fractal_->SaveToFile(filePath);

}
