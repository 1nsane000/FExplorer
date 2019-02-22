#include "qtablewidgetitemp.h"

QTableWidgetItemp::QTableWidgetItemp()
{

}

/*void QTableWidgetItemp::editingFinished(QString str){
    //return str;
}*/

void QTableWidgetItemp::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
    //emit editingFinished(this->text());
}
