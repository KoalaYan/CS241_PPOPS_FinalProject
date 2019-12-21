#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"iostream"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //set the displayformat style
    ui->startTime->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->endTime->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->beginning->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->ending->setDisplayFormat("yyyy-MM-dd hh:mm:ss");

    //fileOperation
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    //openAction->setShortcuts(QKeySequence::Open);

    QAction *quitAction = fileMenu->addAction(tr("&Quit"));
    //quitAction->setShortcuts(QKeySequence::Quit);

    QAction *chooseAction = fileMenu->addAction(tr("&Choose a folder..."));


    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(chooseAction,&QAction::triggered,this,&MainWindow::chooseFolder);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    menuBar()->addMenu(fileMenu);
    statusBar();



    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(7);

    QStringList header;
    header<<tr("time")<<tr("lineID")<<tr("stationID")<<tr("deviceID")<<tr("status")
         <<tr("userID")<<tr("payType");
    ui->tableWidget->setHorizontalHeaderLabels(header);

    //qtchart
    chart->setTitle("Subway Station Flow Chart");   //图标的名字
    ui->widget->setChart(chart);

    //创建曲线序列

    series0->setName("inflow");   //自动添加折线名字
    series1->setName("outflow");
    chart->addSeries(series0);
    chart->addSeries(series1);


    //station_set
    for(int i = 0;i < 81;i++)
    {
        ui->station->addItem(QString::number(i,10));
    }
    //time_step_set
    for(int i = 0;i < 24;i++)
    {
        ui->hour->addItem(QString::number(i,10));
    }
    for(int i = 0;i < 60;i++)
    {
        ui->minite->addItem(QString::number(i,10));
    }
    for(int i = 0;i < 60;i++)
    {
        ui->second->addItem(QString::number(i,10));
    }


    setWindowTitle(tr("Metro"));
    resize(1000, 600);
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::openFile()
{
    const QString fileName =
        QFileDialog::getOpenFileName(this, tr("Choose a data file"), "", "*.csv");
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::chooseFolder()
{
    //qDebug("choose folder\n");
    const QString srcDirPath = QFileDialog::getExistingDirectory(
                this, "choose src Directory",
                 "/");
    QDir dir(srcDirPath);

    QStringList nameFilters;
    nameFilters << "*.csv";
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(nameFilters);


    //qDebug("%s",qPrintable(srcDirPath));

    //qDebug("%d\n",dir.count());

    QStringList string_list;
    for(int i=0; i < dir.count(); i++)
    {
        QString file_name = srcDirPath + "/";  //文件名称
        file_name.append(dir[i]);
        string_list.append(file_name);
    }

    loadFiles(string_list);
    //qDebug("choose folder complete...\n");
}
void MainWindow::loadFile(const QString &fileName)
{
    ui->tableWidget->setColumnHidden(0, !ui->checkBox->isChecked());
    ui->tableWidget->setColumnHidden(1, !ui->checkBox_2->isChecked());
    ui->tableWidget->setColumnHidden(2, !ui->checkBox_3->isChecked());
    ui->tableWidget->setColumnHidden(3, !ui->checkBox_4->isChecked());
    ui->tableWidget->setColumnHidden(4, !ui->checkBox_5->isChecked());
    ui->tableWidget->setColumnHidden(5, !ui->checkBox_6->isChecked());
    ui->tableWidget->setColumnHidden(6, !ui->checkBox_7->isChecked());

    //qDebug("file name is %s\n",qPrintable(fileName));
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream stream(&file);


    bool flag = true;
    while (!stream.atEnd()) {
        const QString Line = stream.readLine();
        if (!Line.isEmpty()) {
            if (flag)
            {
                flag = false;
                continue;
            }

            line.append(Line);
        }
    }

    file.close();
    statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
    tableShow();
}

void MainWindow::loadFiles(const QStringList &stringlist)
{
    ui->tableWidget->setColumnHidden(0, !ui->checkBox->isChecked());
    ui->tableWidget->setColumnHidden(1, !ui->checkBox_2->isChecked());
    ui->tableWidget->setColumnHidden(2, !ui->checkBox_3->isChecked());
    ui->tableWidget->setColumnHidden(3, !ui->checkBox_4->isChecked());
    ui->tableWidget->setColumnHidden(4, !ui->checkBox_5->isChecked());
    ui->tableWidget->setColumnHidden(5, !ui->checkBox_6->isChecked());
    ui->tableWidget->setColumnHidden(6, !ui->checkBox_7->isChecked());

    //qDebug("load files with %d\n",stringlist.count());

    QString fileName;

    int i = 0;
    while(i < stringlist.count())
    {
        fileName = stringlist[i];

        i++;
        if(i > 30)
            break;

        //qDebug("file name is %s\n",qPrintable(fileName));
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text))
            continue;

        QTextStream stream(&file);
        bool flag = true;

        while (!stream.atEnd()) {
            const QString Line = stream.readLine();
            if (!Line.isEmpty()) {
                if (flag)
                {
                    flag = false;
                    continue;
                }
                line.append(Line);

            }
        }


        file.close();

        tableShow();
        qApp->processEvents();

        qDebug("file name is %s\n",qPrintable(fileName));
        statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
    }

}
void MainWindow::tableShow()
{
    int row = ui->tableWidget->rowCount();
    //qDebug("%d\n",row);

    QVector<QString>::iterator iter;
    iter = line.begin() + row;
    while(iter != line.end())
    {
        if(row == 1)
        {
            ui->tableWidget->resizeRowsToContents();
            ui->tableWidget->resizeColumnsToContents();
        }

        ui->tableWidget->insertRow(row); //插入新行

        const QStringList pieces = (*iter).split(',', QString::SkipEmptyParts);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(pieces.value(0)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(pieces.value(1)));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(pieces.value(2)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(pieces.value(3)));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(pieces.value(4)));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(pieces.value(5)));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(pieces.value(6)));

        row++;
        iter++;
    }



}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(0, !ui->checkBox->isChecked());
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(1, !ui->checkBox_2->isChecked());
}

