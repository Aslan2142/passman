#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QTableWidgetItem>
#include "passman.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString version, QWidget *parent = nullptr);
    ~MainWindow() override;
    void check_database();

protected:
    passman password_manager;
    bool saved = true;
    int password_length = 20;
    void closeEvent(QCloseEvent *event) override;

protected slots:
    void decrypt_database();
    void backup_database();
    void save_database();
    void add_entry();
    void remove_entry();
    void generate_password();
    void search(const QString &input);

private slots:
    void on_spinBoxPasswordLength_valueChanged(int arg1);
    void on_tableWidgetCredentials_itemChanged();
    void on_tableWidgetCredentials_itemSelectionChanged();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
