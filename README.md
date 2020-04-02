# cppstudy
cppstudy

《Effective C++》 :条款31：将文件间的编译依存关系降至最低。

问题
通常我们会在一个类的定义中同时写出声明式和定义式，如
···
class Person {
public:
	Person(const std::string &name, const Date& birthday, const Address& addr);
	
	std::string name() const;
	std::string birthDate() const;
	std::string address() const;
	//上面的全是声明式，因为都是函数声明而没有在这里实现
private:
	//下面都是类成员变量的定义，因此这里是定义式
	std::string theName;
	Date theBirthDate;
	Address theAddress;
};

···
而使用这样的写法，很可能会导致文件包含的问题，比如Date类，Address类的头文件包含，同时当改变了Person类所依赖的某个类时，会导致Person类重新编译。这样写也没有做到class的接口与实现分离。因此有两种方法可以实现所谓的接口与实现分离，让逻辑更清晰。

方法一：“pimpl idiom”
pimpl是"pointer to implementation"的缩写，意为用指针指向其实现类来代替主类中的定义式部分。
代码如下：
···
using Date = std::string;//仅供示例，因此不实现Date类了
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

class Person {
public:
	Person(const std::string &name, const Date& birthday, const Address& addr);
	
	std::string name() const;
	std::string birthDate() const;
	std::string address() const;

private:
	std::shared_ptr<PersonImpl> pImpl;//Person的实现部分，使用指针，将Person类变成只有接口的类，
	//而把实现部分放到另一个类中实现，并引用这个类对象的指针来进行操作
};

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

void showInfo(Person &p) {
	std::cout << "---Person Info---" << std::endl;
	std::cout << "Name: " << p.name() << std::endl;
	std::cout << "Birthday: " << p.birthDate() << std::endl;
	std::cout << "Address: " << p.address() << std::endl;
	std::cout << "---The End-------" << std::endl;
}

int main(){
	Person p("Jack", "10/09/2010", "NewYork");
	showInfo<Person>(p);
	return 0;
}

···
方法二：Interface Classes
将Person类定义为内含纯虚函数的类，纯虚函数即是Person类提供的接口。
···
using Date = std::string;//仅供示例，因此不实现Date类了
using Address = std::string;

class Person {
public:
	virtual ~Person() {}
	static std::shared_ptr<Person> create(const std::string &name, const Date& birthday, const Address& addr);

	virtual std::string name() const = 0;
	virtual std::string birthDate() const = 0;
	virtual std::string address() const = 0;
};

class RealPerson : public Person {
public:
	RealPerson(const std::string &name, const Date& birthday, const Address& addr)
		: theName(name), theBirthDate(birthday), theAddress(addr) {
	}

	std::string name() const { return theName; }
	std::string birthDate() const { return theBirthDate; }
	std::string address() const { return theAddress; }

private:
	std::string theName;
	Date theBirthDate;
	Address theAddress;
};

std::shared_ptr<Person> Person::create(const std::string &name, const Date& birthday, const Address& addr) {
	return std::shared_ptr<Person>(new RealPerson(name, birthday, addr));
}

int main()
{
	std::shared_ptr<Person> pp = Person::create("Jack", "10/09/2010", "NewYork");
}

···
总结
来自《Effective C++》的三个设计策略：

如果可以使用对象引用或指针完成任务，就不要用对象 ：因为只靠一个类型声明就可以定义出指向该类型的指针和引用，但如果要定义该类型的对象，则需要用到该类型的定义式。
尽量以class声明式代替class定义式：因为当声明一个函数，而它用到某个class时，它并不需要改class的定义，即使是使用pass by value的方式来传参或者返回值。
为声明式和定义式提供不同的头文件：这样做提供了编程时声明该类的便利，也将声明和实现分离，方便管理。
参考文献
《Effective C++》 - 条款31：将文件间的编译依存关系降至最低
