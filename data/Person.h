#ifndef PERSON_H
#define PERSON_H

using namespace System;

ref class Person abstract
{
private:
	String^ Firstname;
	String^ Lastname;
	DateTime^ Birthdate;

public:
	Person();
	Decimal CalculateSalary();
};	

#endif
