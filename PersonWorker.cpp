#include "PersonWorker.h"

PersonWorker::PersonWorker(int pid, int gid, String^ fn, String^ ln) : Person (pid, gid, fn, ln)
{

}

String^ PersonWorker::ToString()
{
	String^ report = String::Format(" w  {0,2} (G{1}) : ", this->pid, this->gid);
	for each (Activity ^ a in this->aList)
	{
		report += String::Format("{0} ", a->ToString());
	}
	return report + Environment::NewLine;
}
