#include "MainForm.h"

#include "Activity.h"
#include "ActivityPaid.h"
#include "ActivityNotPaid.h"

#include "Person.h"
#include "PersonWorker.h"
#include "PersonManager.h"

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
	pButton->Text = "Выбрать / обновить";
	pButton->Size = System::Drawing::Size(370, 40);
	pButton->Location = Point(10, 10);
	pButton->Tag = 1;

	// Кнопка активностей
	aButton = gcnew Button();
	aButton->Text = "Выбрать / обновить";
	aButton->Size = System::Drawing::Size(370, 40);
	aButton->Location = Point(10, 10);
	aButton->Tag = 2;

	// Кнопка групп
	gButton = gcnew Button();
	gButton->Text = "Выбрать / обновить";
	gButton->Size = System::Drawing::Size(370, 40);
	gButton->Location = Point(10, 10);
	gButton->Tag = 3;

	// Кнопка проектов
	xButton = gcnew Button();
	xButton->Text = "Выбрать / обновить";
	xButton->Size = System::Drawing::Size(370, 40);
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
	resultsGrid->ReadOnly = false; // разрешаем редактирование ячеек — изменения сохраняются в БД в OnResultsGridCellEndEdit
	resultsGrid->AllowUserToAddRows = false;
	resultsGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect; // выделяем только целыми строками, не колонками/ячейками
	resultsGrid->MultiSelect = false; // за раз можно выделить только одну строку
	resultsGrid->CellEndEdit += gcnew DataGridViewCellEventHandler(this, &MainForm::OnResultsGridCellEndEdit);

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
	report1Button->Size = System::Drawing::Size(380, 40);
	report1Button->Location = Point(10, 10);
	report1Button->Click += gcnew EventHandler(this, &MainForm::OnReportClick);
	report1Button->Tag = 1;

	// Метка и поле ввода — чуть ниже кнопки
	report1InputLabel = gcnew Label();
	report1InputLabel->Text = "ID группы";
	report1InputLabel->Location = Point(10, 60);
	report1InputLabel->Size = System::Drawing::Size(90, 20);

	report1Input = gcnew TextBox();
	report1Input->Location = Point(105, 57);
	report1Input->Size = System::Drawing::Size(285, 20);
	report1Input->Text = "1";

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
	// Полное имя System::Drawing::Font обязательно: у Control (через Form)
	// есть собственное свойство Font, которое иначе перекрывает тип при поиске без квалификации
	// (та же история, что раньше была с Size и DialogResult).
	report1Output->Font = gcnew System::Drawing::Font("Consolas", 14);

	// Порядок важен: Fill — первая, Left — последняя (см. комментарий на первой вкладке)
	reportTab1->Controls->Add(report1Output);
	reportTab1->Controls->Add(report1Panel);

	tabs->TabPages->Add(reportTab1);

	// ---------- отчеты 2 ----------
	TabPage^ reportTab2 = gcnew TabPage("Отчеты по проектам");

	report2Button = gcnew Button();
	report2Button->Text = "Создать отчет";
	report2Button->Size = System::Drawing::Size(380, 40);
	report2Button->Location = Point(10, 10);
	report2Button->Click += gcnew EventHandler(this, &MainForm::OnReportClick);
	report2Button->Tag = 2;

	// Метка и поле ввода — чуть ниже кнопки
	report2InputLabel = gcnew Label();
	report2InputLabel->Text = "ID проекта";
	report2InputLabel->Location = Point(10, 60);
	report2InputLabel->Size = System::Drawing::Size(90, 20);

	report2Input = gcnew TextBox();
	report2Input->Location = Point(105, 57);
	report2Input->Size = System::Drawing::Size(285, 20);
	report2Input->Text = "1";

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
	report2Output->Font = gcnew System::Drawing::Font("Consolas", 14);

	// Порядок важен: Fill — первая, Left — последняя (см. комментарий на первой вкладке)
	reportTab2->Controls->Add(report2Output);
	reportTab2->Controls->Add(report2Panel);

	tabs->TabPages->Add(reportTab2);

	// ---------- закладка менеджмента ----------    
	TabPage^ managementTab = gcnew TabPage("Настройки");

	managementButton = gcnew Button();
	managementButton->Text = "Создать случайную БД";
	managementButton->Size = System::Drawing::Size(380, 40);
	managementButton->Location = Point(10, 10);

	managementButton->Click += gcnew EventHandler(this, &MainForm::OnDBInitialization);

	managementButton2 = gcnew Button();
	managementButton2->Text = "Загрузить БД из файла";
	managementButton2->Size = System::Drawing::Size(380, 40);
	managementButton2->Location = Point(10, 60);
	managementButton2->Click += gcnew EventHandler(this, &MainForm::OnManagementLoadButtonClick);

	managementButton3 = gcnew Button();
	managementButton3->Text = "Сохранить БД в файл";
	managementButton3->Size = System::Drawing::Size(380, 40);
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

