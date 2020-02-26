#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "passman.h"
#include "parameterparser.h"

int main(int argc, char *argv[])
{
    QString version = "1.0.0";

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

        password_manager.add_entry(entry[0], entry[1], entry[2], entry[3]);
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
