#include "widget.h"
#include "ui_widget.h"
#include <QWidget>
#include "qtexteditf.h"

#define FOLDER_PROPERTY 5

Widget::Widget(QWidget *parent) :
    QWidget(parent)
//  ui(new Ui::Widget)
{
    setWindowTitle("Explorer");
    driveCbox = new QComboBox;
    driveCbox->setEditable(false);
    driveCbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    auto qlist(QDir::drives());
    for(auto i: qlist){
        driveCbox->addItem(i.absoluteFilePath());
    }
    connect(driveCbox,&QComboBox::currentTextChanged,this,&Widget::driveChanged);
    currentDir = driveCbox->currentText();

    //driveCbox->addItems();

    fpathCbox = new QComboBox;
    fpathCbox->setEditable(true);
    fpathCbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    fpathCbox->setEditText(driveCbox->currentText());
    fpathCbox->setInsertPolicy(QComboBox::NoInsert);
    connect(fpathCbox->lineEdit(),&QLineEdit::returnPressed, this, &Widget::fpathChanged);

    backButton = new QPushButton();
    QPixmap px(":new/res/folder-yellow-parent-icon.png");
    QIcon bi(px);
    backButton->setIcon(bi);
    backButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(backButton,&QAbstractButton::clicked,this,&Widget::back);

    createTable();

    //test = new QLineEdit(this);
    //connect(test, &QTextEditf::editingFinished, this, [&](){ test->setText(""); });
    QGridLayout *mainView = new QGridLayout(this);
    mainView->addWidget(fpathCbox,0,0,1,4);
    mainView->addWidget(driveCbox,1,0);
    mainView->addWidget(backButton,1,1);
    //mainView->addWidget(test,1,2);
    mainView->addWidget(flistTable,2,0,1,4);
    //this->set

//ui->setupUi(this);
}

void Widget::back(){
    if(currentDir == driveCbox->currentText()){
        qInfo() << "current drive";
        return;
    }
    else {
        QString up(QFileInfo(currentDir).absolutePath());
        updateTable(up);
        qInfo() << up;
        flistTable->scrollToItem(flistTable->item(0,0));
    }
}

void Widget::driveChanged(QString str){
    updateTable(str);
    flistTable->scrollToItem(flistTable->item(0,0));
}

void Widget::fpathChanged(){
    QString file = fpathCbox->lineEdit()->text().trimmed();
    file.replace(QRegularExpression("\\\\{1,}"), "/");
    file.replace(QRegularExpression("/{1,}"), "/");
    QFileInfo finfo(file);
    if(finfo.exists()){
        if(finfo.isDir()){
            updateTable(file);
            flistTable->scrollToItem(flistTable->item(0,0));
            if(fpathCbox->findText(file, Qt::MatchFixedString) == -1)
                fpathCbox->insertItem(0,file);
        }
        else{
            QDesktopServices::openUrl(QUrl(file));
        }
    }
    else {
        QMessageBox messageBox;
        messageBox.critical(nullptr,"Error","Can't find specified file");
        messageBox.setFixedSize(500,200);
        fpathCbox->setEditText(currentDir);
    }
}


void Widget::createTable()
{
    flistTable = new QTableWidgetp(0, 6, this);
    QStringList labels;
    labels << "" << "File name" << "Date modified" << "Size";

    flistTable->setHorizontalHeaderLabels(labels);
    flistTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    flistTable->horizontalHeader()->resizeSection(0,20);
    flistTable->horizontalHeader()->resizeSection(1,200);
    flistTable->horizontalHeader()->resizeSection(2,150);
    flistTable->horizontalHeader()->resizeSection(3,100);

    flistTable->verticalHeader()->hide();
    flistTable->setColumnHidden(4,true);
    flistTable->setColumnHidden(5,true);

    flistTable->setShowGrid(false);
    flistTable->setMinimumSize(500,350);
    flistTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    flistTable->setWordWrap(false);
    flistTable->setContextMenuPolicy(Qt::CustomContextMenu);


    updateTable(driveCbox->currentText());

    connect(flistTable->horizontalHeader(), &QHeaderView::sectionClicked,
                this,&Widget::sortTable);
    connect(flistTable, &QTableWidget::cellActivated,
                this, &Widget::open);
    connect(flistTable, &QTableWidget::customContextMenuRequested,
                this, &Widget::contextMenu);
    connect(flistTable->itemDelegate(), &QAbstractItemDelegate::commitData,
            this, &Widget::onCommitData);

}



