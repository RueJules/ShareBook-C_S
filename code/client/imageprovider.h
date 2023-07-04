#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class ImageProvider : public QQuickImageProvider
//QQuickImageProvider用于在QML应用程序中提供高级图像加载功能。它允许QML中的图像为：
    //使用QPixmaps而不是实际图像文件加载
    //在单独的线程中异步加载
//什么东西？？
{
public:
    ImageProvider():QQuickImageProvider(QQuickImageProvider::Pixmap){}
    void setPixmap(QString id,const QPixmap &pixmap) { if (!pixmap.isNull()&&m_pixmap.count(id)==0) m_pixmap.emplace(id,pixmap); }
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    std::map<QString,QPixmap> m_pixmap;
};

#endif // IMAGEPROVIDER_H
