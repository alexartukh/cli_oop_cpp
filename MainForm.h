#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>
#using <System.Xml.dll>

#include "DBH.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Data;
using namespace System::Data::Odbc;

public ref class MainForm : public Form
{
private:
    Button^ pButton;
    Button^ aButton;
    Button^ gButton;
    Button^ xButton;
    Button^ managementButton;
    Button^ managementButton2;
    Button^ managementButton3;
    Label^ fileNameLabel;
    TextBox^ fileNameInput;
    DataGridView^ resultsGrid;
    Button^ report1Button;
    Button^ report2Button;
    TextBox^ reportOutput;
    Label^ report1InputLabel;
    TextBox^ report1Input;
    Label^ report2InputLabel;
    TextBox^ report2Input;
    DBH^ dbh;
    Dictionary<String^, String^>^ config;
    OdbcDataAdapter^ resultsAdapter;
    DataTable^ resultsTable;
    String^ resultsTableName; // имя таблицы в БД для текущей выборки — нужно, чтобы вручную сформировать UPDATE

public:
    MainForm(DBH^ db, Dictionary<String^, String^>^ cfg);

private:
    void OnDBDataButtonClick(Object^ sender, EventArgs^ e);
    void OnDBInitialization(Object^ sender, EventArgs^ e);
    void OnManagementLoadButtonClick(Object^ sender, EventArgs^ e);
    void OnManagementSaveButtonClick(Object^ sender, EventArgs^ e);
    void OnReportClick(Object^ sender, EventArgs^ e);
    void OnResultsGridCellEndEdit(Object^ sender, DataGridViewCellEventArgs^ e);
};
