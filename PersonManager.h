#pragma once

#include "Person.h"

ref class PersonManager : public Person
{
protected:
	int Subs = 0;
public:
	static Decimal MoneyForManager;

	PersonManager(int pid, int gid, String^ fn, String^ ln);
	int GetSubs();
	void SetSubs(int s);
	virtual String^ ToString() override;
	virtual Tuple<Decimal, String^>^ CalculateMoney() override;
};
