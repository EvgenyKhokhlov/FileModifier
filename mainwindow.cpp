#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QIntValidator>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir inputDir(inputFolder);
    if (!inputDir.exists()) {
        ui->log->append("Input folder Not found. New one created at: " + inputDir.absolutePath());
        QDir().mkdir(inputFolder);
    }

    ui->secs->setValidator(new QIntValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseOutputButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if (dialog.exec()) {
        QStringList selectedFolders = dialog.selectedFiles();
        if (!selectedFolders.isEmpty()) {
            outputFolder = selectedFolders.first();
            ui->log->append("Output folder selected : " + outputFolder);
        }
    }
}

void MainWindow::on_startButtton_clicked()
{
    if(outputFolder == nullptr){
        ui->log->append("Choose output folder!");
        return;
    }
    if(ui->inputMask->text() == nullptr){
        ui->log->append("Enter input Mask!");
        return;
    }
    if(ui->modifyMask->text() == nullptr){
        ui->log->append("Enter modify Mask!");
        return;
    }
    if(ui->repeatCheckBox->isChecked() && ui->secs->text().isEmpty()){
        ui->log->append("Enter repeat time!");
        return;
    }

    fileMasks.append(ui->inputMask->text());

    if(ui->repeatCheckBox->isChecked()){
        ui->startButtton->setEnabled(false);
        QTimer *timer = new QTimer;
        connect(timer, &QTimer::timeout, [&](){
            findFiles(inputFolder);
        });
        timer->start(ui->secs->text().toInt() * 1000);
    }

    findFiles(inputFolder);
}

void MainWindow::findFiles(const QString &directoryPath)
{
    QDir directory(directoryPath);

    QStringList inputFileList = directory.entryList(fileMasks, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString& inputFile : inputFileList) {
        modifyFile(directory.filePath(inputFile));
    }

    QStringList subDirectories = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for( const QString& subDirectory : subDirectories){
        findFiles(directoryPath + "/" + subDirectory);
    }
}

void MainWindow::modifyFile(const QString &filePath)
{
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)){
        ui->log->append("File " + filePath + "already open!");
        return;
    }

    QByteArray inputData = inputFile.readAll();
    QByteArray outputData;
    QByteArray key = ui->modifyMask->text().toUtf8();

    for (int i = 0; i < inputData.size(); ++i) {
        outputData.append(inputData.at(i) ^ key.at(i % key.size())); // use XOR operation for file modification
    }

    saveFile(&outputData, filePath);
    inputFile.close();

    if(ui->deleteInputCheckBox->isChecked())
        inputFile.remove();
}

void MainWindow::saveFile(QByteArray *outputData, QString inputFilePath)
{
    QFile inputFile(inputFilePath);
    QFileInfo fileInfoName(inputFile.fileName());
    QString outputFilePath = outputFolder + "/" + fileInfoName.fileName();

    if(!ui->overwriteCheckBox->isChecked()){
        QString tempFileBase = outputFolder + "/" + fileInfoName.baseName();
        QString tempFilePath = outputFilePath;
        int counter = 1;

        while (QFile::exists(tempFilePath)) {
            tempFilePath = QString("%1_%2.%3").arg(tempFileBase).arg(counter).arg(fileInfoName.completeSuffix());
            counter++;
        }
        outputFilePath = tempFilePath;
    }

    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        ui->log->append("Failed to create output file: " + outputFilePath);
        return;
    }

    outputFile.write(*outputData);
    ui->log->append("File modified and saved as " + outputFile.fileName());

    outputFile.close();
}