void Widget::updateTable(const QString& directoryName){
    QDir dir(directoryName);
    flistTable->setUpdatesEnabled(false);
    if(dir.exists()){
        flistTable->setRowCount(0);
        QDirIterator it(directoryName,QDir::AllEntries | QDir::NoDotAndDotDot);
        QStringList files;
        while(it.hasNext()){
            files << it.next();
        }
        files.sort();
        int i = files.count();
        flistTable->setRowCount(i);
        int row = 0;
        for(auto filePath: files){

            const QFileInfo path = filePath;
            const QString fname = path.fileName();
            //const QString fmod = QDateTime(path.metadataChangeTime()).toString(Qt::SystemLocaleShortDate);
            const qint64 size = QFileInfo(filePath).size();

            auto icon = QFileIconProvider().icon(filePath);
            //auto l = new QLabel;
            //l->setPixmap(icon.pixmap(icon.availableSizes().first()));
            QTableWidgetItem *img = new QTableWidgetItem();
            img->setData(Qt::DecorationRole, icon.pixmap(icon.availableSizes().first()));
            img->setFlags(img->flags() ^ Qt::ItemIsEditable);
            QTableWidgetItem *fileNameItem = new QTableWidgetItem();
            fileNameItem->setText(fname);
            fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

            QTableWidgetItem *dateModified = new QTableWidgetItem();
            dateModified->setFlags(dateModified->flags() ^ Qt::ItemIsEditable);
            dateModified->setData(Qt::EditRole, path.metadataChangeTime());

            QTableWidgetItem *folderProperty = new QTableWidgetItem();

            QTableWidgetItem *sizeItem = new QTableWidgetItem();
            if(size > 0){
                sizeItem->setText(QLocale().formattedDataSize(size));
                folderProperty->setData(Qt::EditRole,1);
            }
            else {
                sizeItem->setText((""));
                folderProperty->setData(Qt::EditRole,0);
            }
            folderProperty->setFlags(dateModified->flags() ^ Qt::ItemIsEditable);

            QTableWidgetItem *sizeItemInfo = new QTableWidgetItem();
            sizeItemInfo->setData(Qt::EditRole, size);
            sizeItemInfo->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

            sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);
            sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            //int row = flistTable->rowCount();
            //flistTable->insertRow(row);
            flistTable->setItem(row, 0, img);
            flistTable->setItem(row, 1, fileNameItem);
            flistTable->setItem(row, 2, dateModified);
            flistTable->setItem(row, 3, sizeItem);
            flistTable->setItem(row, 4, sizeItemInfo);
            flistTable->setItem(row++, 5, folderProperty);

        }
        flistTable->setUpdatesEnabled(true);
        currentDir = directoryName;
        fpathCbox->setEditText(currentDir);
        flistTable->sortItems(FOLDER_PROPERTY, Qt::AscendingOrder);
        qInfo() << currentDir;
    }
}

void Widget::sortTable(int index){
    //qInfo()<<index;
    if(sorder == SortOrder::DateAsc||sorder == SortOrder::SizeAsc||sorder == SortOrder::NameAsc){
        if(index == 3)
        {
            flistTable->sortItems(index+1,Qt::DescendingOrder);
            qInfo()<<"Sorting by size";
            flistTable->sortItems(FOLDER_PROPERTY, Qt::DescendingOrder);
        }
        else{
            flistTable->sortItems(index,Qt::DescendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::DescendingOrder);
        }
        switch(index)
        {
            case 1: sorder = SortOrder::NameDesc; break;
            case 2: sorder = SortOrder::DateDesc; break;
            case 3: sorder = SortOrder::SizeDesc; break;
        }

    }
    else {
        flistTable->sortItems(index,Qt::AscendingOrder);
        flistTable->sortItems(FOLDER_PROPERTY, Qt::AscendingOrder);
        switch(index)
        {
            case 1: sorder = SortOrder::NameAsc; break;
            case 2: sorder = SortOrder::DateAsc; break;
            case 3: sorder = SortOrder::SizeAsc; break;
        }
    }
    flistTable->scrollToItem(flistTable->item(0,0));
}

