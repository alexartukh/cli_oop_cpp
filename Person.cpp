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

void Person::operator + (Activity^ a)
{
	this->aList->Add(a);
}

String^ Person::ToString()
{
	String^ report = String::Format("_p_ {0,2} (G{1}) : ", this->pid, this->gid);
	for each(Activity ^ a in this->aList)
	{
		report += String::Format("{0} ", a->ToString());
	}
	return report;
}

Tuple<Decimal, String^>^ Person::CalculateMoney()
{
	Decimal base = Person::MoneyPerHour;
	Decimal result = 0;
	String^ calcLog = String::Format("_p_ {0,2} (G{1}) : ", this->pid, this->gid);
	bool isFirst = true;
	for each (Activity ^ a in this->aList)
	{
		Decimal tmp = base * a->getHours();
		if (!isFirst) {
			calcLog += " + ";
		}
		calcLog += tmp;
		result = Decimal::Add(result, tmp);
		isFirst = false;
	}
	return gcnew Tuple<Decimal, String^>(result, calcLog);
}
