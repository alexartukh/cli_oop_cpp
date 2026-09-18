#pragma once

#include "Activity.h"

using namespace System;
using namespace System::Collections::Generic;

ref class Person
{
protected:
	int pid;
	int gid;
	String^ fn;
	String^ ln;
	List<Activity^>^ aList;
	
public:
	static Decimal MoneyPerHour;

	Person(int pid, int gid, String^ fn, String^ ln);
	void operator + (Activity^ a);

	virtual Tuple<Decimal, String^>^ CalculateMoney() override;
	virtual String^ ToString() override;
};	
