#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

// НАЧАЛО: директивы, глобальные переменные и константы
#include <QDialog>
//---------------------------
// КОНЕЦ: директивы, глобальные переменные и константы
//---------------------------------------------------------------------------------


// НАЧАЛО: классы
namespace Ui {
class DialogAbout;
}

/// \brief класс диалога о программе
class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

private:
    Ui::DialogAbout *ui;
};
//---------------------------
// КОНЕЦ: классы
//---------------------------------------------------------------------------------

#endif // DIALOGABOUT_H
