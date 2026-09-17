#include "DBH.h"

using namespace System;
using namespace System::Data;
using namespace System::Data::Odbc;
using namespace System::Collections::Generic;

DBH::DBH(String^ connectionString)
{
    connection = gcnew OdbcConnection(connectionString);
    connection->Open();
}

OdbcConnection^ DBH::GetConnection()
{
    return connection;
}

void DBH::Close()
{
    if (connection != nullptr && connection->State != ConnectionState::Closed)
    {
        connection->Close();
    }
}

// это единое место, где хранятся все SQL-запросы к БД, чтобы не разбрасывать их по всему коду
String^ DBH::GetSQL(SQLRequestType t)
{
	if (t == SQL_SELECT_PERSONS)
	{
		return "SELECT * FROM persons";
	}
	else if (t == SQL_SELECT_ACTIVITIES)
	{
		return "SELECT * FROM activities";
	}
	else if (t == SQL_SELECT_GROUPS)
	{
		return "SELECT * FROM `groups`";
	}
	else if (t == SQL_SELECT_PROJECTS)
	{
		return "SELECT * FROM projects";
	}
	else if (t == SQL_SELECT_BOSS_INFO) {
		return R"(
			SELECT p.id AS boss_id, COUNT(pp.id) AS sub
			FROM persons AS p
			INNER JOIN persons AS pp ON p.id = pp.boss_id
			WHERE p.boss_id = 0
			GROUP BY 1
		)";
	}
	else if (t == SQL_REPORT1)
	{
		return R"(
			SELECT p.id AS pid, a.id AS aid, a.hours, a.project_id, p.group_id, p.firstname, p.lastname, a.type
			FROM persons AS p, activities AS a 
			WHERE a.owner = p.id AND p.status = 'ACTIVE'
			AND p.group_id = ? 
			ORDER BY 1
		)";
	}
	else if (t == SQL_REPORT2)
	{
		return R"(
			SELECT p.id AS pid, a.id AS aid, a.hours, a.project_id, p.group_id, p.firstname, p.lastname, a.type
			FROM persons AS p, activities AS a 
			WHERE a.owner = p.id AND p.status = 'ACTIVE'
			AND a.project_id = ? 
			ORDER BY 1
		)";
	}

	throw gcnew ArgumentException("This SQLRequestType is not implemented");
}

void DBH::ExecuteManyStatements(String^ statements)
{
	array<String^>^ arr = statements->Split(';');
	for each (String^ stmt in arr)
	{
		String^ trimmed = stmt->Trim();
		if (trimmed->Length == 0) continue;

		try
		{
			OdbcCommand^ cmd = gcnew OdbcCommand(trimmed, this->connection);
			cmd->ExecuteNonQuery();
		}
		catch (OdbcException^ ex)
		{
			Console::WriteLine("SQL ERROR :\n" + trimmed);
		}
	}
}

// В таблицах нет автоинкрементных ключей, чтобы можно было явно задавать id при вставке.
// Поэтому для тестовых данных нужно вручную создавать уникальные id.
String^ DBH::GetInitSQL()
{
	return R"(
		DROP TABLE IF EXISTS `activities`;
		CREATE TABLE `activities` (
			`id` int NOT NULL,
			`type` ENUM('NORMAL', 'BUSINESS_TRIP', 'OVERTIME', 'PAID_VACATION', 'NOT_PAID_VACATION', 'BENCH') NOT NULL DEFAULT 'NORMAL',
			`hours` int NOT NULL,
			`owner` int NOT NULL,
			`project_id` int NOT NULL,
			`description` text NOT NULL,
			UNIQUE KEY `unique_activity_id` (`id`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

		DROP TABLE IF EXISTS `persons`;
		CREATE TABLE `persons` (
			`id` int NOT NULL,
			`firstname` varchar(100) NOT NULL,
			`lastname` varchar(100) NOT NULL,
			`group_id` int NOT NULL,
			`status` ENUM('ACTIVE', 'INACTIVE') NOT NULL DEFAULT 'ACTIVE',
			`boss_id` int NOT NULL DEFAULT 0,
			UNIQUE KEY `unique_person_id` (`id`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

		DROP TABLE IF EXISTS `groups`;
		CREATE TABLE `groups` (
			`id` int NOT NULL,
			`groupname` varchar(100) NOT NULL,
			`description` text NOT NULL,
			UNIQUE KEY `unique_group_id` (`id`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

		DROP TABLE IF EXISTS `projects`;
		CREATE TABLE `projects` (
			`id` int NOT NULL,
			`projectname` varchar(100) NOT NULL,
			`description` text NOT NULL,
			UNIQUE KEY `unique_project_id` (`id`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
	)";
}

String^ DBH::CreateProjectsAndGroups(int groups, int projects)
{
	List<String^>^ list = gcnew List<String^>();
	
	String^ groupName = "g";
	for (int i = 1; i <= groups; i++)
	{
		list->Add(
			String::Format(
				"INSERT INTO `groups` (id, groupname, description) VALUES ({0}, '{1}', '{2}')",
				i,
				groupName,
				"This is a group number " + i
			)
		);
		groupName += "g";
	}

	String^ projectName = "p";
	for (int i = 1; i <= projects; i++)
	{
		list->Add(
			String::Format(
				"INSERT INTO `projects` (id, projectname, description) VALUES ({0}, '{1}', '{2}')",
				i,
				projectName,
				"This is a project number " + i
			)
		);
		projectName += "p";
	}
	
	return String::Join(";", list);
}

String^ DBH::CreateManyPersons(int groups,  int personsPerGroup)
{
	List<String^>^ list = gcnew List<String^>();
	Random^ rnd = gcnew Random();
	int pk = 1;
	for (int g = 1; g <= groups; g++)
	{
		int bossID = 0;
		for (int i = 1; i <= personsPerGroup; i++)
		{
			if (i == 1) {
				bossID = pk;
			}

			list->Add(
				String::Format(
					"INSERT INTO `persons` (id, firstname, lastname, group_id, status, boss_id) VALUES ({0}, '{1}', '{2}', {3}, 1, {4})",
					pk,
					"FN_" + g + "_" + rnd->Next(10000),
					"LN_" + g + "_" + rnd->Next(10000),
					g,
					i > 1 ? bossID : 0
				)
			);
			pk++;
		}
	}
	
	return String::Join(";", list);
}

// каждый юзер имеет activitiesPerPerson активностей, но они распределены рандомно между проектами
String^ DBH::CreateManyActions(int groups, int projects, int personsPerGroup, int activitiesPerPerson)
{
	List<String^>^ list = gcnew List<String^>();	
	Random^ rnd = gcnew Random();
	int pk = 1;
	for (int pid = 1; pid <= personsPerGroup * groups; pid++)
	{
		for (int i = 1; i <= activitiesPerPerson; i++)
		{
			int project = rnd->Next(projects) + 1;
			int hours = rnd->Next(5) + 1;
			list->Add(
				String::Format(
					"INSERT INTO `activities` (id, owner, hours, project_id, description) VALUES ({0}, {1}, {2}, {3}, '{4}')",
					pk,
					pid,
					hours,
					project,
					"WIP : " + hours + "h for project " + project
				)
			);
			pk++;
		}
	}

	return String::Join(";", list);
}
