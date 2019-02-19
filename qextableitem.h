#ifndef QEXTABLEITEM_H
#define QEXTABLEITEM_H

#include <QTableWidgetItem>

//template <class T>
class QExTableItem: public QTableWidgetItem
{
private:
    QVariant  data;
public:
    QExTableItem();
    QExTableItem(QVariant _data);

    const QVariant& getData() const;
//    bool operator<(const QTableWidgetItem& other) const;

};

#endif // QEXTABLEITEM_H
