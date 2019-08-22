#include "mymainwindow.h"
#include "ui_mymainwindow.h"


myMainWindow::myMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::myMainWindow)
{
    ui->setupUi(this);

    this->resize(1200,900);

    this->setWindowTitle("Main Window");

    QIcon mainIcon(":/image/image/rabbit.png");
    this->setWindowIcon(mainIcon);

    this->setStyleSheet("color: rgba(142,53,74,200);font-weight:bold;");

//    this->setAutoFillBackground(true);
//    QPalette palette;
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/image/pinback.jpg")));
//    this->setPalette(palette);
//    setAcceptDrops(true);

    inputPointsNum = 0;

    unit = 50;

    outputPoint.setX(0);
    outputPoint.setY(0);

    //菜单栏
    QMenuBar *mBar = menuBar();
    setMenuBar(mBar);
    mBar->setStyleSheet("background-image:url(:/image/image/back.jpg)");
    //添加菜单
    QMenu *menu = mBar->addMenu("Option");
    menu->setStyleSheet("background-image:url(:/image/image/back.jpg)");

    QAction *actSet= menu->addAction("Setting");

    //工具栏，菜单项的快捷方式
    QToolBar *toolBar = new QToolBar("toolBar",this);
    addToolBar(Qt::RightToolBarArea,toolBar);
    toolBar->setStyleSheet("background-image:url(:/image/image/back.jpg)");

    menu->addSeparator();//为了美观添加分割线
    QAction *actCommand = menu->addAction("Command");
    //工具栏添加快捷键
    toolBar->addAction(actSet);
    toolBar->addAction(actCommand);

    QIcon settingIcon(":/image/image/settingIcon.png");
    actSet->setIcon(settingIcon);
    QIcon commandIcon(":/image/image/commandIcon.png");
    actCommand->setIcon(commandIcon);

    actCommand->setEnabled(false);
    ui->commandTextEdit->hide();
    QString chooseFile = "Please Click Setting Option!";
    //状态栏
    QStatusBar *sBar = statusBar();
    QLabel *label = new QLabel(this);
    label->setText(chooseFile);
    label->setStyleSheet("font-size:40px;font-weight:bold;font-family:Calibri;");
    sBar->addWidget(label);

    ui->commandViewerlabel->hide();
    ui->commandViewerlabel->setStyleSheet("color: rgba(142,53,74,200);font-weight:bold;");
    ui->lastButton->hide();
    ui->nextButton->hide();
    ui->cleanButton->hide();
    ui->limitedCheckBox->hide();
    ui->playButton->hide();
    ui->resetButton->hide();

    settingWidget = new SettingWidget(this);
    connect(actSet,&QAction::triggered,
            [=]()
        {
            label->hide();
            //这里弹出一个子窗口
            connect(settingWidget,&SettingWidget::confirmSignal,this,&myMainWindow::setData);

            connect(settingWidget,&SettingWidget::setInputPointsNumberSignal,
                    [=](int n)
            {
                inputPointsNum = n;
                qDebug()<<"inputPointsNum = "<<inputPointsNum;
            });

            connect(settingWidget,&SettingWidget::sendInputPoint,
                    [=](QPoint p)
            {
                unsigned int size = inputPoints.size();            
                qDebug()<<"size = "<<size;
                if(size == 0)
                {
                    inputPoints.push_back(p);
                    qDebug()<<"First in main window p = "<<p;
                }
                else if(size != 0)
                {
                    bool canStore = true;
                    for(unsigned int i=0;i<size;i++)
                    {
                        if(inputPoints[i]==p || p == outputPoint)
                        {
                            qDebug()<<i<<" : inputPoints["<<i<<"] = "<<inputPoints[i];
                            qDebug()<<"p = "<<p;
                            settingWidget->sendDuplicate(true);
                            canStore = false;
                            qDebug()<<"In Main Window Duplicate : p = "<<p;
                            break;
                        }               
                    }
                    if(canStore)
                    {
                        settingWidget->sendDuplicate(false);
                        inputPoints.push_back(p);
                        qDebug()<<"In main window p = "<<p;
                    }

                }

            });
            settingWidget->exec();

            connect(settingWidget,&SettingWidget::setAgain,
                    [=]()
            {
                inputPoint.clear();
                qDebug()<<"Clear input points!";
            });

        }
    );

    connect(settingWidget,&SettingWidget::outputCheckSignal,
            [=](int x, int y)
    {
        unsigned int size = inputPoints.size();
        qDebug()<<"size = "<<size;
//        for(unsigned int i=0;i<size;i++)
//        {
//            qDebug()<<inputPoints[i];
//        }
        if(size == 0)
        {
            settingWidget->outIsOK(true);
            qDebug()<<"Input points haven't entered, so it's OK.";
        }
        else
        {
            bool flag = true;
            for(unsigned int i=0;i<size;i++)
            {
                qDebug()<<"x = "<<x<<"y = "<<y;
                qDebug()<<"inputPoints["<<i<<"]"<<inputPoints[i];


                if(inputPoints[i].x() == x && inputPoints[i].y() == y)
                {
                    flag = false;
                    qDebug()<<"Output overlap with input!";
                    break;
                }
            }
            settingWidget->outIsOK(flag);

        }
    }
        );


    connect(settingWidget,&SettingWidget::outputFinishedSignal,
            [=](int x, int y)
    {
        outputPoint.setX(x);
        outputPoint.setY(y);
        qDebug()<<"outputPoint = "<<outputPoint;
    });

    canShowMatrix = false;

    connect(settingWidget,&SettingWidget::setAllDone,
            [=]()
    {
        canShowMatrix = true;
        update();
        actCommand->setEnabled(true);
        ui->commandTextEdit->show();
        QString chooseFile = "Please Click Command Option!";
        ui->commandViewerlabel->setText(chooseFile);
        ui->commandViewerlabel->setStyleSheet("color: rgba(142,53,74,200);font-weight:bold;");
        ui->lastButton->show();
        ui->nextButton->show();
        ui->cleanButton->show();
        ui->limitedCheckBox->show();
        ui->playButton->show();
        ui->resetButton->show();

    });   
}

