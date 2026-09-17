#include "PersonManager.h"

PersonManager::PersonManager(int pid, int gid, String^ fn, String^ ln) : Person(pid, gid, fn, ln)
{

}

String^ PersonManager::ToString()
{
	String^ res = this->Person::ToString();
	res = res->Replace("_p_", "*M*");
	return res;
}

Tuple<Decimal, String^>^ PersonManager::CalculateMoney()
{
	auto res = this->Person::CalculateMoney();
	// кортеж является неизменяемой структурой, 
	// поэтому если что-то нужно поменять, то нужно создавать новый кортеж
	Decimal m = res->Item1;
	String^ log = res->Item2;

	Decimal bonus = PersonManager::MoneyForManager * this->Subs;

	m = Decimal::Add(bonus, m);
	log += String::Format(" + !{0} ({1} * {2})!", bonus, PersonManager::MoneyForManager, this->Subs);

	log = log->Replace("_p_", "*M*");
	Tuple<Decimal, String^>^ newRes = gcnew Tuple<Decimal, String^>(m, log);
	return newRes;
}

int PersonManager::GetSubs()
{
	return this->Subs;
}

void PersonManager::SetSubs(int s)
{
	this->Subs = s;
}