void Widget::open(int row, int){
    QString fileName = flistTable->item(row,1)->text();
    QString *filePath;
    if(currentDir == driveCbox->currentText())
        filePath = new QString(currentDir+fileName);
    else {
        filePath = new QString(currentDir+"/"+fileName);

    }
    qInfo() << *filePath;
    QFileInfo finfo(*filePath);
    if(finfo.isDir()){
        updateTable(*filePath);
        flistTable->scrollToItem(flistTable->item(0,0));
        qInfo() << "isdir";
    }
    else{
        QDesktopServices::openUrl(QUrl(*filePath));
        qInfo() << "file";
    }
    delete filePath;


}

void Widget::contextMenu(const QPoint& pos){

    auto item = flistTable->itemAt(pos)?flistTable->item(flistTable->itemAt(pos)->row(),1):nullptr;
    //if(!item)
    //   return;
    QMenu menu;
    QAction *open;
    QAction *copy;
    QAction *cut;
    QAction *paste;
    QAction *rename;
    QAction *del;
    QAction *action ;
    if(item)
    {
        open = menu.addAction("Open");
        copy = menu.addAction("Copy");
        cut = menu.addAction("Cut");
        paste = menu.addAction("Paste");
        rename = menu.addAction("Rename");
        del = menu.addAction("Delete");

    }
    else{
        paste = menu.addAction("Paste");
    }

    action = menu.exec(flistTable->mapToGlobal(pos));
    //qInfo()<< flistTable->item(flistTable->itemAt(pos)->row(),1)->text();

    if(action == open){
        if(item)
            Widget::open(item->row(), 0);
    }
    else if(action == copy)
    {
        copyCut(5);
    }
    else if(action == cut)
    {
        copyCut(2);
    }
    else if(action == paste)
    {
        //QFileInfo pfile(QApplication::clipboard()->text().replace(QRegularExpression("file:///"), ""));
        auto mdata = QApplication::clipboard()->mimeData();
        QByteArray pdata = mdata->data("Preferred DropEffect");
        int deffect;
        QDataStream stream(&pdata, QIODevice::ReadWrite);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream >> deffect;
        QList<QUrl> urls = mdata->urls();
        for(auto url: urls)//ovde sigurno ima bagova
        {
            QFileInfo pfile(url.toString().replace(QRegularExpression("file:///"), ""));
            //pfile.exists();
            qInfo() << url;
            if(url.fileName()=="." ||url.fileName()=="..")
                continue;
            if(pfile.isFile())
                qInfo() << QFile::copy(pfile.absoluteFilePath(),currentDir + "/"+ pfile.fileName());
            else {
                copyFolder(pfile.absoluteFilePath(), currentDir);
            }
            if(deffect == 2)
            {
                QFile::remove(pfile.absoluteFilePath());
            }
        }
        updateTable(currentDir);
        refreshTable();
        //qInfo() << pfile.exists();
        //qInfo() << mdata->urls();
        //qInfo() << deffect;
        //QByteArray
    }

    else if(action == del)
    {
        //ovo treba dopuniti da pita da li obrisati, i da brise kroz sve selektovane a ne samo trenutni selektovan
        auto reply = QMessageBox::question(this, " ",
                                           "Are you sure you want to delete the selected items?",
                                           QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::No)
            return;
        for(auto i: flistTable->selectedItems())
        {
            if(i->column() == 1)
            {
                QFileInfo info(currentDir + "/" +i->text());
                if(info.isFile())
                    QFile::remove(currentDir + "/" +i->text());
                else if(info.isDir())
                {
                    QDir d(currentDir + "/" +i->text());
                    d.removeRecursively();
                }
            }
        }
        updateTable(currentDir);
        refreshTable();
    }
    else if(action == rename)
    {
        //ovo ne valja!!!
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        itemToRename = currentDir + "/" + item->text();
        flistTable->editItem(item);
        qInfo()<<currentDir + "/" + item->text();


    }

}

