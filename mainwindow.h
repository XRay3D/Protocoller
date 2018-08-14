#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class CommandModel;
class ParcelModel;
class ParcelModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbSend_clicked();
    void on_cbxPort_currentIndexChanged(const QString& arg1);

    void on_cbxBaud_currentIndexChanged(const QString& arg1);

private:
    Ui::MainWindow* ui;

    void Error(const QString& errString, const QByteArray& data);
    void setError(const QString& errString);
    void setRx(const QByteArray& data);
    void setTx(const QByteArray& data);

    void writeSettings();
    void readSettings();
    ParcelModel* rx;
    ParcelModel* tx;
    CommandModel* command;
};

#endif // MAINWINDOW_H
