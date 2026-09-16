#include "MainForm.h"

#include "Activity.h"
#include "Person.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Data;
using namespace System::Data::Odbc;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;

MainForm::MainForm(DBH^ db, Dictionary<String^, String^>^ cfg)
{
	dbh = db;
	config = cfg;

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
	pButton->Text = "Выьрать из БД";
	pButton->Size = System::Drawing::Size(120, 40);
	pButton->Location = Point(10, 10);
	pButton->Tag = 1;

	// Кнопка активностей
	aButton = gcnew Button();
	aButton->Text = "Выьрать из БД";
	aButton->Size = System::Drawing::Size(120, 40);
	aButton->Location = Point(10, 10);
	aButton->Tag = 2;

	// Кнопка групп
	gButton = gcnew Button();
	gButton->Text = "Выьрать из БД";
	gButton->Size = System::Drawing::Size(120, 40);
	gButton->Location = Point(10, 10);
	gButton->Tag = 3;

	// Кнопка проектов
	xButton = gcnew Button();
	xButton->Text = "Выьрать из БД";
	xButton->Size = System::Drawing::Size(120, 40);
	xButton->Location = Point(10, 10);
	xButton->Tag = 4;

	// Подписываемся на событие клика
	pButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);
	aButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);
	gButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);
	xButton->Click += gcnew EventHandler(this, &MainForm::OnDBDataButtonClick);

	// Сетка результатов — заполняет всё оставшееся место на странице.
	// Общая для всех 4 кнопок, лежит ВНЕ внутреннего TabControl.
	resultsGrid = gcnew DataGridView();
	resultsGrid->Dock = DockStyle::Fill;
	resultsGrid->ReadOnly = true;
	resultsGrid->AllowUserToAddRows = false;
	resultsGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect; // выделяем только целыми строками, не колонками/ячейками
	resultsGrid->MultiSelect = false; // за раз можно выделить только одну строку

	// Вертикальная панель слева — фиксированной ширины 200px, во всю высоту страницы.
	Panel^ leftPanel = gcnew Panel();
	leftPanel->Dock = DockStyle::Left;
	leftPanel->Width = 400;
	leftPanel->BorderStyle = BorderStyle::FixedSingle;

	// Внутри левой панели — свой TabControl на 4 вкладки,
	// на каждой из которых лежит одна из 4 кнопок.
	TabControl^ leftTabs = gcnew TabControl();
	leftTabs->Dock = DockStyle::Fill;

	TabPage^ personsTab = gcnew TabPage("Персоны");
	personsTab->Controls->Add(pButton);

	TabPage^ activitiesTab = gcnew TabPage("Активности");
	activitiesTab->Controls->Add(aButton);

	TabPage^ groupsTab = gcnew TabPage("Группы");
	groupsTab->Controls->Add(gButton);

	TabPage^ projectsTab = gcnew TabPage("Проекты");
	projectsTab->Controls->Add(xButton);

	leftTabs->TabPages->Add(personsTab);
	leftTabs->TabPages->Add(activitiesTab);
	leftTabs->TabPages->Add(groupsTab);
	leftTabs->TabPages->Add(projectsTab);

	leftPanel->Controls->Add(leftTabs);

	// Порядок важен: контрол, добавленный последним, оказывается ближе к краю
	// (занимает свою полосу первым, от исходного размера страницы).
	// Сетка (Fill) — первая, левая панель (Left) — последняя.
	mainPage->Controls->Add(resultsGrid);
	mainPage->Controls->Add(leftPanel);

	tabs->TabPages->Add(mainPage);

	// ---------- отчеты 1 ----------
	TabPage^ reportTab1 = gcnew TabPage("Отчеты по группам");

	report1Button = gcnew Button();
	report1Button->Text = "Создать отчет";
	report1Button->Size = System::Drawing::Size(220, 40);
	report1Button->Location = Point(10, 10);
	report1Button->Click += gcnew EventHandler(this, &MainForm::OnGroupReportClick);

	// Метка и поле ввода — чуть ниже кнопки
	report1InputLabel = gcnew Label();
	report1InputLabel->Text = "Значение:";
	report1InputLabel->Location = Point(10, 60);
	report1InputLabel->Size = System::Drawing::Size(90, 20);

	report1Input = gcnew TextBox();
	report1Input->Location = Point(105, 57);
	report1Input->Size = System::Drawing::Size(285, 20);

	// Панель слева — такой же ширины (400px), как leftPanel на первой вкладке
	Panel^ report1Panel = gcnew Panel();
	report1Panel->Dock = DockStyle::Left;
	report1Panel->Width = 400;
	report1Panel->Controls->Add(report1Button);
	report1Panel->Controls->Add(report1InputLabel);
	report1Panel->Controls->Add(report1Input);

	// Текстовое поле вывода — занимает всю оставшуюся площадь
	report1Output = gcnew TextBox();
	report1Output->Multiline = true;
	report1Output->ReadOnly = true;
	report1Output->ScrollBars = ScrollBars::Vertical;
	report1Output->Dock = DockStyle::Fill;

	// Порядок важен: Fill — первая, Left — последняя (см. комментарий на первой вкладке)
	reportTab1->Controls->Add(report1Output);
	reportTab1->Controls->Add(report1Panel);

	tabs->TabPages->Add(reportTab1);

	// ---------- отчеты 2 ----------
	TabPage^ reportTab2 = gcnew TabPage("Отчеты по проектам");

	report2Button = gcnew Button();
	report2Button->Text = "Создать отчет";
	report2Button->Size = System::Drawing::Size(220, 40);
	report2Button->Location = Point(10, 10);
	report2Button->Click += gcnew EventHandler(this, &MainForm::OnProjectReportClick);

	// Метка и поле ввода — чуть ниже кнопки
	report2InputLabel = gcnew Label();
	report2InputLabel->Text = "Значение:";
	report2InputLabel->Location = Point(10, 60);
	report2InputLabel->Size = System::Drawing::Size(90, 20);

	report2Input = gcnew TextBox();
	report2Input->Location = Point(105, 57);
	report2Input->Size = System::Drawing::Size(285, 20);

	// Панель слева — такой же ширины (400px), как leftPanel на первой вкладке
	Panel^ report2Panel = gcnew Panel();
	report2Panel->Dock = DockStyle::Left;
	report2Panel->Width = 400;
	report2Panel->Controls->Add(report2Button);
	report2Panel->Controls->Add(report2InputLabel);
	report2Panel->Controls->Add(report2Input);

	// Текстовое поле вывода — занимает всю оставшуюся площадь
	report2Output = gcnew TextBox();
	report2Output->Multiline = true;
	report2Output->ReadOnly = true;
	report2Output->ScrollBars = ScrollBars::Vertical;
	report2Output->Dock = DockStyle::Fill;

	// Порядок важен: Fill — первая, Left — последняя (см. комментарий на первой вкладке)
	reportTab2->Controls->Add(report2Output);
	reportTab2->Controls->Add(report2Panel);

	tabs->TabPages->Add(reportTab2);

	// ---------- закладка менеджмента ----------    
	TabPage^ managementTab = gcnew TabPage("Настройки");

	managementButton = gcnew Button();
	managementButton->Text = "Создать случайную БД";
	managementButton->Size = System::Drawing::Size(220, 40);
	managementButton->Location = Point(10, 10);

	managementButton->Click += gcnew EventHandler(this, &MainForm::OnDBInitialization);

	managementButton2 = gcnew Button();
	managementButton2->Text = "Загрузить БД из файла";
	managementButton2->Size = System::Drawing::Size(220, 40);
	managementButton2->Location = Point(10, 60);
	managementButton2->Click += gcnew EventHandler(this, &MainForm::OnManagementLoadButtonClick);

	managementButton3 = gcnew Button();
	managementButton3->Text = "Сохранить БД в файл";
	managementButton3->Size = System::Drawing::Size(220, 40);
	managementButton3->Location = Point(10, 110);
	managementButton3->Click += gcnew EventHandler(this, &MainForm::OnManagementSaveButtonClick);

	tabs->TabPages->Add(managementTab);

	managementTab->Controls->Add(managementButton);
	managementTab->Controls->Add(managementButton2);
	managementTab->Controls->Add(managementButton3);
	managementTab->Controls->Add(fileNameLabel);
	managementTab->Controls->Add(fileNameInput);

	this->Controls->Add(tabs);
}