void Widget::onCommitData(QWidget* lineEdit)
{
    QString strNewText = reinterpret_cast<QLineEdit*>(lineEdit)->text();
    QTableWidgetItem* item = flistTable->item(flistTable->currentRow(),1);
    item->setText(strNewText);
    QFile::rename(itemToRename, currentDir + "/" + strNewText);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    updateTable(currentDir);
    refreshTable();


}

void Widget::copyCut(int dropEffect){
    //ako je dropEffect 2 katuje, a ako je 5 kopira
    QMimeData* mdata = new QMimeData;
    QList<QUrl> urls;
    for(auto item: flistTable->selectedItems())
    {
        if(item->column() == 1)
            urls.append(QUrl::fromLocalFile(currentDir + "/" + item->text()));
    }
    mdata->setUrls(urls);
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream<<dropEffect;
    qInfo() << data;

    mdata->setData("Preferred DropEffect", data);
    qInfo() << mdata->data("Preferred DropEffect");
    QApplication::clipboard()->setMimeData(mdata);
}


void Widget::copyFolder(const QString& cfolder, const QString& dfolder)
{
    //rekurzivno kopiranje foldera
    QDir destinationFolder(dfolder);
    QDir copyFolder(cfolder);
    if(destinationFolder.absolutePath().contains(copyFolder.absolutePath()))
     {
        QMessageBox::critical(this, " ","Cannot copy a directory into it's subdirectory");
        return;
     }

    QFileInfoList list = copyFolder.entryInfoList();
    destinationFolder.mkpath(copyFolder.dirName());
    for(auto i: list) // ovo je malo prljavo sa svim continuima
    {
        qInfo() << i.filePath();
        qInfo() << dfolder +"/" + copyFolder.dirName()+ "/" + i.fileName();
        if(destinationFolder == copyFolder && i.fileName()==copyFolder.dirName())
            continue;
        if(i.fileName()=="." ||i.fileName()=="..")
            continue;
        if(i.isSymLink())
            continue;
        if(i.isDir())
        {
            Widget::copyFolder(i.filePath(), dfolder + "/" + copyFolder.dirName());
            continue;
        }
        else if(i.isFile() && i.isReadable()){
            if(QFile::exists(dfolder +"/" + copyFolder.dirName()+ "/" + i.fileName()))
            {
                auto reply = QMessageBox::question(this, " ",
                                                   "File" +i.fileName() + "already exists in "+ dfolder +"/" + copyFolder.dirName()+ "/" + i.fileName()+ ". Overwrite?",
                                                   QMessageBox::Yes|QMessageBox::No);
                if(reply == QMessageBox::Yes)
                {
                    if(QFile::copy(i.filePath(),dfolder +"/" + copyFolder.dirName()+ "/" + i.fileName()))
                        QFile::remove(i.filePath());
                    continue;
                }
                else {
                    continue;
                }

            }
            qInfo() << "copying" << QFile::copy(i.filePath(),dfolder +"/" + copyFolder.dirName()+ "/" + i.fileName());
        }
    }
}





//Ovo sluzi da ostane isti poredak ako se nesto menja
void Widget::refreshTable()
{
    switch(sorder)
    {
        case(NameAsc):
            flistTable->sortItems(1,Qt::AscendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::AscendingOrder);
        break;
        case(DateAsc):
            flistTable->sortItems(2,Qt::AscendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::AscendingOrder);
        break;
        case(SizeAsc):
            flistTable->sortItems(4,Qt::AscendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::AscendingOrder);
        break;
        case(NameDesc):
            flistTable->sortItems(1,Qt::DescendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::DescendingOrder);
        break;
        case(DateDesc):
            flistTable->sortItems(2,Qt::DescendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::DescendingOrder);
        break;
        case(SizeDesc):
            flistTable->sortItems(4,Qt::DescendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::DescendingOrder);
        break;
        default:
            flistTable->sortItems(1,Qt::AscendingOrder);
            flistTable->sortItems(FOLDER_PROPERTY, Qt::AscendingOrder);
    }
}

Widget::~Widget()
{
    delete backButton;
    delete fpathCbox;
    delete driveCbox;
    delete flistTable;
}
