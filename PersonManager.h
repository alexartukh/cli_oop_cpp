#pragma once

#include "Person.h"

ref class PersonManager : public Person
{
public:
	PersonManager(int pid, int gid, String^ fn, String^ ln);
	virtual String^ ToString() override;
};

