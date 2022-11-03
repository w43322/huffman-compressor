#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hufftree.h"
#include "huffdata.h"

#include<QDebug>
#include<QFile>
#include<QDir>
#include<QProgressBar>
#include<QFileDialog>

#include<unordered_map>
#include<cmath>

QFile *fileptr=NULL,*destptr=NULL,*treeptr=NULL;
char *buffer;
int siz=-1;
int MODE=0;//0-file 1-text 2-decomp

std::unordered_map<char,int>ByteCnt;
std::unordered_map<char,string>HuffCode;

QByteArray CodedFile;
int BitCnt=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ShowStatus("Compress Mode - From File");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::ProcessShowText(QByteArray &OrigFile,bool DisPlayText)
{
    QString res;
    res+=OrigFile.toHex().toUpper();
    for(int i=2;i<res.length();i+=3)
        res=res.left(i)+" "+res.mid(i);
    if(res.length()==299)
        res="first 100 bytes:\n"+res;
    if(!DisPlayText)
        return res;

    QString PritableContent=OrigFile;
    int allprint=1;
    for(auto &&chr:PritableContent)
        if(!isprint(chr.toLatin1())&&!isspace(chr.toLatin1()))
        {
            allprint=0;
            break;
        }
    if(allprint)
        res+="\n-----Content-Start------------\n"+
                PritableContent+
                "\n-----Content-End--------------";
    return res;
}

void MainWindow::ClearTable()
{
    while(ui->tableWidget->rowCount())
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);
}

void MainWindow::FillTable()
{
    ClearTable();
    int MaxOccur=0,LeastOccur=siz;
    for(auto i:ByteCnt)
    {
        MaxOccur=max(MaxOccur,i.second);
        LeastOccur=min(LeastOccur,i.second);
    }
    vector<pair<int,char> >SortedByteCnt;
    for(auto i:ByteCnt)
        SortedByteCnt.push_back(make_pair(i.second,i.first));
    sort(SortedByteCnt.begin(),SortedByteCnt.end());
    for(auto it=SortedByteCnt.rbegin();it!=SortedByteCnt.rend();++it)
    {
        auto i=*it;
        int nowrow=ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nowrow);
        if(isprint(i.second))
            ui->tableWidget->setItem(nowrow,0,new QTableWidgetItem((QString)i.second));
        else
            ui->tableWidget->setItem(nowrow,0,new QTableWidgetItem("N/A"));
        ui->tableWidget->setItem(nowrow,1,new QTableWidgetItem("0x"+QStringLiteral("%1").arg((unsigned char)i.second,2,16,QLatin1Char('0')).toUpper()));
        ui->tableWidget->setItem(nowrow,2,new QTableWidgetItem(QString::fromStdString(HuffCode[i.second])));
        ui->tableWidget->setItem(nowrow,3,new QTableWidgetItem(QString::number(i.first)));
        ui->tableWidget->setItem(nowrow,4,new QTableWidgetItem(QString::number(100.0*i.first/siz,'f',4)+'%'));

        QProgressBar *PB=new QProgressBar;
        PB->setMaximum(10000*log(MaxOccur));
        PB->setMinimum(10000*log(LeastOccur)-1);
        PB->setValue(10000*log(i.first));
        PB->setTextVisible(false);
        ui->tableWidget->setCellWidget(nowrow,5,PB);
    }
}

void MainWindow::on_CompressButton_clicked()
{
    if(ui->FromTextRadioButton->isChecked())
    {
        if(siz!=-1)free(buffer);
        siz=ui->plainTextEdit->toPlainText().length();
        buffer=(char*)malloc(siz);
        qstrcpy(buffer,ui->plainTextEdit->toPlainText().toStdString().c_str());
    }
    if(siz<1)return;


    //gen huff code
    ByteCnt=CountEachByte(buffer,siz);
    HuffCode=GetHuffCode(ByteCnt);


    //fill table
    FillTable();



    //write preview
    BitCnt=Encode(CodedFile,HuffCode,buffer,siz);
    QString TextBrowserContent;
    QByteArray TempByteArray=CodedFile.mid(0,100);
    TextBrowserContent=ProcessShowText(TempByteArray,false);



    ui->textBrowser->setText(TextBrowserContent);

    ShowStatus("Compression Completed. Rate: "+
               QString::number(CodedFile.size())+
               "/"+QString::number(siz)+"="+
               QString::number(100.0*CodedFile.size()/siz,'f',2)+
               "%");
}

