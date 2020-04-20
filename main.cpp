#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "passman.h"
#include "parameterparser.h"

int main(int argc, char *argv[])
{
    QString version = "1.0.3";

    if (argc == 1)
    {
        QApplication a(argc, argv);
        MainWindow w(version);
        w.show();
        w.check_database();

        return a.exec();
    }



    //Logic for CLI

    parameterparser parameter_parser(argc, argv);
    passman password_manager;
    
    if (parameter_parser.has_parameter("help", 'h'))
    {
        std::cout << "Passman v" << version.toStdString() << " - A Simple Password Manager with AES-256 Encryption by Aslan2142\n\n";

        std::cout << "-h | --help | Shows this Help Screen\n";
        std::cout << "-p <password> | --pass <password> | Input Password\n";
        std::cout << "-d | --create-database | Creates new Database if it doesn't already exist\n";
        std::cout << "-n <website, username, password, note> | --new <website, username, password, note> | Creates new Database Entry\n";
        std::cout << "-r <website, username> | --remove <website, username> | Removes Database Entry\n";
        std::cout << "-b | --backup | Makes Database Backup\n";
        std::cout << "-a | --show-all | Shows all Database Entries\n";
        std::cout << "-s <website> | --show <website> | Shows all Database Entries for given Website\n\n";

        std::cout << "If you don't input Password using parameter, Program will expect you to enter Password after you start Passman.\n";
        std::cout << "Database (including backups) is saved in your Documents Folder.\n";
        std::cout << "You can restore backup by renaming it.\n";
        std::cout << "Database can't be deleted using Passman from CLI for safety purposes, however you can delete it by removing the Database File." << std::endl;

        return 0;
    }

    std::string password = parameter_parser.get_value("pass", 'p');
    if (password.compare("-") == 0)
    {
        std::cin >> password;
    }
    password_manager.key = QString::fromStdString(password);

    if (parameter_parser.has_parameter("create-database", 'd'))
    {
        if (password_manager.database_exists())
        {
            std::cerr << "Error Creating Database (database already exists)" << std::endl;
            return 4;
        }

        if (password.length() > 0)
        {
            password_manager.encrypt();
            password_manager.save();
        } else {
            std::cerr << "Error Creating Database (blank password)" << std::endl;
            return 3;
        }
    }

    if (!password_manager.load())
    {
        std::cerr << "Error Loading Database" << std::endl;
        return 1;
    }
    if (!password_manager.decrypt())
    {
        std::cerr << "Error Decrypting Database" << std::endl;
        return 2;
    }

    std::string new_entry = parameter_parser.get_value("new", 'n');
    if (new_entry.compare("-") != 0)
    {
        QStringList entry = QString::fromStdString(new_entry).split(",");

        if (entry.size() != 4)
        {
            std::cerr << "Error Creating Database Entry (bad parameters)" << std::endl;
            return 5;
        }

        password_manager.add_entry(entry[0], entry[1], entry[2], entry[3]);
        password_manager.encrypt();
        password_manager.save();
    }
    
    std::string remove_entry = parameter_parser.get_value("remove", 'r');
    if (remove_entry.compare("-") != 0)
    {
        int index = -1;
        QStringList input = QString::fromStdString(remove_entry).split(",");

        if (input.size() != 2)
        {
            std::cerr << "Error Removing Database Entry (bad parameters)" << std::endl;
            return 5;
        }

        const std::vector<std::array<QString, 4>> database = password_manager.get_database_copy();
        for (ulong i = 0; i < database.size(); i++)
        {
            if (database[i][0].compare(input[0]) == 0 && database[i][1].compare(input[1]) == 0)
            {
                index = i;
                break;
            }
        }

        if (index < 0)
        {
            std::cerr << "Error Removing Database Entry (entry not Found)" << std::endl;
            return 6;
        }

        password_manager.remove_entry(index);
        password_manager.encrypt();
        password_manager.save();
    }

    if (parameter_parser.has_parameter("backup", 'b'))
    {
        password_manager.backup();
    }

    if (parameter_parser.has_parameter("show-all", 'a'))
    {
        const std::vector<std::array<QString, 4>> database = password_manager.get_database_copy();
        for (const std::array<QString, 4>& entry : database)
        {
            std::cout << "|| " << entry[0].toStdString() << " | " << entry[1].toStdString() << " | " << entry[2].toStdString() << " | " << entry[3].toStdString() << " ||" << std::endl;
        }
    }

    std::string show_website = parameter_parser.get_value("show", 's');
    if (show_website.compare("-") != 0)
    {
        const std::vector<std::array<QString, 4>> database = password_manager.get_database_copy();
        for (const std::array<QString, 4>& entry : database)
        {
            if (entry[0].toStdString().compare(show_website) != 0)
            {
                continue;
            }
            std::cout << "|| " << entry[0].toStdString() << " | " << entry[1].toStdString() << " | " << entry[2].toStdString() << " | " << entry[3].toStdString() << " ||" << std::endl;
        }
    }

    return 0;
}
