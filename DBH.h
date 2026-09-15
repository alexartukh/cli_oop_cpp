#pragma once

#using <System.dll>
#using <System.Data.dll>

enum SQLRequestType
{
	SQL_SELECT_PERSONS,
	SQL_SELECT_ACTIVITIES,
	SQL_SELECT_GROUPS,
	SQL_SELECT_PROJECTS
};

// Класс-обёртка над соединением с БД.
// Соединение открывается один раз в конструкторе при запуске программы,
public ref class DBH
{
private:
    System::Data::Odbc::OdbcConnection^ connection;

public:
    DBH(System::String^ connectionString);
    
	System::Data::Odbc::OdbcConnection^ GetConnection();
	void ExecuteManyStatements(System::String^ statements);
	void Close();

	System::String^ GetSQL(SQLRequestType t);
	System::String^ GetInitSQL();
	System::String^ CreateManyPersons();
	System::String^ CreateManyActions();
	System::String^ CreateProjectsAndGroups();
	
};
