#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString version, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("PassMan v" + version);

    connect(ui->pushButtonUnlock, SIGNAL(clicked()), this, SLOT(decrypt_database()));
    connect(ui->pushButtonBackup, SIGNAL(clicked()), this, SLOT(backup_database()));
    connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(save_database()));
    connect(ui->pushButtonAddEntry, SIGNAL(clicked()), this, SLOT(add_entry()));
    connect(ui->pushButtonRemoveEntry, SIGNAL(clicked()), this, SLOT(remove_entry()));
    connect(ui->pushButtonGeneratePassword, SIGNAL(clicked()), this, SLOT(generate_password()));
    connect(ui->lineEditSearch, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));

    ui->lineEditEncryptionKey->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::decrypt_database()
{
    //Load and decrypt database
    password_manager.key = ui->lineEditEncryptionKey->text();

    if (!password_manager.load())
    {
        ui->labelDatabaseInfo->setText("Database could not be Loaded");
        ui->labelEncryptionInfo->setText("");
        return;
    }
    if (!password_manager.decrypt())
    {
        ui->labelDatabaseInfo->setText("Database Loaded");
        ui->labelEncryptionInfo->setText("Database could not be Decrypted");
        return;
    }

    ui->labelDatabaseInfo->setText("Database Loaded");
    ui->labelEncryptionInfo->setText("Database Decrypted");

    //Fill up the table
    std::vector<std::array<QString, 4>> database = password_manager.get_database_copy();

    ui->tableWidgetCredentials->clear();
    ui->tableWidgetCredentials->setRowCount(static_cast<int>(database.size()));
    ui->tableWidgetCredentials->setColumnCount(4);

    for (ulong i = 0; i < database.size(); i++)
    {
        for (ulong j = 0; j < database[i].size(); j++)
        {
            ui->tableWidgetCredentials->setItem(static_cast<int>(i), static_cast<int>(j), new QTableWidgetItem(database[i][j]));
        }
    }

    saved = true;

    //Enable/Disable UI elements
    ui->pushButtonUnlock->setEnabled(false);
    ui->lineEditEncryptionKey->setEnabled(false);

    ui->pushButtonBackup->setEnabled(true);
    ui->pushButtonSave->setEnabled(true);
    ui->pushButtonAddEntry->setEnabled(true);
    ui->pushButtonRemoveEntry->setEnabled(true);
    ui->pushButtonGeneratePassword->setEnabled(true);
    ui->spinBoxPasswordLength->setEnabled(true);
    ui->lineEditSearch->setEnabled(true);
    ui->tableWidgetCredentials->setEnabled(true);

    //Set UI collumn names
    ui->tableWidgetCredentials->setHorizontalHeaderLabels({"Website", "Username", "Password", "Note"});
    ui->tableWidgetCredentials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->lineEditSearch->setFocus();
}

void MainWindow::backup_database()
{
    //Backup the database if it exists
    if (password_manager.backup())
    {
        ui->labelDatabaseInfo->setText("Database Backup Complete");
    } else {
        ui->labelDatabaseInfo->setText("Database could not be Backed Up");
    }
}

void MainWindow::save_database()
{
    //Clear old database data
    password_manager.clear_database();

    std::array<QString, 4> tmp_row;
    for (int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++)
    {
        //Get the data from the row
        for (int j = 0; j < 4; j++)
        {
            tmp_row[static_cast<ulong>(j)] = ui->tableWidgetCredentials->item(i, j)->text();
        }
        //Add the row data to the database
        password_manager.add_entry(tmp_row[0], tmp_row[1], tmp_row[2], tmp_row[3]);
    }

    //Encrypt and save the database onto a drive
    password_manager.encrypt();
    password_manager.save();

    saved = true;
}

void MainWindow::add_entry()
{
    int row_count = ui->tableWidgetCredentials->rowCount();

    //Add the entry to the end of the database and scroll to the row
    ui->tableWidgetCredentials->insertRow(row_count);
    ui->tableWidgetCredentials->scrollToItem(ui->tableWidgetCredentials->takeItem(row_count, 0));

    ui->tableWidgetCredentials->setItem(row_count, 0, new QTableWidgetItem(""));
    ui->tableWidgetCredentials->setItem(row_count, 1, new QTableWidgetItem(""));
    ui->tableWidgetCredentials->setItem(row_count, 2, new QTableWidgetItem(password_manager.generate_password(password_length))); //Add generated password
    ui->tableWidgetCredentials->setItem(row_count, 3, new QTableWidgetItem(""));

    saved = false;
}

