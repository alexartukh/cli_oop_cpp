#using <System.dll>
#using <System.Windows.Forms.dll>

#include "DBH.h"
#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Data::Odbc;
using namespace System::IO;
using namespace System::Collections::Generic;

// Простое чтение .ini файла: секции ([...]) и комментарии (;...) пропускаются,
// из остального читаются пары "ключ=значение".
Dictionary<String^, String^>^ ReadIni(String^ path)
{
    Dictionary<String^, String^>^ values = gcnew Dictionary<String^, String^>();
    array<String^>^ lines = File::ReadAllLines(path);

    for each (String^ rawLine in lines)
    {
        String^ line = rawLine->Trim();

        if (line->Length == 0 || line->StartsWith(";") || line->StartsWith("["))
            continue; // пустая строка, комментарий или заголовок секции — пропускаем

        int eq = line->IndexOf('=');
        if (eq < 0)
            continue;

        String^ key = line->Substring(0, eq)->Trim();
        String^ value = line->Substring(eq + 1)->Trim();
        values[key] = value;
    }

    return values;
}

[STAThreadAttribute]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    // Параметры подключения читаем из db.ini, лежащего рядом с исходниками
    // (при запуске из Visual Studio рабочая директория по умолчанию — папка проекта).
    Dictionary<String^, String^>^ config;
    try
    {
        config = ReadIni("db.ini");
    }
    catch (Exception^ ex)
    {
        MessageBox::Show("Не удалось прочитать db.ini:\n" + ex->Message,
            "Ошибка конфигурации", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return 1;
    }

    String^ connStr = String::Format(
        "DRIVER={{MySQL ODBC 26.7 Unicode Driver}};SERVER={0};DATABASE={1};UID={2};PWD={3};",
        config["server"], config["database"], config["user"], config["password"]
    );

    // Соединение с БД устанавливается один раз, сразу при запуске программы.
    DBH^ dbh;
    try
    {
        dbh = gcnew DBH(connStr);
    }
    catch (OdbcException^ ex)
    {
        MessageBox::Show(
            "Не удалось подключиться к БД:\n" + ex->Message,
            "Ошибка подключения",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error
        );
        return 1;
    }

    Application::Run(gcnew MainForm(dbh));

    dbh->Close();
    return 0;
}
