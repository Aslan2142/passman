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
    //Load and Decrypt Database
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

    //Fill up the Table
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

    //Enable/Disable UI Elements
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

    ui->tableWidgetCredentials->setHorizontalHeaderLabels({"Website", "Username", "Password", "Note"});
    ui->tableWidgetCredentials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::backup_database()
{
    if (password_manager.backup())
    {
        ui->labelDatabaseInfo->setText("Database Backup Complete");
    } else {
        ui->labelDatabaseInfo->setText("Database could not be Backed Up");
    }
}

void MainWindow::save_database()
{
    password_manager.clear_database();

    std::array<QString, 4> tmp_row;
    for (int i = 0; i < ui->tableWidgetCredentials->rowCount(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            tmp_row[static_cast<ulong>(j)] = ui->tableWidgetCredentials->item(i, j)->text();
        }
        password_manager.add_entry(tmp_row[0], tmp_row[1], tmp_row[2], tmp_row[3]);
    }

    password_manager.encrypt();
    password_manager.save();

    saved = true;
}

void MainWindow::add_entry()
{
    int row_count = ui->tableWidgetCredentials->rowCount();

    ui->tableWidgetCredentials->insertRow(row_count);
    ui->tableWidgetCredentials->scrollToItem(ui->tableWidgetCredentials->takeItem(row_count, 0));

    ui->tableWidgetCredentials->setItem(row_count, 0, new QTableWidgetItem(""));
    ui->tableWidgetCredentials->setItem(row_count, 1, new QTableWidgetItem(""));
    ui->tableWidgetCredentials->setItem(row_count, 2, new QTableWidgetItem(password_manager.generate_password(password_length)));
    ui->tableWidgetCredentials->setItem(row_count, 3, new QTableWidgetItem(""));

    saved = false;
}

void MainWindow::remove_entry()
{
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    saved = selected.size() <= 0;

    for (QTableWidgetItem* item : selected)
    {
        ui->tableWidgetCredentials->removeRow(item->row());
    }
}

void MainWindow::generate_password()
{
    QList<QTableWidgetItem*> selected = ui->tableWidgetCredentials->selectedItems();

    saved = selected.size() <= 0;

    for (QTableWidgetItem* item : selected)
    {
        std::array<QString, 4> entry = password_manager.get_entry_copy(item->row());
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

    QInputDialog input_dialog;
    input_dialog.resize(400, 200);
    input_dialog.setWindowTitle("Database not Found");
    input_dialog.setLabelText("Enter Password for your new Database:");

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

    check_database();
}

void MainWindow::on_spinBoxPasswordLength_valueChanged(int arg1)
{
    password_length = arg1;
}

void MainWindow::on_tableWidgetCredentials_itemChanged()
{
    saved = false;
}

void MainWindow::on_tableWidgetCredentials_itemSelectionChanged()
{
    if (ui->tableWidgetCredentials->selectedItems().size() > 1)
    {
        ui->pushButtonRemoveEntry->setText("Remove Selected Entries");
    } else {
        ui->pushButtonRemoveEntry->setText("Remove Selected Entry");
    }
}
