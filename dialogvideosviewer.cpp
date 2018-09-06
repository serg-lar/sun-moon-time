// Qt
#include <QFile>
// sun-moon-time
#include "dialogvideosviewer.h"
#include "ui_dialogvideosviewer.h"
#include "generalmisc.h"


QDialogVideosViewer::QDialogVideosViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVideosViewer)
{
    ui->setupUi(this);
}
//------------------

QDialogVideosViewer::~QDialogVideosViewer()
{
    delete ui;
}
//------------------

void QDialogVideosViewer::showEkadashiVideos() {
    // Показать локальные копии видео о экадаше.
    // Открыть файл с html документом и показать его в виджете.
    QFile ekadashiVideosFile(":/html/ekadashi_videos");
    if (true == ekadashiVideosFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ekadashiVideosHtml(&ekadashiVideosFile);
        ui->textBrowser->setHtml(ekadashiVideosHtml.readAll());
        ekadashiVideosFile.close();

        // показать этот виджет - диалоговое окно.
        show();
    }
    else { // Ошибка открытия файла
        qWarning() << Q_FUNC_INFO << SunMoonTimeGeneralMisc::errors::fileOpenError() << "(ekadashi videos file)";
    }
}
//------------------

