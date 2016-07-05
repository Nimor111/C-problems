#include <iostream>
using namespace std; 

// because strcat_s sucks? 
char* concatenate(char* str1, const char *str2) {
	char *buf = new char[strlen(str1) + 1];
	strcpy_s(buf, strlen(str1) + 1, str1);

	size_t size = strlen(str1) + strlen(str2) + 1;
	str1 = new char[size];

	for (unsigned int i = 0; i < strlen(buf); i++) {
		str1[i] = buf[i];
	}

	unsigned int j = 0;
	for (unsigned int i = strlen(buf); i < size; i++) {
		str1[i] = str2[j];
		j++;
	}

	return str1;
}

struct FileType {
	static int REGULAR;
	static int DIRECTORY;
	static int LINK;
};

int FileType::REGULAR = 0;
int FileType::DIRECTORY = 1;
int FileType::LINK = 2;

class File { // Abstract class
public:
	File(const char* name = " ") {
		this->name = new char[strlen(name) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
	}

	File(const File &other) {
		this->name = new char[strlen(other.name) + 1];
		strcpy_s(this->name, strlen(other.name) + 1, other.name);
	}

	File& operator=(const File &other) {
		if (this != &other) {
			delete name;
			this->name = new char[strlen(other.name) + 1];
			strcpy_s(this->name, strlen(other.name) + 1, other.name);
		}

		return *this;
	}

	virtual ~File() {
		delete name;
	}

	void rename(const char* name) {
		this->name = new char[strlen(name)+1];
		strcpy_s(this->name, strlen(name) + 1, name);
	}

	const char *getName() const {
		return name;
	}

	virtual void printFileContent() const = 0;
	virtual int getFileType() const = 0;
	virtual void operator+=(File* other) = 0;

private:
	char *name;
protected:
	FileType type;
};

class RegularFile : public File {
public:
	RegularFile(const char *name, const char *extention, const char *content) : File(name) {
		this->extention = new char[strlen(extention) + 1];
		strcpy_s(this->extention, strlen(extention) + 1, extention);
		this->content = new char[strlen(content) + 1];
		strcpy_s(this->content, strlen(content) + 1, content);
	}

	RegularFile(RegularFile const &other) : File(other) {
		this->extention = new char[strlen(other.extention) + 1];
		strcpy_s(this->extention, strlen(other.extention) + 1, other.extention);
		this->content = new char[strlen(other.content) + 1];
		strcpy_s(this->content, strlen(other.content) + 1, other.content);
	}

	RegularFile& operator=(const RegularFile &other) {
		if (this != &other) {
			File::operator=(other);
			this->extention = new char[strlen(other.extention) + 1];
			strcpy_s(this->extention, strlen(other.extention) + 1, other.extention);
			this->content = new char[strlen(other.content) + 1];
			strcpy_s(this->content, strlen(other.content) + 1, other.content);
		}

		return *this;
	}

	~RegularFile() {
		delete extention;
		delete content;
	}

	int getFileType() const {
		return type.REGULAR;
	}

	void printFileContent() const {
		cout << "Name: " << getName() << endl;
		cout << "Extention: " << extention << endl;
		cout << "Content: " << content << endl;
	}

	//@Override
	void operator+=(File *other) {
		if (other->getFileType() == 0 || other->getFileType() == 2) {
			content = concatenate(content, dynamic_cast<RegularFile*>(other)->content); // TODO fix with strcpy_s maybe
		} else {
			cout << "Nothing to be changed." << endl;
		}
	}
private:
	char *extention;
	char *content;
};

class Directory : public File {
public:
	Directory(const char *name) : File(name) {
		capacity = 20; 
		size = 0; 
		files = new File *[capacity];
	}

	Directory(const Directory &other) : File(other) {
		this->capacity = other.capacity; 
		this->size = other.size; 
		
		files = new File *[capacity];

		for (unsigned int i = 0; i < size; i++) {
			files[i] = other.files[i];
		}
	}

