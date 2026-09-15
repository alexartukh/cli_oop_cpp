#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>

#include "DBH.h"

using namespace System;
using namespace System::Windows::Forms;

public ref class MainForm : public Form
{
private:
    Button^ pButton;
    Button^ aButton;
    Button^ gButton;
    Button^ xButton;
    Button^ managementButton;
    DataGridView^ resultsGrid;
    Button^ report1Button;
    Button^ report2Button;
    TextBox^ report1Output;
    TextBox^ report2Output;
    Label^ report1InputLabel;
    TextBox^ report1Input;
    Label^ report2InputLabel;
    TextBox^ report2Input;
    DBH^ dbh;

public:
    MainForm(DBH^ db);

private:
    void OnDBDataButtonClick(Object^ sender, EventArgs^ e);
    void OnDBInitialization(Object^ sender, EventArgs^ e);
    void OnGroupReportClick(Object^ sender, EventArgs^ e);
    void OnProjectReportClick(Object^ sender, EventArgs^ e);
};