void MainWindow::remove_entry()
{
    //Get a list of selected database entries
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    if (selected.size() > 0)
    {
        saved = false;
    }

    //Get rid of item duplicates caused by multiple collumns
    int last_row = -1;
    for (int i = 0; i < selected.size(); i++)
    {
        if (last_row == selected[i]->row())
        {
            selected.removeAt(i--);
            continue;
        }
        last_row = selected[i]->row();
    }

    //Remove all selected rows
    for (QTableWidgetItem* item : selected)
    {
        ui->tableWidgetCredentials->removeRow(item->row());
    }
}

void MainWindow::generate_password()
{
    //Get a list of selected database entries
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    if (selected.size() > 0)
    {
        saved = false;
    }

    //Get rid of item duplicates caused by multiple collumns
    int last_row = -1;
    for (int i = 0; i < selected.size(); i++)
    {
        if (last_row == selected[i]->row())
        {
            selected.removeAt(i--);
            continue;
        }
        last_row = selected[i]->row();
    }

    //Generate new password for all selected rows
    for (QTableWidgetItem* item : selected)
    {
        ui->tableWidgetCredentials->setItem(item->row(), 2, new QTableWidgetItem(password_manager.generate_password(password_length)));
    }
}

void MainWindow::search(const QString &input)
{
    //Show all rows
    for (int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++)
    {
        ui->tableWidgetCredentials->showRow(i);
    }

    //End if not searching
    if (input.size() == 0)
    {
        return;
    }

    //Hide rows that don't contain searched text
    for (int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++)
    {
        if (!ui->tableWidgetCredentials->item(i, 0)->text().contains(input))
        {
            ui->tableWidgetCredentials->hideRow(i);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //Close the program if the newest database is saved, if not show a dialog
    if (saved)
    {
        event->accept();
        return;
    }

    QMessageBox exit_message_box;
    exit_message_box.setIcon(QMessageBox::Warning);
    exit_message_box.setText("You didn't save!");
    exit_message_box.setInformativeText("Do you want to save before exiting?");
    exit_message_box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    exit_message_box.setDefaultButton(QMessageBox::Yes);

    switch (exit_message_box.exec()) {
        case QMessageBox::Save:
            save_database();
            event->accept();
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        default:
            event->ignore();
            break;
    }
}

void MainWindow::check_database()
{
    if (password_manager.database_exists())
    {
        return;
    }

    //Show new database dialog
    QInputDialog input_dialog;
    input_dialog.resize(400, 200);
    input_dialog.setWindowTitle("Database not Found");
    input_dialog.setLabelText("Enter Password for your new Database:");

    //Close the dialog
    if (input_dialog.exec() == 0)
    {
        close();
        return;
    }

    QString password = input_dialog.textValue();

    if (password.length() > 0)
    {
        password_manager.key = password;
        password_manager.encrypt();
        password_manager.save();
    }

    //Recheck to see if a new database has been created
    check_database();
}

void MainWindow::on_spinBoxPasswordLength_valueChanged(int arg1)
{
    password_length = arg1;
}

void MainWindow::on_tableWidgetCredentials_itemChanged()
{
    saved = false; //Set save indicator to false if database entry has been changed
}

void MainWindow::on_tableWidgetCredentials_itemSelectionChanged()
{
    //Get a list of selected database entries
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    int count = 0;
    int last_row = -1;
    //Count the number of entries
    for (QTableWidgetItem* item : selected)
    {
        if (last_row != item->row()) count++; //Count only rows(entries) and not the collumns
        last_row = item->row();
    }

    //Set the remove entry button text according to number of selected entries
    if (count > 1)
    {
        ui->pushButtonRemoveEntry->setText("Remove Selected Entries");
    } else {
        ui->pushButtonRemoveEntry->setText("Remove Selected Entry");
    }
}
