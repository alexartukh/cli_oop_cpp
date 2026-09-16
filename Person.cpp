#include "Person.h"

using namespace System::Collections::Generic;

Person::Person(int pid)
{
	this->pid = pid;
	this->aList = gcnew List<Activity^>();
}

void Person::AddActivity(Activity^ a)
{
	this->aList->Add(a);
}

String^ Person::GenerateTextReport()
{
	return "Person : " + this->pid + Environment::NewLine;
}

Decimal Person::CalculateSalary()
{
	return 0;
}
