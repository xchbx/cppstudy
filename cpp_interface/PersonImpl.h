#if 0
class PersonImpl {
public:
    PersonImpl(string& name, int age);
    virtual ~PersonImpl();

    string& getName() const;
    void setName(string& name);
    int getAge() const;
    void setAge(int age);

private:
    string& mName;
    int mAge;
};
#endif


#if 0

class RealPerson: public Person {
public:
    RealPerson(std::string& name, int age);
    virtual ~RealPerson();

   	std::string& getName() const;
    void setName(std::string& name);
    int getAge() const;
    void setAge(int age);

private:
    friend Person* creat(std::string& name, int age);

private:
    std::string& mName;
    int mAge;
};
#endif

#include <string>
using Date = std::string;
using Address = std::string;

class PersonImpl {
public:
	PersonImpl(const std::string &name, const Date& birthday, const Address& addr) {
		this->theName = name;
		this->theBirthDate = birthday;
		this->theAddress = addr;
	}

	std::string name() const { return theName; }
	std::string birthDate() const { return theBirthDate; }
	std::string address() const { return theAddress; }

private:
	std::string theName;
	Date theBirthDate;
	Address theAddress;
};