#include "mainwindow.h"
#include "commandmodel.h"
#include "parcelmodel.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include <QTime>

#include <hw/interface.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , rx(new ParcelModel(this))
    , tx(new ParcelModel(this))
    , command(new CommandModel(this, tx))
{
    ui->setupUi(this);

    for (const QSerialPortInfo& PortInfo : QSerialPortInfo::availablePorts())
        ui->cbxPort->addItem(PortInfo.portName());

    for (qint32& BaudRate : QSerialPortInfo::standardBaudRates())
        ui->cbxBaud->addItem(QString::number(BaudRate));

    ui->tvCommand->setModel(command);
    ui->tvCommand->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tvCommand->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tvCommand->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tvTx->setModel(tx);
    ui->tvTx->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tvTx->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tvTx->setColumnWidth(1, 60);
    ui->tvTx->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tvTx->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tvRx->setModel(rx);
    ui->tvRx->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tvRx->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tvRx->setColumnWidth(1, 60);
    ui->tvRx->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tvRx->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tvRx->setItemDelegate(new MyItemDelegate());
    ui->tvTx->setItemDelegate(new MyItemDelegate());

    connect(ui->tvCommand, &DataView::clicked, [&](const QModelIndex& index) {
        if (index.row() > -1) {
            rx->setData(command->rxData(index.row()));
            tx->setData(command->txData(index.row()));
            ui->tvRx->resizeRowsToContents();
            ui->tvTx->resizeRowsToContents();
        }
    });

    connect(command, &CommandModel::resetParcelModel, [&]() {
        rx->setData(nullptr);
        tx->setData(nullptr);
    });

    connect(hwi::tester, &Tester::Read, rx, &ParcelModel::setParcel);
    connect(hwi::tester, &Tester::Read, this, &MainWindow::setRx);
    connect(hwi::tester, &Tester::Error, this, &MainWindow::Error);

    connect(ui->action, &QAction::triggered, [&]() {
        setTx(tx->parcel());
        emit hwi::tester->Write(tx->parcel());
    });

    connect(rx, &ParcelModel::error, this, &MainWindow::setError);

    QMenu* fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(QIcon(), "Новый", []() {});
    fileMenu->addAction(QIcon(), "Открыть", []() {});
    fileMenu->addAction(QIcon(), "Сохранить", []() {});
    fileMenu->addAction(QIcon(), "Сохранить как", []() {});

    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
    command->save();
    delete ui;
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("widget/geometry", ui->widget->saveGeometry());
    settings.setValue("widget/windowState", ui->widget->saveState());
    settings.setValue("cbxPort", ui->cbxPort->currentIndex());
    settings.setValue("cbxBaud", ui->cbxBaud->currentIndex());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    ui->widget->restoreGeometry(settings.value("widget/geometry").toByteArray());
    ui->widget->restoreState(settings.value("widget/windowState").toByteArray());
    ui->cbxPort->setCurrentIndex(settings.value("cbxPort").toInt());
    ui->cbxBaud->setCurrentIndex(settings.value("cbxBaud").toInt());
    settings.endGroup();
}

void MainWindow::on_pbSend_clicked()
{
    QByteArray data(tx->parcel());
    setTx(data);
    emit hwi::tester->Write(data);
}

void MainWindow::on_cbxPort_currentIndexChanged(const QString& arg1)
{
    if (!hwi::tester->setPortName(arg1))
        QMessageBox::critical(this, "", arg1 + ": " + hwi::tester->errorString());
}

void MainWindow::on_cbxBaud_currentIndexChanged(const QString& arg1)
{
    if (!hwi::tester->setBaudRate(arg1.toInt()))
        QMessageBox::critical(this, "", ui->cbxPort->currentText() + ": " + hwi::tester->errorString());
}

void MainWindow::Error(const QString& errString, const QByteArray& data)
{
    ui->textEdit->setTextColor(Qt::red);
    ui->textEdit->append("Rx " + QTime::currentTime().toString("hh:mm:ss.zzz: ") + " " + errString + " " + data.toHex().toUpper());
}

void MainWindow::setError(const QString& errString)
{
    ui->textEdit->setTextColor(Qt::red);
    ui->textEdit->append(errString);
}

void MainWindow::setRx(const QByteArray& data)
{
    ui->textEdit->setTextColor(Qt::darkGreen);
    ui->textEdit->append("Rx " + QTime::currentTime().toString("hh:mm:ss.zzz: ") + data.toHex().toUpper());
}

void MainWindow::setTx(const QByteArray& data)
{
    ui->textEdit->setTextColor(Qt::blue);
    ui->textEdit->append("Tx " + QTime::currentTime().toString("hh:mm:ss.zzz: ") + data.toHex().toUpper());
}
