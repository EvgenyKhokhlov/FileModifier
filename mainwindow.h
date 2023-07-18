#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
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

private:
    QString outputFolder = nullptr;
    const QString inputFolder = "input";
    QStringList fileMasks;

private slots:
    void on_chooseOutputButton_clicked();
    void on_startButtton_clicked();

private:
    Ui::MainWindow *ui;

    void findFiles(const QString& directoryPath);
    void modifyFile(const QString& filePath);
    void saveFile(QByteArray* outputData, QString inputFilePath);
};
#endif // MAINWINDOW_H