void MainForm::OnGroupReportClick(Object^ sender, EventArgs^ e)
{
	report1Output->Clear();
	Dictionary<int, int>^ subsPerBoss = gcnew Dictionary<int, int>();
	Dictionary<int, Person^>^ allPersons = gcnew Dictionary<int, Person^>();

	try
	{
		// отдельно читаем информацию о том, сколько подчиненных у каждого начальника
		String^ sql2 = dbh->GetSQL(SQL_SELECT_BOSS_INFO);
		OdbcCommand^ cmd2 = gcnew OdbcCommand(sql2, dbh->GetConnection());
		OdbcDataReader^ reader2 = cmd2->ExecuteReader();
		
		while (reader2->Read())
		{
			int bossId = Convert::ToInt32(reader2["boss_id"]);
			int sub = Convert::ToInt32(reader2["sub"]);

			subsPerBoss[bossId] = sub;

			//report1Output->AppendText(String::Format("BOSS_ID = {0} SUB = {1} {2}", bossId, sub, Environment::NewLine));
		}

		// читаем все активности и создаем все нужные объекты
		String^ sql = dbh->GetSQL(SQL_REPORT1);
		OdbcCommand^ cmd = gcnew OdbcCommand(sql, dbh->GetConnection());
		OdbcDataReader^ reader = cmd->ExecuteReader();
		
		while (reader->Read())
		{
			int pid = Convert::ToInt32(reader["pid"]);
			int aid = Convert::ToInt32(reader["aid"]);
			int hours = Convert::ToInt32(reader["hours"]);
			int projectId = Convert::ToInt32(reader["project_id"]);
			
			// добавляем в словарь новую персону, если ее еще нет,
			// или возвращаем ссылку на существующую
			Person^ currentPerson;
			if (allPersons->ContainsKey(pid)) {
				currentPerson = allPersons[pid];
			}
			else {
				currentPerson = gcnew Person(pid);
				allPersons->Add(pid, currentPerson);
			}

			// создаем новую активность в любом случае
			Activity^ a = gcnew Activity(aid, hours, projectId);
			currentPerson->AddActivity(a);

			//report1Output->AppendText(String::Format("{0} {1} : H={2} {3}", pid, aid, hours, Environment::NewLine));
		}
	}
	catch (OdbcException^ ex)
	{
		MessageBox::Show("Ошибка: " + ex->Message);
	}

	// отчет о созданных объектах
	for each (int k in allPersons->Keys) {
		report1Output->AppendText(
			allPersons[k]->GenerateTextReport()
		);
	}
}

