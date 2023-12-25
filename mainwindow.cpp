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
#include <QtWidgets>

#include <hw/interface.h>

#include <ranges>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) //

{
    ui->setupUi(this);

    rx = new ParcelModel(this);
    tx = new ParcelModel(this);

    command = new CommandModel(this, tx);

    {
        auto piv{QSerialPortInfo::availablePorts().toVector()};
        std::ranges::sort(piv, {}, [](const QSerialPortInfo& pi) { return pi.portName().midRef(3).toUInt(); });
        for(auto&& pi : piv)
            ui->cbxPort->addItem(pi.portName());
        for(qint32& BaudRate : QSerialPortInfo::standardBaudRates())
            ui->cbxBaud->addItem(QString::number(BaudRate));
    }

    {
        auto toolBar = addToolBar("Sender");
        toolBar->setObjectName("Sender");
        toolBar->setMovable(false);

        auto action = toolBar->addAction(QIcon{}, "Send (F1)", [this] {
            QByteArray data(tx->parcel());
            setTx(data);
            emit hwi::tester->Write(data);
        });
        action->setShortcut({"F1"});

        QWidget* spitoolBarnWidget = new QWidget(toolBar);
        QHBoxLayout* spinLay = new QHBoxLayout(spitoolBarnWidget);
        spinLay->setContentsMargins(0, 0, 10, 0); // Here you set the spacing

        spinLay->addWidget(ui->cbxPort);
        spinLay->addWidget(ui->cbxBaud);
        spinLay->addWidget(ui->chbxAddress);
        spinLay->addWidget(ui->spinBoxAddress);

        toolBar->addWidget(spitoolBarnWidget);
    }

    ui->tvCommand->setModel(command);
    ui->tvCommand->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tvCommand->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
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

    connect(ui->tvCommand, &DataView::clicked, [this](const QModelIndex& index) {
        if(index.row() > -1) {
            rx->setData(&command->rxData(index.row()));
            tx->setData(&command->txData(index.row()));
            command->txData(index.row())[ui->chbxAddress->isChecked() ? 3 : 2].setValue<uint8_t>(ui->spinBoxAddress->value());
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
    connect(ui->pbClear, &QPushButton::clicked, ui->textEdit, &QTextEdit::clear);

    QMenu* fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(QIcon(), "Новый", []() {});
    fileMenu->addAction(QIcon(), "Открыть", []() {});
    fileMenu->addAction(QIcon(), "Сохранить", []() {});
    fileMenu->addAction(QIcon(), "Сохранить как", []() {});

    readSettings();
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

MainWindow::~MainWindow() {
    writeSettings();
    command->save();
    delete ui;
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("widget/geometry", ui->widget->saveGeometry());
    settings.setValue("widget/windowState", ui->widget->saveState());
    settings.setValue("cbxPort", ui->cbxPort->currentIndex());
    settings.setValue("cbxBaud", ui->cbxBaud->currentIndex());
    settings.setValue("spinBoxAddress", ui->spinBoxAddress->value());
    settings.endGroup();
}

void MainWindow::readSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    ui->widget->restoreGeometry(settings.value("widget/geometry").toByteArray());
    ui->widget->restoreState(settings.value("widget/windowState").toByteArray());
    ui->cbxPort->setCurrentIndex(settings.value("cbxPort").toInt());
    ui->cbxBaud->setCurrentIndex(settings.value("cbxBaud").toInt());
    ui->spinBoxAddress->setValue(settings.value("spinBoxAddress").toInt());
    settings.endGroup();
}


void MainWindow::on_cbxPort_currentIndexChanged(const QString& arg1) {
    if(!hwi::tester->setPortName(arg1))
        QMessageBox::critical(this, "", arg1 + ": " + hwi::tester->errorString());
}

void MainWindow::on_cbxBaud_currentIndexChanged(const QString& arg1) {
    if(!hwi::tester->setBaudRate(arg1.toInt()))
        QMessageBox::critical(this, "", ui->cbxPort->currentText() + ": " + hwi::tester->errorString());
}

void MainWindow::Error(const QString& errString, const QByteArray& data) {
    ui->textEdit->setTextColor(Qt::red);
    ui->textEdit->append("Rx " + QTime::currentTime().toString("hh:mm:ss.zzz: ") + " " + errString + " " + data.toHex().toUpper());
    ui->textEdit->append(QString::fromLocal8Bit(data.mid(5)));
    setErrorType(reinterpret_cast<const uchar*>(data.data())[4]);
}

void MainWindow::setError(const QString& errString) {
    ui->textEdit->setTextColor(Qt::red);
    ui->textEdit->append(errString);
}

void MainWindow::setRx(const QByteArray& data) {
    ui->textEdit->setTextColor(Qt::darkGreen);
    ui->textEdit->append("Rx " + QTime::currentTime().toString("hh:mm:ss.zzz:\n") + data.toHex().toUpper());
    ui->textEdit->append(QString::fromLocal8Bit(data.mid(5)));
    setErrorType(reinterpret_cast<const uchar*>(data.data())[4]);
}

void MainWindow::setTx(const QByteArray& data) {
    ui->textEdit->setTextColor(Qt::blue);
    ui->textEdit->append("Tx " + QTime::currentTime().toString("hh:mm:ss.zzz:\n") + data.toHex().toUpper());
    //ui->textEdit->append(QString::fromLocal8Bit(data.mid(5)));
}

void MainWindow::setErrorType(uchar err) {
    enum {
        BufferOverflow = 0xF0,
        WrongCommand = 0xF1,
        TextualParcel = 0xF2,
        CrcError = 0xF3
    };

    switch(err) {
    case BufferOverflow:
        ui->textEdit->append("BUFFER_OVERFLOW");
        break;
    case WrongCommand:
        ui->textEdit->append("WRONG_COMMAND");
        break;
    case TextualParcel:
        ui->textEdit->append("TEXTUAL_PARCEL");
        break;
    case CrcError:
        ui->textEdit->append("CRC_ERROR");
        break;
    default:
        return;
    }
}
