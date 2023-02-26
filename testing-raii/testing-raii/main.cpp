#include <iostream>


class RAII
{
public:
	RAII(int number) : _number(number) {}
	~RAII() { std::cout << _number << std::endl; }
private:
	int _number;
};

int
main(
	int argc,
	const char* argv[]
)
{
	RAII first(1);
	RAII second(2);
	return 0;
}