void MainWindow::ShowStatus(const QString &SBAR)
{
    ui->statusbar->showMessage(SBAR);
}

void MainWindow::on_OrigFetchButton_clicked()
{
    if(MODE==1)
    {
        QString Content;
        for(int i=0;i<1000;++i)
        {
            char c=arc4random();
            if(!isalpha(c)&&!isdigit(c))
                continue;
            Content+=c;
        }
        ui->plainTextEdit->setPlainText(Content);
        return;
    }
    qDebug()<<QDir::currentPath();
    QString path=ui->lineEdit->text();

    qDebug()<<"\n\n\n\n\nfile path:";
    qDebug()<<path;
    qDebug()<<"----------content-----------------------";

    //memset(buffer,0,sizeof buffer);
    if(ui->FromFileRadioButton->isChecked())
    {
        fileptr=new QFile(path);
        fileptr->open(QFileDevice::ReadOnly);
        if(!fileptr->isOpen())
        {
            ShowStatus("Failed Fetching File. Please Check File Path & Name!");
            return;
        }
        QDataStream stream(fileptr);

        if(siz!=-1)free(buffer);
        buffer=(char*)malloc(fileptr->size());
        siz=stream.readRawData(buffer,fileptr->size());


        QByteArray OrigFile;
        for(int i=0;i<min(siz,100);++i)
            OrigFile.push_back(buffer[i]);
        QString TextBrowserContent=ProcessShowText(OrigFile,true);
        ui->plainTextEdit->setPlainText(TextBrowserContent);

        fileptr->close();
    }
    else
    {
        return;
    }
    for(int i=0;i<siz;++i)
        //putchar(buffer[i]);
        qDebug("%d",buffer[i]);
    qDebug()<<"----------------------------------------";

    qDebug()<<"bytes read:"<<siz;

    ui->DestSaveButton->setEnabled(false);

    ShowStatus("File Fetched Successfully. Bytes Read: "+
               QString::number(siz));
}

void MainWindow::on_FromFileRadioButton_clicked()
{
    if(siz!=-1)
    {
        free(buffer);
        siz=-1;
    }
    if(MODE==0)return;
    ui->plainTextEdit->setPlainText(NULL);
    ui->textBrowser->setText(NULL);
    ui->OrigFetchButton->setEnabled(false);
    ui->OrigFetchButton->setText("Fetch⬇️");
    ui->OrigSaveButton->setEnabled(false);
    ui->DestFetchButton->setEnabled(false);
    ui->DestSaveButton->setEnabled(false);
    ui->plainTextEdit->setReadOnly(true);
    ui->lineEdit->setEnabled(true);
    ui->lineEdit->setText("");
    ui->BrowseButton->setEnabled(true);
    ui->BrowseButton_2->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_2->setText("");
    ui->DeCompressButton->setEnabled(false);
    MODE=0;
    ShowStatus("Compress Mode - From File");
}

void MainWindow::on_FromTextRadioButton_clicked()
{
    if(siz!=-1)
    {
        free(buffer);
        siz=-1;
    }
    if(MODE==1)return;
    ui->plainTextEdit->setPlainText(NULL);
    ui->textBrowser->setText(NULL);
    ui->OrigFetchButton->setEnabled(true);
    ui->OrigFetchButton->setText("RandomGenerate⬇️");
    ui->OrigSaveButton->setEnabled(false);
    ui->DestFetchButton->setEnabled(false);
    ui->DestSaveButton->setEnabled(false);
    ui->plainTextEdit->setReadOnly(false);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit->setText("");
    ui->BrowseButton->setEnabled(false);
    ui->BrowseButton_2->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_2->setText("");
    ui->DeCompressButton->setEnabled(false);
    MODE=1;
    ShowStatus("Compress Mode - From Text");
}

void MainWindow::on_DeCompRadioButton_clicked()
{
    if(siz!=-1)
    {
        free(buffer);
        siz=-1;
    }
    if(MODE==2)return;
    ui->plainTextEdit->setPlainText(NULL);
    ui->textBrowser->setText(NULL);
    ui->OrigFetchButton->setEnabled(false);
    ui->OrigFetchButton->setText("Fetch⬇️");
    ui->OrigSaveButton->setEnabled(false);
    ui->DestFetchButton->setEnabled(false);
    ui->DestSaveButton->setEnabled(false);
    ui->plainTextEdit->setReadOnly(true);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit->setText("");
    ui->BrowseButton->setEnabled(false);
    ui->BrowseButton_2->setEnabled(true);
    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit_2->setText("");
    ui->DeCompressButton->setEnabled(false);
    MODE=2;
    ShowStatus("DeCompress Mode");
}

