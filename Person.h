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
	Person(int pid, int gid, String^ fn, String^ ln);
	void AddActivity(Activity^ a);
	int CalculateSalary();
	virtual String^ ToString() override;
};	