void MainForm::OnProjectReportClick(Object^ sender, EventArgs^ e)
{
	report2Output->AppendText("222222222222222" + Environment::NewLine);
}

void MainForm::OnDBInitialization(Object^ sender, EventArgs^ e)
{
	String^ init = dbh->GetInitSQL();
	dbh->ExecuteManyStatements(init);

	String^ init2 = dbh->CreateManyPersons(
		Convert::ToInt32(this->config["groups"]),
		Convert::ToInt32(this->config["personsPerGroup"])
	);
	dbh->ExecuteManyStatements(init2);

	String^ init3 = dbh->CreateManyActions(
		Convert::ToInt32(this->config["groups"]),
		Convert::ToInt32(this->config["projects"]),
		Convert::ToInt32(this->config["personsPerGroup"]),
		Convert::ToInt32(this->config["activitiesPerPerson"])
	);
	dbh->ExecuteManyStatements(init3);

	String^ init4 = dbh->CreateProjectsAndGroups(
		Convert::ToInt32(this->config["groups"]),
		Convert::ToInt32(this->config["projects"])
	);
	dbh->ExecuteManyStatements(init4);

	MessageBox::Show("DB has been initialized by random data");
}

void MainForm::OnManagementLoadButtonClick(Object^ sender, EventArgs^ e)
{
	OpenFileDialog^ dialog = gcnew OpenFileDialog();
	dialog->Filter = "SQL files (*.sql)|*.sql|All files (*.*)|*.*";
	dialog->Title = "Загрузить БД из файла";

	if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		ProcessStartInfo^ psi = gcnew ProcessStartInfo();
		psi->FileName = config["mysql"];
		if (config["password"] != "")
		{
			psi->Arguments = String::Format("-u {0} -p{1} {2}", config["user"], config["password"], config["database"]);
		}
		else {
			psi->Arguments = String::Format("-u {0} {1}", config["user"], config["database"]);
		}
		psi->RedirectStandardInput = true;   // сюда сами запишем содержимое файла
		psi->UseShellExecute = false;        // обязательно false для перенаправления
		psi->CreateNoWindow = true;

		Process^ process = Process::Start(psi);
		process->StandardInput->Write(File::ReadAllText(dialog->FileName));
		process->StandardInput->Close();     // сигнал mysql, что ввод закончен
		process->WaitForExit();

		MessageBox::Show("БД загружена из файла " + dialog->FileName);
	}
}

void MainForm::OnManagementSaveButtonClick(Object^ sender, EventArgs^ e)
{
	SaveFileDialog^ dialog = gcnew SaveFileDialog();
	dialog->Filter = "SQL files (*.sql)|*.sql|All files (*.*)|*.*";
	dialog->Title = "Сохранить БД в файл";

	if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		ProcessStartInfo^ psi = gcnew ProcessStartInfo();
		psi->FileName = config["mysqldump"];
		if (config["password"] != "")
		{
			psi->Arguments = String::Format("-u {0} -p{1} {2}", config["user"], config["password"], config["database"]);
		}
		else {
			psi->Arguments = String::Format("-u {0} {1}", config["user"], config["database"]);
		}
		psi->RedirectStandardOutput = true;   // перехватываем stdout, чтобы записать его в файл
		psi->UseShellExecute = false;         // обязательно false, если хотим перехватывать вывод
		psi->CreateNoWindow = true;           // не показывать окно консоли

		Process^ process = Process::Start(psi);
		String^ output = process->StandardOutput->ReadToEnd();
		process->WaitForExit();

		File::WriteAllText(dialog->FileName, output);

		MessageBox::Show("БД сохранена в файле " + dialog->FileName);
	}
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