void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(2, !ui->checkBox_3->isChecked());
}


void MainWindow::on_checkBox_4_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(3, !ui->checkBox_4->isChecked());
}

void MainWindow::on_checkBox_5_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(4, !ui->checkBox_5->isChecked());
}

void MainWindow::on_checkBox_6_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(5, !ui->checkBox_6->isChecked());
}

void MainWindow::on_checkBox_7_stateChanged(int arg1)
{
    ui->tableWidget->setColumnHidden(6, !ui->checkBox_7->isChecked());
}

void MainWindow::on_set_filter_clicked()
{

    //qDebug("set_filter_clicked\n");

    int RC = ui->tableWidget->rowCount();


    //qDebug("table row is %d",RC);

    for(int i = 0;i < RC;i++)
    {
        ui->tableWidget->setRowHidden(i,true);
    }
    //qDebug("All rows have been hidden\n");

    bool flag[7];
    flag[0] = ui->lineA->isChecked();
    flag[1] = ui->lineB->isChecked();
    flag[2] = ui->lineC->isChecked();
    flag[3] = ui->type_1->isChecked();
    flag[4] = ui->type_2->isChecked();
    flag[5] = ui->type_3->isChecked();
    flag[6] = ui->type_4->isChecked();

    QString date1 = ui->startTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString date2 = ui->endTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    for(int j = 0;j < 3;j++)
    {
        if(flag[j])
        {
            QList<QTableWidgetItem*> item = ui->tableWidget->findItems(QString(char('A'+j)),Qt::MatchContains);
            if(!item.isEmpty())
            {
                for(int k =0 ;k < item.count();k++)
                {
                    int num = item.at(k)->row();
                    if(item.at(k)->column()==1)
                    {
                        for(int m = 3;m < 7;m++)
                        {
                            if(flag[m])
                            {
                                if(ui->tableWidget->item(num,6)->text() == QString(char('0'+m-3)))
                                {
                                    if((!(QString::compare(line[num],date1) < 0 ))&&(!(QString::compare(line[num],date2) > 0 )))
                                    {
                                        ui->tableWidget->setRowHidden(num,false);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    qSort(line.begin(),line.end());

    qDebug("sort complete\n");

    //ui->tableWidget->clearContents();
    //ui->tableWidget->setRowCount(0);

    //tableShow();
    paint();

}

void MainWindow::paint()
{
    //qDebug("painting...\n");
    int hour = ui->hour->currentText().toInt();
    int minite = ui->minite->currentText().toInt();
    int second = ui->second->currentText().toInt();
    qint64 sum = hour*3600 + minite*60 + second;

    QDateTime time_1 = ui->beginning->dateTime();
    QDateTime time_2 = time_1;
    time_2 = time_2.addSecs(sum);
    QDateTime flag = ui->ending->dateTime();
    qint64 k = time_1.secsTo(flag) / sum + 1;
    //qDebug("%d\n",k);
    tim_0 = new int[k];
    tim_1 = new int[k];
    for(int i = 0;i < k;i++)
        tim_0[i] = tim_1[i] = 0;

    //qDebug("test0\n");


    qreal max = 0;

    //qDebug("%s",qPrintable((time_1.toString("yyyy-MM-dd hh:mm:ss"))));
    //qDebug("%s",qPrintable((time_2.toString("yyyy-MM-dd hh:mm:ss"))));
    QVector<QString>::iterator iter = line.begin();
    while(QString::compare(*iter,time_1.toString("yyyy-MM-dd hh:mm:ss")) < 0 )
    {
        //qDebug("%s",qPrintable((*iter)));
        iter++;
    }

    QStringList pieces;
    QString station = ui->station->currentText();

    //qDebug("test1\n");
    int RC = line.count();
    int row = 0;
    while(iter != line.end() &&
          QString::compare(time_2.toString("yyyy-MM-dd hh:mm:ss"),flag.toString("yyyy-MM-dd hh:mm:ss")) < 0)
    {
        //qDebug("in loop...\n");
        pieces = (*iter).split(',', QString::SkipEmptyParts);
        if(pieces.value(2) == station)
        {
            if((QString::compare(*iter,time_1.toString("yyyy-MM-dd hh:mm:ss")) > 0 )
                    &&(QString::compare(*iter,time_2.toString("yyyy-MM-dd hh:mm:ss")) < 0 ))
            {
                if(pieces.value(4) == QString("0"))
                {
                    tim_0[row]++;
                }
                else
                {
                    tim_1[row]++;
                }

            }
            else
            {
                bool sym = false;
                while((QString::compare(*iter,time_2.toString("yyyy-MM-dd hh:mm:ss")) > 0)
                      && !(QString::compare(time_1.toString("yyyy-MM-dd hh:mm:ss"),flag.toString("yyyy-MM-dd hh:mm:ss")) > 0))
                {
                    row++;
                    time_1 = time_2;
                    time_2 = time_2.addSecs(sum);

                }
                if(QString::compare(time_1.toString("yyyy-MM-dd hh:mm:ss"),flag.toString("yyyy-MM-dd hh:mm:ss")) > 0)
                {
                    break;
                }


                if(pieces.value(4) == QString("0"))
                {
                    tim_0[row]++;
                }
                else
                {
                    tim_1[row]++;
                }
            }
        }

        iter++;
    }
    //qDebug("test2\n");
    time_1 = ui->beginning->dateTime();
    time_1 = time_1.addSecs(sum/2);
    for(int i = 0;i < k;i++)
    {
        //qDebug("The %d point is %d and %d",i,tim_0[i],tim_1[i]);
        if(tim_0[i] > max)
            max = tim_0[i];
        if(tim_1[i] > max)
            max = tim_1[i];

        series0->append(time_1.toMSecsSinceEpoch(),tim_0[i]);
        series1->append(time_1.toMSecsSinceEpoch(),tim_1[i]);
        time_1 = time_1.addSecs(sum);
    }

    qDebug("test3\n");
    dateAxisX->setRange(ui->beginning->dateTime(), flag);//设置坐标轴范围
    dateAxisX->setTitleText("time");//标题
    dateAxisX->setFormat("MM/dd hh:mm:ss");
    dateAxisX->setTickCount(k+1); //主分隔个数
    //dateAxisX->setMinorTickCount(4); //每个单位之间绘制了多少虚网线

    axisY->setRange(0, 1.5*max);
    axisY->setTitleText("popularity");
    axisY->setLabelFormat("%d"); //标签格式
    axisY->setTickCount(10);
    //axisY->setMinorTickCount(4);

    chart->setAxisX(dateAxisX, series0);
    chart->setAxisY(axisY, series0);

    chart->setAxisX(dateAxisX, series1);
    chart->setAxisY(axisY, series1);
}

void MainWindow::on_selectAll_stateChanged(int arg1)
{
    if(ui->selectAll->isChecked())
    {
        ui->checkBox->setChecked(true);
        ui->checkBox_2->setChecked(true);
        ui->checkBox_3->setChecked(true);
        ui->checkBox_4->setChecked(true);
        ui->checkBox_5->setChecked(true);
        ui->checkBox_6->setChecked(true);
        ui->checkBox_7->setChecked(true);
    }
    else
    {
        ui->checkBox->setChecked(false);
        ui->checkBox_2->setChecked(false);
        ui->checkBox_3->setChecked(false);
        ui->checkBox_4->setChecked(false);
        ui->checkBox_5->setChecked(false);
        ui->checkBox_6->setChecked(false);
        ui->checkBox_7->setChecked(false);
    }
}
