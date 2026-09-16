#pragma once

#using <System.dll>
#using <System.Data.dll>

using namespace System;
using namespace System::Data::Odbc;

enum SQLRequestType
{
	SQL_SELECT_PERSONS,
	SQL_SELECT_ACTIVITIES,
	SQL_SELECT_GROUPS,
	SQL_SELECT_PROJECTS,
	SQL_REPORT1,
	SQL_REPORT2,
	SQL_SELECT_BOSS_INFO
};

public ref class DBH
{
private:
    OdbcConnection^ connection;

public:
    DBH(String^ connectionString);
    
	OdbcConnection^ GetConnection();
	void ExecuteManyStatements(String^ statements);
	void Close();

	String^ GetSQL(SQLRequestType t);
	String^ GetInitSQL();
	String^ CreateManyPersons();
	String^ CreateManyActions();
	String^ CreateProjectsAndGroups();
};
