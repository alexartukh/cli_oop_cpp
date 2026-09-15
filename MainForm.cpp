#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Data;
using namespace System::Data::Odbc;

MainForm::MainForm(DBH^ db)
{
	dbh = db;

	// Настройки окна
	this->Text = gcnew String("SHRMS - Простая система управления персоналом");
	this->Size = System::Drawing::Size(800, 500);
	this->StartPosition = FormStartPosition::CenterScreen;

	// Набор закладок занимает всё окно целиком
	TabControl^ tabs = gcnew TabControl();
	tabs->Dock = DockStyle::Fill;

	// ---------- Первая закладка ----------
	TabPage^ mainPage = gcnew TabPage("БД");

	// Кнопка персон
	pButton = gcnew Button();
	pButton->Text = "Персоны";
	pButton->Size = System::Drawing::Size(120, 40);
	pButton->Location = Point(10, 10);
	pButton->Tag = 1;

	// Кнопка активностей
	aButton = gcnew Button();
	aButton->Text = "Активности";
	aButton->Size = System::Drawing::Size(120, 40);
	aButton->Location = Point(130, 10);
	aButton->Tag = 2;

	// Кнопка групп
	gButton = gcnew Button();
	gButton->Text = "Группы";
	gButton->Size = System::Drawing::Size(120, 40);
	gButton->Location = Point(250, 10);
	gButton->Tag = 3;

	// Кнопка проектов
	xButton = gcnew Button();
	xButton->Text = "Проекты";
	xButton->Size = System::Drawing::Size(120, 40);
	xButton->Location = Point(370, 10);
	xButton->Tag = 4;

	// Подписываемся на событие клика
	pButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);
	aButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);
	gButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);
	xButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);

	// Панель сверху для кнопок — фиксированной высоты, прибита к верху страницы
	Panel^ buttonPanel = gcnew Panel();
	buttonPanel->Dock = DockStyle::Top;
	buttonPanel->Height = 60;
	buttonPanel->Controls->Add(pButton);
	buttonPanel->Controls->Add(aButton);
	buttonPanel->Controls->Add(gButton);
	buttonPanel->Controls->Add(xButton);

	// Сетка результатов — заполняет всё оставшееся место на странице
	// (Dock проще и надёжнее, чем вручную считать Size/Anchor от ClientSize)
	resultsGrid = gcnew DataGridView();
	resultsGrid->Dock = DockStyle::Fill;
	resultsGrid->ReadOnly = true;
	resultsGrid->AllowUserToAddRows = false;
	resultsGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect; // выделяем только целыми строками, не колонками/ячейками
	resultsGrid->MultiSelect = false; // за раз можно выделить только одну строку

	// Вертикальная панель слева — фиксированной ширины 200px, во всю высоту страницы.
	// Пока пустая, добавил рамку, чтобы было видно её границы.
	Panel^ leftPanel = gcnew Panel();
	leftPanel->Dock = DockStyle::Left;
	leftPanel->Width = 200;
	leftPanel->BorderStyle = BorderStyle::FixedSingle;

	// Порядок важен: контрол, добавленный последним, оказывается ближе к краю
	// (занимает свою полосу первым, от исходного размера страницы).
	// Сетка (Fill) — первая, кнопочная панель (Top) — вторая (внутри оставшейся
	// после leftPanel ширины), левая панель (Left) — последняя.
	mainPage->Controls->Add(resultsGrid);
	mainPage->Controls->Add(buttonPanel);
	mainPage->Controls->Add(leftPanel);

	tabs->TabPages->Add(mainPage);

	// ---------- Ещё 4 пустые закладки ----------
	tabs->TabPages->Add(gcnew TabPage("Управление группами"));
	tabs->TabPages->Add(gcnew TabPage("Управление проектами"));
	tabs->TabPages->Add(gcnew TabPage("Отчеты по группам"));
	tabs->TabPages->Add(gcnew TabPage("Отчеты по проектам"));

	// ---------- закладка менеджмента ----------    
	TabPage^ managementTab = gcnew TabPage("Настройки");

	managementButton = gcnew Button();
	managementButton->Text = "Инициализация БД";
	managementButton->Size = System::Drawing::Size(220, 40);
	managementButton->Location = Point(10, 10);

	managementButton->Click += gcnew EventHandler(this, &MainForm::OnDBInitialization);

	tabs->TabPages->Add(managementTab);

	managementTab->Controls->Add(managementButton);

	this->Controls->Add(tabs);
}

void MainForm::OnDBInitialization(System::Object^ sender, System::EventArgs^ e)
{
	String^ init = dbh->GetInitSQL();
	dbh->ExecuteManyStatements(init);

	String^ init2 = dbh->CreateManyPersons();
	dbh->ExecuteManyStatements(init2);

	String^ init3 = dbh->CreateManyActions();
	dbh->ExecuteManyStatements(init3);

	String^ init4 = dbh->CreateProjectsAndGroups();
	dbh->ExecuteManyStatements(init4);

	MessageBox::Show("DB initialization has been finished");
}

void MainForm::OnDBDataButtonClick(Object^ sender, EventArgs^ e)
{
	Button^ b = safe_cast<Button^>(sender);
	String^ sql = nullptr;
	if (b->Tag->Equals(1)) sql = dbh->GetSQL(SQL_SELECT_PERSONS);
	if (b->Tag->Equals(2)) sql = dbh->GetSQL(SQL_SELECT_ACTIVITIES);
	if (b->Tag->Equals(3)) sql = dbh->GetSQL(SQL_SELECT_GROUPS);
	if (b->Tag->Equals(4)) sql = dbh->GetSQL(SQL_SELECT_PROJECTS);
	Console::WriteLine(sql);

	try
	{
		OdbcDataAdapter^ adapter = gcnew OdbcDataAdapter(sql, dbh->GetConnection());
		DataTable^ table = gcnew DataTable();
		adapter->Fill(table);

		resultsGrid->DataSource = table;
	}
	catch (OdbcException^ ex)
	{
		MessageBox::Show("Ошибка: " + ex->Message);
	}
}

