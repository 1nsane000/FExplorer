#ifndef QTABLEWIDGETP_H
#define QTABLEWIDGETP_H

#include <QTableWidget>
#include <QObject>

class QTableWidgetp: public QTableWidget
{
    Q_OBJECT
public:
    QTableWidgetp();
    QTableWidgetp(int row, int col, QWidget* parent);
};

#endif // QTABLEWIDGETP_H
