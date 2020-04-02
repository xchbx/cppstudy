
#if 0
#include "Person.h"
#include "PersonImpl.h"

Person::Person(string& name, int age):
    mPersonImpl(new PersonImpl(name, age))
{
    std::cout << "construct Person" << std::endl;
}

Person::~Person() {
    delete mPersonImpl;
    std::cout << "deconstruct Person" << std::endl;
}

string& Person::getName() const {
    return mPersonImpl->getName();
}

void Person::setName(string& name) {
    mPersonImpl->setName(name);
}

int Person::getAge() const {
    return mPersonImpl->getAge();
}

void Person::setAge(int age) {
    mPersonImpl->setAge(age);
}

#endif

#include "Person.h"


Person::Person(const std::string &name, const Date& birthday, const Address& addr):
	pImpl(new PersonImpl(name,birthday,addr)){
}

std::string Person::name() const {
	return pImpl->name();
}

std::string Person::birthDate() const {
	return pImpl->birthDate();
}

std::string Person::address() const {
	return pImpl->address();
}