void myMainWindow::paintEvent(QPaintEvent *)
{
    QPainter p;//创建画家对象
    p.begin(this);//指定当前窗口为绘图设备
    p.drawPixmap(0,0,width(),height(),QPixmap(":/image/image/pinback.jpg"));
    //设置屏幕透明度
    setWindowOpacity(0.96);

    if(canShowMatrix)
    {
        //定义画笔
        QPen pen;
        pen.setWidth(5);//设置线宽
        pen.setColor(QColor(220,159,180));
        pen.setStyle(Qt::SolidLine);

        //把画笔交给画家
        p.setPen(pen);

        //创建画刷对象
        QBrush brush;
        brush.setColor(QColor(255,255,255,98));//设置颜色
        brush.setStyle(Qt::SolidPattern);//设置样式
        //把画刷交给画家
        p.setBrush(brush);
        int x = width()/2-40;
        int y = height()-70;

        //画背景矩形
        p.drawRect(20,50,x,y);

        //画矩阵框
        pen.setColor(QColor(191,103,102));
        pen.setStyle(Qt::SolidLine);
        p.setPen(pen);
        p.drawRect(70,100,x-100,y-100);

    }
    p.end();
}

void myMainWindow::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(this,"question","Do you want to quit?",QMessageBox::Yes|QMessageBox::No);
    switch(ret)
    {
        case QMessageBox::Yes:
            break;
        case QMessageBox::No:
            event->ignore();
            break;
        default:
            break;
    }
}

myMainWindow::~myMainWindow()
{
    delete ui;
}

void myMainWindow::setData(int r, int c)
{
    row = r;
    col = c;
    //data->setNumber(row, col);
    qDebug()<<"setData!"<<"row = "<<row<<" col = "<<col;
}
