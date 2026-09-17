#pragma once

#include "Person.h"

ref class PersonWorker : public Person
{
public:
	PersonWorker(int pid, int gid, String^ fn, String^ ln);
	virtual String^ ToString() override;
	virtual Tuple<Decimal, String^>^ CalculateMoney() override;
};

