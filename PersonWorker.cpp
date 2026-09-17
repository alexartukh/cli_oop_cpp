#include "PersonWorker.h"

PersonWorker::PersonWorker(int pid, int gid, String^ fn, String^ ln) : Person (pid, gid, fn, ln)
{

}

String^ PersonWorker::ToString()
{
	String^ res = this->Person::ToString();
	res = res->Replace("_p_", " w ");
	return res;
}

Tuple<Decimal, String^>^ PersonWorker::CalculateMoney()
{
	auto res = this->Person::CalculateMoney();
	// кортеж является неизменяемой структурой, 
	// поэтому если что-то нужно поменять, то нужно создавать новый кортеж
	Decimal m = res->Item1;
	String^ log = res->Item2;
	log = log->Replace("_p_", " w ");
	Tuple<Decimal, String^>^ newRes = gcnew Tuple<Decimal, String^>(m, log);
	return newRes;
}
