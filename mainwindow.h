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
    void check_database(); //Shows dialog to make a new database if it doesn't exist

protected:
    passman password_manager; //Password manager object
    bool saved = true; //Indicates if all the database changes has been saved
    int password_length = 20; //Used by the password generator
    void closeEvent(QCloseEvent *event) override; //Executes when user tries to close the app

protected slots:
    void decrypt_database(); //Executes when user clicks on decrypt database button
    void backup_database(); //Executes when user clicks on backup database button
    void save_database(); //Executes when user clicks on save database button
    void add_entry(); //Executes when user clicks on add entry button
    void remove_entry(); //Executes when user clicks on remove entry button
    void generate_password(); //Executes when user clicks on generate password button
    void search(const QString &input); //Executes when user changes

private slots:
    void on_spinBoxPasswordLength_valueChanged(int arg1); //Executes when password length spinbox has been changed
    void on_tableWidgetCredentials_itemChanged(); //Executes when database entry has been changed
    void on_tableWidgetCredentials_itemSelectionChanged(); //Executes when database entry selection has been changed

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
