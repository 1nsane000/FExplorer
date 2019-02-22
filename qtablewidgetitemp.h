#ifndef QTABLEWIDGETITEMP_H
#define QTABLEWIDGETITEMP_H

#include <QTableWidgetItem>
#include <QObject>

class QTableWidgetItemp: public QWidget,  public QTableWidgetItem
{
    Q_OBJECT
public:
    QTableWidgetItemp();


signals:
    void editingFinished(QString);

protected:
    void focusOutEvent(QFocusEvent *e);
};

#endif // QTABLEWIDGETITEMP_H