void MainWindow::on_plainTextEdit_textChanged()
{
    if(ui->plainTextEdit->toPlainText()=="")
    {
        if(MODE==0)
            ui->CompressButton->setEnabled(false);
        else if(MODE==2)
            ui->OrigSaveButton->setEnabled(false);
        else if(MODE==1)
        {
            ui->CompressButton->setEnabled(false);
            ui->lineEdit_2->setText("");
        }
    }
    else
    {
        if(MODE==0)
        {
            ui->CompressButton->setEnabled(true);
            ui->textBrowser->setText(NULL);
            on_textBrowser_textChanged();
        }
        else if(MODE==2)
        {
            ui->OrigSaveButton->setEnabled(true);
        }
        else if(MODE==1)
        {
            ui->CompressButton->setEnabled(true);
            ui->textBrowser->setText(NULL);
            on_textBrowser_textChanged();
            ui->lineEdit_2->setText("PLAINTEXT.txt.hufzip");
        }
    }
}


void MainWindow::on_textBrowser_textChanged()
{
    if(ui->textBrowser->toPlainText()=="")
    {
        ClearTable();
        if(MODE==0||MODE==1)
            ui->DestSaveButton->setEnabled(false);
        else if(MODE==2)
        {
            ui->DeCompressButton->setEnabled(false);
        }
    }
    else
    {
        if(MODE==0||MODE==1)
            ui->DestSaveButton->setEnabled(true);
        else if(MODE==2)
            ui->DeCompressButton->setEnabled(true);
    }
}


void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    if(arg1=="")
    {
        ui->lineEdit_2->setText("");
        ui->OrigFetchButton->setEnabled(false);
    }
    else
    {
        ui->lineEdit_2->setText(arg1+".hufzip");
        ui->OrigFetchButton->setEnabled(true);
        ShowStatus("Ready to Fetch File.");
    }
    ui->plainTextEdit->setPlainText(NULL);
    ui->textBrowser->setText(NULL);
}

void MainWindow::on_lineEdit_2_textEdited(const QString &arg1)
{
    if(!arg1.endsWith(".hufzip")||arg1.length()<=7)
    {
        ui->lineEdit->setText("");
        ui->DestFetchButton->setEnabled(false);
        ShowStatus("Invalid Hufzip File!");
    }
    else
    {
        QString FilePath=arg1;
        FilePath=FilePath.left(FilePath.length()-7);
        int INDEX=FilePath.lastIndexOf('.');
        QString SUFFIX;
        if(INDEX!=-1)
            SUFFIX=FilePath.mid(FilePath.lastIndexOf('.'));
        ui->lineEdit->setText(FilePath+".DECOMP"+SUFFIX);
        ui->DestFetchButton->setEnabled(true);
        ShowStatus("Ready to Fetch File.");
    }
    ui->plainTextEdit->setPlainText(NULL);
    ui->textBrowser->setText(NULL);
}



void MainWindow::on_DestSaveButton_clicked()
{
    qDebug()<<QDir::currentPath();
    QString path=ui->lineEdit_2->text();

    qDebug()<<"\n\n\n\n\nfile path:";
    qDebug()<<path;
    qDebug()<<"----------content-----------------------";
    destptr=new QFile(path);
    destptr->open(QFileDevice::WriteOnly);
    QDataStream stream(destptr);

    stream<<BitCnt;
    stream.writeRawData(CodedFile,CodedFile.size());

    destptr->close();

    for(auto &&i:CodedFile)
        qDebug()<<(int)i;
    qDebug()<<"----------------------------------------";

    qDebug()<<"bytes written:"<<CodedFile.size();

    treeptr=new QFile(path+".tree");
    treeptr->open(QFileDevice::WriteOnly);

    QDataStream TreeStream(treeptr);

    TreeStream<<(int)HuffCode.size();
    for(auto &&i:HuffCode)
    {
        TreeStream<<(qint8)i.first<<QString::fromStdString(i.second);
    }
    treeptr->close();

    ShowStatus("File Saved Successfully. Bytes Written: "+
               QString::number(CodedFile.size()));

}


