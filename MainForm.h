#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Data.dll>

#include "DBH.h"

public ref class MainForm : public System::Windows::Forms::Form
{
private:
    System::Windows::Forms::Button^ pButton;
    System::Windows::Forms::Button^ aButton;
    System::Windows::Forms::Button^ gButton;
    System::Windows::Forms::Button^ xButton;
    System::Windows::Forms::Button^ managementButton;
    System::Windows::Forms::DataGridView^ resultsGrid;
    DBH^ dbh;

public:
    MainForm(DBH^ db);

private:
    void OnDBDataButtonClick(System::Object^ sender, System::EventArgs^ e);
    void OnDBInitialization(System::Object^ sender, System::EventArgs^ e);
};
