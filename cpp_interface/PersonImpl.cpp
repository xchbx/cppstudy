#include "PersonImpl.h"

using namespace std;

#if 0
PersonImpl::PersonImpl(string& name, int age):
    mName(name),
    mAge(age)
{

}

PersonImpl::~PersonImpl() {

}

string& PersonImpl::getName() const {
    return mName;
}

void PersonImpl::setName(string& name) {
    mName = name;
}

int PersonImpl::getAge() const {
    return mAge;
}

void PersonImpl::setAge(int age) {
    mAge = age;
}
#endif

#if 0

RealPerson::RealPerson(string& name, int age):
    mName(name),
    mAge(age)
{

}

RealPerson::~RealPerson()
{

}

string& RealPerson::getName() const {
    return mName;
}

void RealPerson::setName(string& name){
    mName = name;
}

int RealPerson::getAge() const{
    return mAge;
}

void RealPerson::setAge(int age){
    mAge = age;
}

Person* creat(string& name, int age) {
    return new RealPerson(name, age);
}

#endif
