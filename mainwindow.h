#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void FillTable();
    void ClearTable();
    void ShowStatus(const QString&);
    QString ProcessShowText(QByteArray &OrigFile,bool);

private slots:
    void on_CompressButton_clicked();
    void on_DeCompressButton_clicked();
    void on_OrigFetchButton_clicked();
    void on_FromTextRadioButton_clicked();
    void on_FromFileRadioButton_clicked();
    void on_plainTextEdit_textChanged();
    void on_textBrowser_textChanged();
    void on_DeCompRadioButton_clicked();
    void on_lineEdit_textEdited(const QString &arg1);
    void on_DestSaveButton_clicked();
    void on_DestFetchButton_clicked();
    void on_lineEdit_2_textEdited(const QString &arg1);
    void on_OrigSaveButton_clicked();
    void on_BrowseButton_clicked();
    void on_BrowseButton_2_clicked();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
