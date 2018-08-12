#ifndef DIALOGVIDEOSVIEWER_H
#define DIALOGVIDEOSVIEWER_H

#include <QDialog>

namespace Ui {
class DialogVideosViewer;
}
//------------------


class DialogVideosViewer : public QDialog
{
    Q_OBJECT

public:
    explicit DialogVideosViewer(QWidget *parent = nullptr);
    ~DialogVideosViewer();

    // Показать локальные копии видео про экадаш.
    void showEkadashiVideos();

private:
    Ui::DialogVideosViewer *ui;
};
//------------------

#endif // DIALOGVIDEOSVIEWER_H
