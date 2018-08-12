#include "dialogvideosviewer.h"
#include "ui_dialogvideosviewer.h"


DialogVideosViewer::DialogVideosViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVideosViewer)
{
    ui->setupUi(this);
}
//------------------

DialogVideosViewer::~DialogVideosViewer()
{
    delete ui;
}
//------------------

void DialogVideosViewer::showEkadashiVideos() {
//    ui->textBrowser->loadResource(,QUrl(":/html/ekadashi_videos"));
}
//------------------