void MainForm::OnReportClick(Object^ sender, EventArgs^ e)
{
	Button^ b = safe_cast<Button^>(sender);
	String^ sql = nullptr;
	String^ searchParam1 = nullptr;

	PersonManager::MoneyForManager = 5;
	Person::MoneyPerHour = 20;

	if (b->Tag->Equals(1)) {
		sql = dbh->GetSQL(SQL_REPORT1);
		searchParam1 = report1Input->Text;
	}
	if (b->Tag->Equals(2)) {
		sql = dbh->GetSQL(SQL_REPORT2);
		searchParam1 = report2Input->Text;
	}

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
		}

		// читаем все активности и создаем все нужные объекты	
		OdbcCommand^ cmd = gcnew OdbcCommand(sql, dbh->GetConnection());
		cmd->Parameters->AddWithValue("filter", searchParam1);
		OdbcDataReader^ reader = cmd->ExecuteReader();
		
		while (reader->Read())
		{
			int pid = Convert::ToInt32(reader["pid"]);
			int aid = Convert::ToInt32(reader["aid"]);
			int hours = Convert::ToInt32(reader["hours"]);
			int projectId = Convert::ToInt32(reader["project_id"]);
			int gid = Convert::ToInt32(reader["group_id"]);
			String^ fn = reader["firstname"]->ToString();
			String^ ln = reader["lastname"]->ToString();
			String^ type = reader["type"]->ToString();

			// добавляем в словарь новую персону, если ее еще нет,
			// или возвращаем ссылку на существующую
			Person^ currentPerson;
			if (allPersons->ContainsKey(pid)) {
				currentPerson = allPersons[pid];
			}
			else {
				if (subsPerBoss->ContainsKey(pid)) 
				{
					currentPerson = gcnew PersonManager(pid, gid, fn, ln);
					((PersonManager^)currentPerson)->SetSubs(subsPerBoss[pid]);
				}
				else
				{
					currentPerson = gcnew PersonWorker(pid, gid, fn, ln);
				}
				
				allPersons->Add(pid, currentPerson);
			}

			// создаем новую активность в любом случае
			// используем разные классы в зависимости от типа активности
			Activity^ a;
			if (type->Equals("NORMAL") || type->Equals("BUSINESS_TRIP") || type->Equals("OVERTIME") || type->Equals("PAID_VACATION")) {
				a = gcnew ActivityPaid(aid, hours, projectId);
			}
			else if (type->Equals("BENCH") || type->Equals("NOT_PAID_VACATION")) {
				a = gcnew ActivityNotPaid(aid, hours, projectId);
			}
			else {
				a = gcnew Activity(aid, hours, projectId);
			}
			
			currentPerson->AddActivity(a);
		}
	}
	catch (OdbcException^ ex)
	{
		MessageBox::Show("Ошибка: " + ex->Message);
	}

	// output

	TextBox^ tb;
	if (b->Tag->Equals(1)) tb = report1Output;
	if (b->Tag->Equals(2)) tb = report2Output;

	tb->Clear();
	for each (int k in allPersons->Keys) {
		tb->AppendText(allPersons[k]->ToString() + Environment::NewLine);
	}
	tb->AppendText(Environment::NewLine);
	Decimal total = 0;
	for each (int k in allPersons->Keys) {
		auto tuple = allPersons[k]->CalculateMoney();
		Decimal m = tuple->Item1;
		String^ log = tuple->Item2;			
			
		total = Decimal::Add(m, total);
		tb->AppendText( log + Environment::NewLine);
	}

	tb->AppendText("-----------------" + Environment::NewLine);
	tb->AppendText(total + Environment::NewLine);
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
	// Имя таблицы нужно отдельно от SQL-запроса — используем его в OnResultsGridCellEndEdit
	// для формирования UPDATE. `groups` в обратных кавычках, т.к. это зарезервированное слово MySQL.
	if (b->Tag->Equals(1)) { sql = dbh->GetSQL(SQL_SELECT_PERSONS); resultsTableName = "persons"; }
	if (b->Tag->Equals(2)) { sql = dbh->GetSQL(SQL_SELECT_ACTIVITIES); resultsTableName = "activities"; }
	if (b->Tag->Equals(3)) { sql = dbh->GetSQL(SQL_SELECT_GROUPS); resultsTableName = "`groups`"; }
	if (b->Tag->Equals(4)) { sql = dbh->GetSQL(SQL_SELECT_PROJECTS); resultsTableName = "projects"; }
	Console::WriteLine(sql);

	try
	{
		// table держим как поле класса (resultsTable), а не локальную переменную —
		// он должен быть жив к моменту CellEndEdit, где мы читаем из него
		// оригинальное (полученное из БД) значение id для WHERE.
		//
		// Раньше здесь использовался OdbcCommandBuilder для автогенерации UPDATE,
		// но ODBC-драйвер MySQL не всегда отдаёт корректную информацию о ключах,
		// из-за чего автосгенерированный UPDATE либо не находит нужную строку
		// (WHERE строится по ВСЕМ колонкам с их старыми значениями), либо не
		// применяется вовсе — без явной ошибки. Поэтому UPDATE теперь формируется
		// вручную в OnResultsGridCellEndEdit.
		resultsAdapter = gcnew OdbcDataAdapter(sql, dbh->GetConnection());

		resultsTable = gcnew DataTable();
		resultsAdapter->Fill(resultsTable);

		resultsGrid->DataSource = resultsTable;

		// id — первичный ключ, редактировать его через сетку нельзя (см. UPDATE ниже,
		// который всегда ищет строку по id — изменить сам id таким способом не получится).
		if (resultsGrid->Columns->Contains("id"))
		{
			resultsGrid->Columns["id"]->ReadOnly = true;
		}
	}
	catch (OdbcException^ ex)
	{
		MessageBox::Show("Ошибка: " + ex->Message);
	}
}

