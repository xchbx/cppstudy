#include <string>
#include <memory>

#include "PersonImpl.h"
#if 0
class PersonImpl;

class Person {
public:
    Person(string& name, int age);
    virtual ~Person();

    string& getName() const;
    void setName(string& name);
    int getAge() const;
    void setAge(int age);

private:
    PersonImpl *mPersonImpl;
};
#endif

#if 0

class Person {
public:
    Person(){};
    virtual ~Person(){};

    virtual std::string& getName() const = 0;
    virtual void setName(std::string& name) = 0;
    virtual int getAge() const = 0;
    virtual void setAge(int age) = 0;
};

Person* creat(std::string& name, int age);
#endif
using Date = std::string;
using Address = std::string;

class Person {
public:
	Person(const std::string &name, const Date& birthday, const Address& addr);
	
	std::string name() const;
	std::string birthDate() const;
	std::string address() const;

private:
	std::shared_ptr<PersonImpl> pImpl;

};