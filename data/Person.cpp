#include "Person.h"

Person::Person()
{
	Firstname = "AAA";
	Lastname = "BBB";
	Birthdate = DateTime::Now;	
}

Decimal Person::CalculateSalary()
{
	return 0;
}
