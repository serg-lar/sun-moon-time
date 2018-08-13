#ifndef DIALOGVIDEOSVIEWER_H
#define DIALOGVIDEOSVIEWER_H

#include <QDialog>

namespace Ui {
class DialogVideosViewer;
}
//------------------

// TODO Сделать отображение видео файлов внутри интерфейса приложения.
class QDialogVideosViewer : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogVideosViewer(QWidget *parent = nullptr);
    ~QDialogVideosViewer();

    // Показать локальные копии видео про экадаш.
    void showEkadashiVideos();

private:
    Ui::DialogVideosViewer *ui;
};
//------------------

#endif // DIALOGVIDEOSVIEWER_H