void MainForm::OnResultsGridCellEndEdit(Object^ sender, DataGridViewCellEventArgs^ e)
{
	if (resultsTable == nullptr || resultsTableName == nullptr)
	{
		return;
	}

	DataRow^ row = resultsTable->Rows[e->RowIndex];
	String^ columnName = resultsTable->Columns[e->ColumnIndex]->ColumnName;

	// На случай, если колонка id всё же оказалась редактируемой (страховка,
	// основная защита — resultsGrid->Columns["id"]->ReadOnly в OnDBDataButtonClick).
	if (columnName->Equals("id", StringComparison::OrdinalIgnoreCase))
	{
		return;
	}

	// Значение id берём из ОРИГИНАЛЬНОЙ версии строки (той, что была получена из БД),
	// а не из текущей — так WHERE всегда указывает на правильную запись, даже если
	// в этой строке уже редактировались другие поля.
	Object^ idValue = row[resultsTable->Columns["id"], DataRowVersion::Original];
	Object^ newValue = row[columnName];

	String^ sql = String::Format("UPDATE {0} SET {1} = ? WHERE id = ?", resultsTableName, columnName);

	try
	{
		OdbcCommand^ cmd = gcnew OdbcCommand(sql, dbh->GetConnection());
		cmd->Parameters->AddWithValue("value", newValue);
		cmd->Parameters->AddWithValue("id", idValue);

		int affected = cmd->ExecuteNonQuery();
		if (affected == 0)
		{
			MessageBox::Show("Запись не найдена, изменение не сохранено");
			return;
		}

		// Фиксируем строку: "оригинальное" значение id (и остальных полей) обновляется
		// до текущего, чтобы следующее редактирование этой же строки снова работало верно.
		row->AcceptChanges();
	}
	catch (OdbcException^ ex)
	{
		MessageBox::Show("Ошибка сохранения: " + ex->Message);
	}
}
