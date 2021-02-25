#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <windows.h>
#include <dbt.h>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    strWindowTitle = "IMX8MM Flash Tool V1.0";

    setWindowTitle(tr("%1").arg(strWindowTitle));

    ui->cBoxDevice->addItem(tr("EMMC"));
    ui->cBoxDevice->addItem(tr("SD/TF"));

    m_Process = new QProcess(this);
    connect(m_Process,SIGNAL(finished(int)),this,SLOT(doProcessFinished()));
    connect(m_Process,SIGNAL(readyReadStandardOutput()),this,SLOT(CmdOutput()));

    connect(ui->tbtnSelectUboot,SIGNAL(clicked(bool)),this,SLOT(doSelectBootFile()));
    connect(ui->tBtnSelectTree,SIGNAL(clicked(bool)),this,SLOT(doSelectTreeFile()));
    connect(ui->tBtnSelectKernel,SIGNAL(clicked(bool)),this,SLOT(doSelectKernelFile()));
    connect(ui->tBtnSelectRootfs,SIGNAL(clicked(bool)),this,SLOT(doSelectRootfsFile()));
    connect(ui->tBtnSelectUser,SIGNAL(clicked(bool)),this,SLOT(doSelectUserSpaceFile()));

    //connect(ui->tBtnWriteUboot,SIGNAL(clicked(bool)),this,SLOT(doWriteUbootFile()));
    //connect(ui->tBtnWriteTree,SIGNAL(clicked(bool)),this,SLOT(doWriteTreeFile()));
    //connect(ui->tBtnWriteKernel,SIGNAL(clicked(bool)),this,SLOT(doWriteKernelFile()));
    //connect(ui->tBtnWriteRootfs,SIGNAL(clicked(bool)),this,SLOT(doWriteRootfsFile()));
    //connect(ui->tBtnWriteUser,SIGNAL(clicked(bool)),this,SLOT(doWriteUserSpaceFile()));

    connect(ui->tBtnWriteAll,SIGNAL(clicked(bool)),this,SLOT(doWriteAllFile()));

    connect(ui->BtnSaveConfig,SIGNAL(clicked(bool)),this,SLOT(doSaveConfigFile()));
    connect(ui->BtnInputConfig,SIGNAL(clicked(bool)),this,SLOT(doInputConfigFile()));
}

Widget::~Widget()
{

    if(m_Process->Running)
    {
        m_Process->close();
        m_Process->deleteLater();
    }
    doDeleteTempFile();

    delete ui;
}


void Widget::doProcessFinished()
{
    doDeleteTempFile();
    ui->textEdit->append("----Execution completed, Please set to EMMC mode and restart the board.----");
}

void Widget::doDeleteTempFile()
{
     QStringList filepath;
     QFile file;
     filepath << "./scripts/emmc/write_boot_Temp.clst"
              << "./scripts/emmc/write_dtb_Temp.clst"
              << "./scripts/emmc/write_kernel_Temp.clst"
              << "./scripts/emmc/write_rootfs_Temp.clst"
              << "./scripts/emmc/write_user_file_Temp.clst"
              << "./scripts/emmc/write_all_Temp.clst"
              << "./scripts/sd/write_boot_Temp.clst"
              << "./scripts/sd/write_dtb_Temp.clst"
              << "./scripts/sd/write_kernel_Temp.clst"
              << "./scripts/sd/write_rootfs_Temp.clst"
              << "./scripts/sd/write_user_file_Temp.clst"
              << "./scripts/sd/write_all_Temp.clst";
     for(int i=0; i< filepath.count();i++)
     {
         file.setFileName(filepath[i]);
         if(file.exists() == true) file.remove();
     }
}

void Widget::CmdOutput()
{
   QByteArray abt = m_Process->readAllStandardOutput();
   QString strMsg = QString::fromLocal8Bit(abt);
   ui->textEdit->append(strMsg);
   //ui->textEdit->insertPlainText(strMsg);
   //ui->textEdit->moveCursor(QTextCursor::End);
}


void Widget::doSelectBootFile()
{

    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.*");
    ui->lineEdit_Uboot->setText(file_name);
}

void Widget::doSelectUserSpaceFile()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.*");
    ui->lineEdit_User->setText(file_name);
}

void Widget::doSelectTreeFile()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.dtb");
    ui->lineEdit_Tree->setText(file_name);
}

void Widget::doSelectKernelFile()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.*");
    ui->lineEdit_Kernel->setText(file_name);
}


void Widget::doSelectRootfsFile()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.*");
    ui->lineEdit_Rootfs->setText(file_name);
}

void Widget::doSaveConfigFile()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.cfg");

    // 写入配置文件
    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strConfig = tr("%1@%2@%3@%4@%5@%6@%7@%8@%9@%10")
            .arg((ui->checkBox_uboot->isChecked()) ? 1 : 0)
            .arg(ui->lineEdit_Uboot->text())
            .arg((ui->checkBox_dtb->isChecked()) ? 1 : 0)
            .arg(ui->lineEdit_Tree->text())
            .arg((ui->checkBox_kernel->isChecked()) ? 1 : 0)
            .arg(ui->lineEdit_Kernel->text())
            .arg((ui->checkBox_rootfs->isChecked()) ? 1 : 0)
            .arg(ui->lineEdit_Rootfs->text())
            .arg((ui->checkBox_users->isChecked()) ? 1 : 0)
            .arg(ui->lineEdit_User->text());
    file.write(strConfig.toUtf8());
    file.close();
}

