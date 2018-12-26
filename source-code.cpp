#define BLOCKSIZE 256
#define REFRESH 100
#define VISUALISE false

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>

enum commands {
	INC, DEC, ADD, SUB, OUTP, INP, JUMF, JUMB
};

class MemoryBlock {
private:
	unsigned char data[BLOCKSIZE];
	static unsigned int pointer;
	MemoryBlock *nextblock;
	MemoryBlock *prevblock;

public:
	MemoryBlock() {
		for (int i = 0; i < BLOCKSIZE; i++)
		{
			data[i] = 0;
		}
		nextblock = nullptr;
		prevblock = nullptr;
	}

	~MemoryBlock()
	{
		if ((nextblock != nullptr) && (prevblock != nullptr))
		{
			nextblock->prevblock = prevblock;
			prevblock->nextblock = nextblock;
		}
		else if (nextblock != nullptr)
		{
			nextblock->prevblock = nullptr;
		}
		else if (prevblock != nullptr)
		{
			prevblock->nextblock = nullptr;
		}
	}

private:
	void increment() {
		pointer = (pointer + 1) % BLOCKSIZE;
	}

	void decrement() {
		pointer = (pointer + BLOCKSIZE - 1) % BLOCKSIZE;
	}

	void add() {
		data[pointer]++;
	}

	void sub() {
		data[pointer]--;
	}

	void in()
	{
		std::cin >> std::noskipws >> data[pointer];
	}

	void out()
	{
		std::cout << data[pointer];
	}

	friend class BrainFuck;
};

class BrainFuck {
private:
	MemoryBlock *begin;
	MemoryBlock *currentblock;
	std::ifstream fileread;
	std::vector<unsigned char> output;
	std::vector<commands> routines;
	bool visualisation;

public:
	BrainFuck()
	{
		currentblock = new MemoryBlock;
		begin = currentblock;
		fileread.open("source.bf");
		visualisation = VISUALISE;
	}

	void read()
	{
		char ch;
		int loopbalance = 0;
		while (fileread.get(ch))
		{
			switch (ch)
			{
			case '>':
				routines.push_back(commands::INC);
				break;
			case '<':
				routines.push_back(commands::DEC);
				break;
			case '+':
				routines.push_back(commands::ADD);
				break;
			case '-':
				routines.push_back(commands::SUB);
				break;
			case '.':
				routines.push_back(commands::OUTP);
				break;
			case ',':
				routines.push_back(commands::INP);
				break;
			case '[':
				loopbalance++;
				routines.push_back(commands::JUMF);
				break;
			case ']':
				loopbalance--;
				if (loopbalance < 0)
				{
					std::cout << "Loop Unbalanced!" << std::endl;
					return;
				}
				routines.push_back(commands::JUMB);
				break;
			default:
				break;
			}
		}
		if (loopbalance != 0)
		{
			std::cout << "Loop Unbalanced!" << std::endl;
			return;
		}
	}

	void display()
	{
		MemoryBlock *temp = begin;
		int blockcount = 0;
		std::cout << "     ";
		for (int i = 0; i < 16; i++)
			std::cout << " " << std::hex << std::setw(2) << std::uppercase << i << " ";
		std::cout << std::endl << std::endl << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << blockcount << " ";
		while (temp != nullptr)
		{
			for (int i = 0; i < BLOCKSIZE; i++)
			{
				if ((temp == currentblock) && (i == currentblock->pointer))
					std::cout << "[" << std::setw(2) <<int(temp->data[i]) << "]";
				else
					std::cout << " " << std::setw(2) << int(temp->data[i]) << " ";
				if ((i % 16 == 15) && ((currentblock->nextblock != nullptr) || (blockcount % 16 != 15)))
				{
					blockcount++;
					std::cout << std::endl << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << blockcount << " ";
				}
			}
			temp = temp->nextblock;
		}
		std::cout << std::endl << "OUTPUT: ";
		for (auto itr = output.begin(); itr != output.end(); itr++)
		{
			std::cout << *itr;
		}
		std::cout << std::endl;
	}

	void run()
	{
		if (visualisation)
		{
			this->display();
			Sleep(REFRESH);
		}
		else
		{
			std::cout << "OUTPUT: ";
		}
		for (std::vector<commands>::iterator itr = routines.begin(); itr != routines.end(); itr++)
		{
			switch (*itr)
			{
			case INC:
				if (currentblock->pointer == BLOCKSIZE - 1)
				{
					if (currentblock->nextblock == nullptr)
					{
						currentblock->nextblock = new MemoryBlock;
						currentblock->nextblock->prevblock = currentblock;
					}
					currentblock = currentblock->nextblock;
				}
				currentblock->pointer = (currentblock->pointer + 1) % BLOCKSIZE;
				break;
			case DEC:
				if (currentblock->pointer == 0)
				{
					if (currentblock->prevblock == nullptr)
					{
						std::cout << "Memory Failed!" << std::endl;
						return;
					}
					currentblock = currentblock->prevblock;
				}
				currentblock->pointer = (currentblock->pointer + BLOCKSIZE - 1) % BLOCKSIZE;
				break;
			case ADD:
				currentblock->data[currentblock->pointer] += 1;
				break;
			case SUB:
				currentblock->data[currentblock->pointer] -= 1;
				break;
			case OUTP:
				output.push_back(currentblock->data[currentblock->pointer]);
				if (!visualisation)
					std::cout << currentblock->data[currentblock->pointer];
				break;
			case INP:
				std::cout << std::endl << "INPUT: ";
				std::cin >> currentblock->data[currentblock->pointer];
				std::cout << "OUTPUT: ";
				break;
			case JUMF:
				if (currentblock->data[currentblock->pointer] == 0)
				{
					int newcount = 0;
					while (true)
					{
						if (*itr == commands::JUMF)
							newcount++;
						else if (*itr == commands::JUMB)
						{
							newcount--;
							if (newcount == 0)
								break;
						}
						itr++;
						if (itr == routines.end())
						{
							itr--;
							break;
						}
					}
				}
				break;
			case JUMB:
				if (currentblock->data[currentblock->pointer] != 0)
				{
					int newcount = 0;
					while (true)
					{
						if (*itr == commands::JUMB)
							newcount--;
						else if (*itr == commands::JUMF)
						{
							newcount++;
							if (newcount == 0)
								break;
						}
						itr--;
						if (itr == routines.begin())
							break;
					}
				}
				break;
			default:
				break;
			}
			if (visualisation)
			{
				system("CLS");
				this->display();
				Sleep(REFRESH);
			}
		}
	}

	~BrainFuck()
	{
		while (true)
		{
			if ((currentblock->nextblock == nullptr) && (currentblock->prevblock == nullptr))
				break;
			else
			{
				if (currentblock->nextblock != nullptr)
					delete currentblock->nextblock;
				if (currentblock->prevblock != nullptr)
					delete currentblock->prevblock;
			}
		}
		delete currentblock;
	}
};

unsigned int MemoryBlock::pointer = 0;

int main()
{
	BrainFuck myCompiler;
	myCompiler.read();
	myCompiler.run();
	return 0;
}
