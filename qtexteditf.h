#ifndef QTEXTEDITF_H
#define QTEXTEDITF_H

#include <QLineEdit>
#include <QObject>

class QTextEditf: public QLineEdit
{
    Q_OBJECT
public:
    QTextEditf(QWidget* parent = nullptr);
    ~QTextEditf();

signals:
   // QString editingFinished(QString);

protected:
    //void focusOutEvent(QFocusEvent *e);
};



#endif // QTEXTEDITF_H