	Directory& operator=(const Directory &other) {
		if (this != &other) {
			File::operator=(other); 
			this->capacity = other.capacity;
			this->size = other.size;

			files = new File *[capacity];

			for (unsigned int i = 0; i < size; i++) {
				files[i] = other.files[i];
			}
		}

		return *this;
	}

	~Directory() {
		for (unsigned int i = 0; i < size; i++) {
			delete files[i];
		}
	}

	//@Override
	void operator+=(File *other) {
		if (size > capacity) {
			cout << " No room in directory.\n"; 
		}
		else {
			files[size++] = other; 
		}
	}
	
	int getFileType() const {
		return type.DIRECTORY; 
	}

	void printFileContent() const {
		cout << getName() << endl; 
		for (unsigned int i = 0; i < size; i++) {
			cout << i << ". "; 
			cout << files[i]->getName() << endl;
		}
	}

	
private:
	File **files;
	size_t capacity;
	size_t size;
};

class Link : public File {
public:
	Link(File *file) {
		link = file; 
	}

	Link(const Link &other) {
		this->link = other.link; 
	}

	Link& operator=(const Link &other) {
		if (this != &other) {
			this->link = other.link; 
		}

		return *this; 
	}

	~Link() {
		delete link; 
	}

	int getFileType() const {
		return type.LINK;
	}

	//@Override
	void printFileContent() const {
		link->printFileContent();
	}

	void operator+=(File *other) {
		*this->link += other; 
	}
private:
	File *link; 
};

class User {
public:
	User(const char *name = " " ,const char *password = " ") {
		this->name = new char[strlen(name) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
		this->password = new char[strlen(password) + 1];
		strcpy_s(this->password, strlen(password) + 1, password);
		capacity = 100; 
		size = 0; 
		computer = new File *[capacity];
	}

	User(const User &other) {
		this->name = new char[strlen(other.name) + 1];
		strcpy_s(this->name, strlen(other.name) + 1, other.name);
		this->password = new char[strlen(other.password) + 1];
		strcpy_s(this->password, strlen(other.password) + 1, other.password);
		capacity = other.capacity;
		size = other.size;
		computer = new File *[capacity];

		for (unsigned int i = 0; i < size; i++) {
			computer[i] = other.computer[i];
		}
	}

	User& operator=(const User &other) {
		if (this != &other) {
			this->name = new char[strlen(other.name) + 1];
			strcpy_s(this->name, strlen(other.name) + 1, other.name);
			this->password = new char[strlen(other.password) + 1];
			strcpy_s(this->password, strlen(other.password) + 1, other.password);
			capacity = other.capacity;
			size = other.size;
			computer = new File *[capacity];

			for (unsigned int i = 0; i < size; i++) {
				computer[i] = other.computer[i];
			}
		}

		return *this;
	}

	~User() {
		delete name;
		delete password;

		for (unsigned int i = 0; i < size; i++) {
			delete computer[i];
		}
	}

	void operator+=(File *other) {
		if (size > capacity) {
			cout << "No room in your computer. Consider freeing memory.\n";
		} else {
			computer[size++] = other;
		}
	}
private:
	char *name;
	char *password;
	File **computer;
	size_t capacity;
	size_t size;
};

int main()
{
	File *file = new RegularFile("Pesho", ".psh", "I like them cakes cakes");
	File *file2 = new RegularFile("Asha", ".ash", " Cakes are coolz so coolz so coolz");

	File *files = new Directory("Testdir"); 
	*files += file; 
	*files += file2;

//	files->printFileContent();

	File *link = new Link(file); 
	link->printFileContent(); 
	*link += file2;
	link->printFileContent();

	cout << endl << endl; 

	*file += file2;
	file->printFileContent();

	cout << endl << endl;

	file = file2;
	file->printFileContent();

	delete file, file2, files, link;

	system("PAUSE");
	return 0;
}
