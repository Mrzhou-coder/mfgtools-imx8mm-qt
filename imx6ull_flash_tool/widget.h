#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
private slots:
    void doProcessFinished();
    void CmdOutput();
    void doSelectBootFile();
    void doSelectTreeFile();
    void doSelectKernelFile();
    void doSelectRootfsFile();
    void doWriteUbootFile();
    void doWriteTreeFile();
    void doWriteKernelFile();
    void doWriteRootfsFile();
    void doWriteAllFile();
    void doWriteUserSpaceFile();
    void doSelectUserSpaceFile();
    void doSaveConfigFile();
    void doInputConfigFile();
private:
    Ui::Widget *ui;

    QString strWindowTitle;

    QProcess *m_Process;
    void doDeleteTempFile();
    int uuu_for_each_devices();
};

#endif // WIDGET_H
