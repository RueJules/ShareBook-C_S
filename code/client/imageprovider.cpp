#include "imageprovider.h"



QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    if(m_pixmap.count(id)>0)
        return m_pixmap[id];
}
