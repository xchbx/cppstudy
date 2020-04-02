#include <iostream>
#include "Person.h"

using namespace std;

void showInfo(Person &p) {
	std::cout << "---Person Info---" << std::endl;
	std::cout << "Name: " << p.name() << std::endl;
	std::cout << "Birthday: " << p.birthDate() << std::endl;
	std::cout << "Address: " << p.address() << std::endl;
	std::cout << "---The End-------" << std::endl;
}

int main()
{
	string usr_name("test");
	string date("10/09/2010");
	string location("NewYork");
	#if 0
	//Person* inst = new Person(usr_name,18);

	Person* inst = creat(usr_name, 18);

	cout<<"name ="<<inst->getName()<<endl;
	cout<<"age = "<<inst->getAge()<<endl;
	#endif

	Person p(usr_name, date, location);
	showInfo(p);

	return 0;
}