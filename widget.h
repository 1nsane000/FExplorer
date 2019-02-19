#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDir>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QGridLayout>
#include <QDirIterator>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QLabel>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>
#include <QMessageBox>
#include <QScrollBar>
#include <QMenu>
#include <QMimeData>
#include <QClipboard>
#include <QIcon>



class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

namespace Ui {
class Widget;
}



enum SortOrder{
    Default = 0,
    NameAsc = 1,
    NameDesc = 2,
    DateAsc = 3,
    DateDesc = 4,
    SizeAsc = 5,
    SizeDesc = 6
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void open(int row, int);
    void back();
    void driveChanged(QString str);
    void fpathChanged();
    void sortTable(int index);
    void contextMenu(const QPoint& post);
    void itemFinishedEditing(int row, int);

private:
    void createTable();
    void updateTable(const QString& file);
    void refreshTable();
    void copyFolder(const QString& cfolder, const QString& dfolder);
    void copyCut(int dropEffect);

    QPushButton *backButton;
    QComboBox *fpathCbox;
    QComboBox *driveCbox;
    QTableWidget *flistTable;
    QString currentDir;
    QString itemToRename;
    SortOrder sorder;
    //Ui::Widget *ui;
};

#endif // WIDGET_H