void Widget::doInputConfigFile()
{
    QString file_name = QFileDialog::getOpenFileName(nullptr,"File",".","*.cfg");

    // 读取配置文件
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strConfig = QString(file.readAll());
    file.close();

    qDebug()<<"Config read:"<<strConfig<<endl;

    QList<QString> tempList = strConfig.split('@');

    ui->checkBox_uboot->setChecked(((tempList.at(0) == "1") ? true : false));
    ui->lineEdit_Uboot->setText(tempList.at(1));

    ui->checkBox_dtb->setChecked(((tempList.at(2) == "1") ? true : false));
    ui->lineEdit_Tree->setText(tempList.at(3));

    ui->checkBox_kernel->setChecked(((tempList.at(4) == "1") ? true : false));
    ui->lineEdit_Kernel->setText(tempList.at(5));

    ui->checkBox_rootfs->setChecked(((tempList.at(6) == "1") ? true : false));
    ui->lineEdit_Rootfs->setText(tempList.at(7));

    ui->checkBox_users->setChecked(((tempList.at(8) == "1") ? true : false));
    ui->lineEdit_User->setText(tempList.at(9));
}


void Widget::doWriteUbootFile()
{
    if(ui->lineEdit_Uboot->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个Uboot文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_boot.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_boot_Temp.clst").arg(strDevicePath);

    // 读取脚本文件
    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();

    file.setFileName(strConfigTempFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_boot",ui->lineEdit_Uboot->text());
    file.write(strScript.toUtf8());
    file.close();

    QString strCmd = tr("%1 %2").arg("./bin/uuu.exe").arg(strConfigTempFileName);
    m_Process->start(strCmd);

}

void Widget::doWriteTreeFile()
{
    if(ui->lineEdit_Tree->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个设备树文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_dtb.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_dtb_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Tree->text());
    QFileInfo fileTemp(ui->lineEdit_Tree->text());
    strScript = strScript.replace("_name",fileTemp.fileName());
    file.write(strScript.toUtf8());
    file.close();

    QString strCmd = tr("%1 %2").arg("./bin/uuu.exe").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}

void Widget::doWriteKernelFile()
{
    if(ui->lineEdit_Kernel->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个内核文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_kernel.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_kernel_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_Kernel->text());
    QFileInfo fileTemp(ui->lineEdit_Kernel->text());
    strScript = strScript.replace("_name",fileTemp.fileName());
    file.write(strScript.toUtf8());
    file.close();


    QString strCmd = tr("%1 %2").arg("./bin/uuu.exe").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}

void Widget::doWriteRootfsFile()
{
    if(ui->lineEdit_Rootfs->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个文件系统!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_rootfs.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_rootfs_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file",ui->lineEdit_Rootfs->text()).replace("_boot",ui->lineEdit_Uboot->text());
    file.write(strScript.toUtf8());
    file.close();

    QString strCmd = tr("%1 %2").arg("./bin/uuu.exe").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}

void Widget::doWriteUserSpaceFile()
{
    if(ui->lineEdit_User->text() == "")
    {
        QMessageBox::critical(this,"错误","请选择一个用户文件!");
        return;
    }

    if(m_Process->Running) m_Process->close();

    QString strDevicePath=ui->cBoxDevice->currentIndex() == 0 ? "emmc" :"sd";

    QString strConfigFileName =tr("./scripts/%1/write_user_file.clst").arg(strDevicePath);
    QString strConfigTempFileName =tr("./scripts/%1/write_user_file_Temp.clst").arg(strDevicePath);


    QFile file(strConfigFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QString strScript = QString(file.readAll());
    file.close();


    file.setFileName(strConfigTempFileName);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.resize(0);// 清空
    strScript = strScript.replace("_file","@"+ui->lineEdit_User->text());
    QFileInfo fileTemp(ui->lineEdit_User->text());
    strScript = strScript.replace("_name",fileTemp.fileName());
    file.write(strScript.toUtf8());
    file.close();

    QString strCmd = tr("%1 %2").arg("./bin/uuu.exe").arg(strConfigTempFileName);
    m_Process->start(strCmd);
}

void Widget::doWriteAllFile()
{
    if(ui->lineEdit_Uboot->text() == "")
    {
        QMessageBox::critical(this,"错误","Uboot不能为空!");
        return;
    }

    if(ui->checkBox_uboot->isChecked())
    {
        doWriteUbootFile();
    }
    if(ui->checkBox_dtb->isChecked())
    {
        doWriteTreeFile();
    }
    if(ui->checkBox_kernel->isChecked())
    {
        doWriteKernelFile();
    }
    if(ui->checkBox_rootfs->isChecked())
    {
        doWriteRootfsFile();
    }
    if(ui->checkBox_users->isChecked())
    {
        doWriteUserSpaceFile();
    }
}
