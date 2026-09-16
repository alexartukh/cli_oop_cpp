#pragma once

#include "Activity.h"

using namespace System;
using namespace System::Collections::Generic;

ref class Person
{
private:
	int pid;
	List<Activity^>^ aList;

public:
	Person(int pid);
	void AddActivity(Activity^ a);
	Decimal CalculateSalary();
	String^ GenerateTextReport();
};	
