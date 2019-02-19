#include "qextableitem.h"
#include "QDateTime"

QExTableItem::QExTableItem()
{

}

QExTableItem::QExTableItem(QVariant  _data) : data(_data)
{
}

const QVariant& QExTableItem::getData() const{
    return data;
}

//bool QExTableItem::operator<(const QTableWidgetItem &other) const{
    //QExTableItem *temp = *other;
  //  return true;
//}