void MainWindow::on_DestFetchButton_clicked()
{
    qDebug()<<QDir::currentPath();
    QString path=ui->lineEdit_2->text();

    qDebug()<<"\n\n\n\n\nfile path:";
    qDebug()<<path;
    qDebug()<<"----------content-----------------------";
    destptr=new QFile(path);
    destptr->open(QFileDevice::ReadOnly);
    treeptr=new QFile(path+".tree");
    treeptr->open(QFileDevice::ReadOnly);

    if(!destptr->isOpen())
    {
        ShowStatus("Failed Fetching Compressed File. Please Check File Path & Name!");
        return;
    }
    if(!treeptr->isOpen())
    {
        ShowStatus("Corresponding '.tree' file not found!");
        return;
    }


    QDataStream stream(destptr);

    stream>>BitCnt;
    if(siz!=-1)free(buffer);
    buffer=(char*)malloc(destptr->size());
    siz=stream.readRawData(buffer,destptr->size());

    destptr->close();

    for(int i=0;i<siz;++i)
    {
        qDebug()<<(int)buffer[i];
    }
    qDebug()<<"----------------------------------------";

    qDebug()<<"bytes read:"<<siz;


    QDataStream TreeStream(treeptr);


    int TreeDicCnt;
    TreeStream>>TreeDicCnt;
    HuffCode.clear();
    while(TreeDicCnt--)
    {
        qint8 CH;
        QString CODE;
        TreeStream>>CH>>CODE;
        HuffCode[CH]=CODE.toStdString();
    }
    treeptr->close();


    //write preview
    QByteArray TempByteArr;
    QString TextBrowserContent;
    for(int i=0;i<min(siz,100);++i)
        TempByteArr.push_back(buffer[i]);
    TextBrowserContent=ProcessShowText(TempByteArr,false);
    ui->textBrowser->setText(TextBrowserContent);

    ShowStatus("File Fetched Successfully. Bytes Read: "+
               QString::number(siz));
}


void MainWindow::on_DeCompressButton_clicked()
{
    QString HuffString=Decode(buffer,siz,BitCnt);
    //qDebug()<<"decoded:"<<HuffString;
    CodedFile=DeCompress(HuffString,HuffCode);

    //gen huff code
    ByteCnt=CountEachByte(CodedFile);
    //HuffCode=GetHuffCode(ByteCnt);


    //fill table
    FillTable();

    //write preview
    QString TextBrowserContent;
    QByteArray TempByteArray=CodedFile.mid(0,100);
    TextBrowserContent+=ProcessShowText(TempByteArray,true);
    ui->plainTextEdit->setPlainText(TextBrowserContent);

    ShowStatus("DeCompression Completed. Rate: "+
               QString::number(siz)+
               "/"+QString::number(CodedFile.size())+"="+
               QString::number(100.0*siz/CodedFile.size(),'f',2)+
               "%");
}

void MainWindow::on_OrigSaveButton_clicked()
{
    qDebug()<<QDir::currentPath();
    QString path=ui->lineEdit->text();

    qDebug()<<"\n\n\n\n\nfile path:";
    qDebug()<<path;
    qDebug()<<"----------content-----------------------";
    fileptr=new QFile(path);
    fileptr->open(QFileDevice::WriteOnly);
    QDataStream stream(fileptr);

    stream.writeRawData(CodedFile,CodedFile.size());

    fileptr->close();

    for(auto &&i:CodedFile)
        qDebug()<<(int)i;
    qDebug()<<"----------------------------------------";

    qDebug()<<"bytes written:"<<CodedFile.size();

    ShowStatus("File Saved Successfully. Bytes Written: "+
               QString::number(CodedFile.size()));
}


void MainWindow::on_BrowseButton_clicked()
{
    QFileDialog *QFD=new QFileDialog();
    ui->lineEdit->setText(QFD->getOpenFileName());
    QFD->close();
    on_lineEdit_textEdited(ui->lineEdit->text());
}

void MainWindow::on_BrowseButton_2_clicked()
{
    QFileDialog *QFD=new QFileDialog();
    ui->lineEdit_2->setText(QFD->getOpenFileName());
    QFD->close();
    on_lineEdit_2_textEdited(ui->lineEdit_2->text());
}
