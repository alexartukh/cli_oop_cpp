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
		return R"(
			SELECT p.id AS `person id`, p.firstname, p.lastname, g.groupname, s.shortname, p.boss_id
			FROM persons AS p 
			INNER JOIN statuses AS s ON p.status = s.id
			INNER JOIN `groups` AS g ON p.group_id = g.id
			ORDER BY 1
		)";
	}
	else if (t == SQL_SELECT_ACTIVITIES)
	{
		return R"(
			SELECT a.id AS `activity id`, p.firstname, p.lastname, a.hours, pr.projectname
			FROM persons AS p
			INNER JOIN activities AS a ON p.id = a.owner
			INNER JOIN projects AS pr ON pr.id = a.project_id
			ORDER BY 1
		)";
	}
	else if (t == SQL_SELECT_GROUPS)
	{
		return R"(
			SELECT id AS `group id`, groupname, description
			FROM `groups`
			ORDER BY 1
		)";
	}
	else if (t == SQL_SELECT_PROJECTS)
	{
		return R"(
			SELECT id AS 'project id', projectname, description
			FROM projects
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
		DROP TABLE IF EXISTS `statuses`;
		CREATE TABLE `statuses` (
			`id` int NOT NULL,
			`shortname` varchar(100) NOT NULL,
			`description` text NOT NULL,
			UNIQUE KEY `unique_status_id` (`id`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
		INSERT INTO `statuses` VALUES (1,'active','This person is active');
		INSERT INTO `statuses` VALUES (2,'inactive','This person is inactive');

		DROP TABLE IF EXISTS `activities`;
		CREATE TABLE `activities` (
			`id` int NOT NULL,
			`type` ENUM('NORMAL', 'BUSINESS_TRIP', 'OVERTIME') NOT NULL DEFAULT 'NORMAL',
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
			`status` int NOT NULL,
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

//
// весь код ниже инициализирует пустую БД набором тестовых записей
//

int projects = 4;
int groups = 3;
int personsPerGroup = 20;
int activitiesPerPerson = 30;

String^ DBH::CreateProjectsAndGroups()
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

String^ DBH::CreateManyPersons()
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
String^ DBH::CreateManyActions()
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
