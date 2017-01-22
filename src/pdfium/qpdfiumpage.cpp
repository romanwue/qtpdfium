#include "qpdfiumpage.h"
#include "../3rdparty/pdfium/public/fpdfview.h"
#include "../3rdparty/pdfium/core/fpdfapi/page/cpdf_page.h"

QT_BEGIN_NAMESPACE

PageHolder::PageHolder(CPDF_Page *page)
    : m_page(page)
{
}

PageHolder::~PageHolder()
{
    if (m_page)
        FPDF_ClosePage(m_page);
}

QPdfiumPage::QPdfiumPage(QSharedPointer<PageHolder> page, int index)
    : m_pageHolder(page)
    , m_index(index)
{
}

QPdfiumPage::QPdfiumPage(const QPdfiumPage &other)
    : m_pageHolder(other.m_pageHolder)
    , m_index(other.m_index)
{
}

QPdfiumPage &QPdfiumPage::operator=(const QPdfiumPage &other)
{
    m_pageHolder = other.m_pageHolder;
    m_index = other.m_index;
    return *this;
}

QPdfiumPage::~QPdfiumPage()
{
}

qreal QPdfiumPage::width() const
{
    if (!m_pageHolder)
        return -1;
    return m_pageHolder.data()->m_page->GetPageWidth();
}

qreal QPdfiumPage::height() const
{
    if (!m_pageHolder)
        return -1;
    return m_pageHolder.data()->m_page->GetPageHeight();
}

bool QPdfiumPage::isValid() const
{
    return !m_pageHolder.isNull();
}

QImage QPdfiumPage::image(qreal scale)
{
    if (!isValid())
        return QImage();

    QImage image(width()*scale, height()*scale, QImage::Format_RGBA8888);

    if(image.isNull())
        return QImage();

    image.fill(0xFFFFFFFF);

    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(),
                                             FPDFBitmap_BGRA,
                                             image.scanLine(0), image.bytesPerLine());
    if(bitmap == NULL) {
        return QImage();
    }

    FPDF_RenderPageBitmap(bitmap, m_pageHolder.data()->m_page,
                          0, 0, image.width(), image.height(),
                          0, 0); // no rotation, no flags
    FPDFBitmap_Destroy(bitmap);
    bitmap = NULL;

    for(int i = 0; i < image.height(); i++) {
        uchar *pixels = image.scanLine(i);
        for(int j = 0; j < image.width(); j++) {
            qSwap(pixels[0], pixels[2]);
            pixels += 4;
        }
    }

    return image;

}

QT_END_NAMESPACE

