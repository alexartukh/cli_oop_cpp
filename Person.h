#pragma once

#include "Activity.h"

using namespace System;
using namespace System::Collections::Generic;

ref class Person
{
private:
	int id;
	String^ Firstname;
	String^ Lastname;
	List<Activity^>^ aList;

public:
	Person();
	Decimal CalculateSalary();
};	
