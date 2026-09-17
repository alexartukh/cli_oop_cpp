#include "Person.h"

using namespace System::Collections::Generic;

Person::Person(int pid, int gid, String^ fn, String^ ln)
{
	this->pid = pid;
	this->gid = gid;
	this->fn = fn;
	this->ln = ln;
	this->aList = gcnew List<Activity^>();
}

void Person::AddActivity(Activity^ a)
{
	this->aList->Add(a);
}

String^ Person::ToString()
{
	String^ report = String::Format("_P_ {0,2} (G{1}) : ", this->pid, this->gid);
	for each(Activity ^ a in this->aList)
	{
		report += String::Format("{0} ", a->ToString());
	}
	return report + Environment::NewLine;
}

int Person::CalculateSalary()
{
	return 0;
}
