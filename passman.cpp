#include "passman.h"

passman::passman() : encryption(new QAESEncryption(QAESEncryption::AES_256, QAESEncryption::CBC)) { }

//Save Database to Disk
void passman::save() const
{
    QFile output_file(database_path);
    output_file.open(QIODevice::WriteOnly);
    output_file.write(encrypted_data);
    output_file.close();
}

//Load Database from Disk
bool passman::load()
{
    if (!database_exists())
    {
        return false;
    }

    QFile input_file(database_path);
    input_file.open(QIODevice::ReadOnly);
    encrypted_data = input_file.readAll();
    input_file.close();

    return true;
}

//Encrypt Database
void passman::encrypt()
{
    QByteArray data = QString("passwords\n").toLocal8Bit();

    //Convert Database into Byte Array
    for (std::array<QString, 4>& entry_row : decrypted_entries)
    {
        for (QString& entry_column : entry_row)
        {
            data.append((entry_column + '\t').toLocal8Bit());
        }
        data.append(QString('\n').toLocal8Bit());
    }

    //Encrypt data for every iv
    for (int i = 0; i < ivs.size(); i++)
    {
        data = encryption->encode(data, QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256), QCryptographicHash::hash((ivs[i] + key).toLocal8Bit(), QCryptographicHash::Md5));
    }
    encrypted_data = data;
}

//Decrypt Database
bool passman::decrypt()
{
    QByteArray data = encrypted_data;

    //Decrypt data for every iv
    for (int i = ivs.size() - 1; i >= 0; i--)
    {
        data = encryption->decode(data, QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256), QCryptographicHash::hash((ivs[i] + key).toLocal8Bit(), QCryptographicHash::Md5));
    }

    decrypted_entries.clear();

    //Convert Byte Array into Database
    QString stringData = QString::fromLocal8Bit(data);
    QString line = "";
    for (QChar& ch : stringData)
    {
        if (ch == '\n')
        {
            if (line == "password")
            {
                line = "";
                continue;
            }

            QStringList columns = line.split('\t');
            std::array<QString, 4> row;
            if (columns.length() == 5)
            {
                for (int i = 0; i < 4; i++)
                {
                    row[static_cast<ulong>(i)] = columns[i];
                }
                decrypted_entries.emplace_back(row);
            }

            line = "";
            continue;
        }
        line += ch;
    }

    return stringData.startsWith("password");
}

//Make Database Backup
bool passman::backup() const
{
    if (!database_exists())
    {
        return false;
    }

    QFile input_file(database_path);
    input_file.open(QIODevice::ReadOnly);
    QByteArray data = input_file.readAll();
    input_file.close();

    QFile output_file(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/credentials_backup.database");
    output_file.open(QIODevice::WriteOnly);
    output_file.write(data);
    output_file.close();

    return true;
}

void passman::add_entry(QString website_name, QString username, QString password, QString note)
{
    std::array<QString, 4> row;
    row[0] = website_name;
    row[1] = username;
    row[2] = password;
    row[3] = note;
    decrypted_entries.emplace_back(row);
}

void passman::remove_entry(int index)
{
    decrypted_entries.erase(decrypted_entries.begin() + index);
}

void passman::alter_entry(int index, QString new_website_name, QString new_username, QString new_password, QString new_note)
{
    std::array<QString, 4>& row = decrypted_entries[static_cast<ulong>(index)];
    row[0] = new_website_name;
    row[1] = new_username;
    row[2] = new_password;
    row[3] = new_note;
}

void passman::clear_database()
{
    decrypted_entries.clear();
}

std::array<QString, 4> passman::get_entry_copy(int index) const
{
    return decrypted_entries[static_cast<ulong>(index)];
}

std::vector<std::array<QString, 4>> passman::get_database_copy() const
{
    return decrypted_entries;
}

QString passman::generate_password(int length) const
{
    QRandomGenerator rand = QRandomGenerator::securelySeeded();

    QString output = "";
    for (int i = 0; i < length; i++)
    {
        output += characters[rand.bounded(0, characters.length())];
    }

    return output;
}

bool passman::database_exists() const
{
    return QFile::exists(database_path);
}
