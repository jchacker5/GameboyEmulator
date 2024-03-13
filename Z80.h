#define TRUE 1
#define FALSE 0
class Z80
{
public:
	int A, B, C, D, E, H, L, SP, PC;
	int FLAG_C, FLAG_N, FLAG_P, FLAG_H, FLAG_Z, FLAG_S;
	int FLAG_I, interrupt_deferred, interrupts;
	int FLAGS;
	bool halted;
	
	int opcode,value,imm;
	const char* instruction;
	int cycles;

	Z80(unsigned char(*)(int),void(*)(int,unsigned char));
	void reset();
	void doInstruction();
	void checkForInterrupts();
	void throwInterrupt(int);

private:
	int arithmetic(int,bool,bool);
	int arithmetic16(int,int,bool,bool);
	void daa();
	int incrementDecrement(int,bool);
	void logicflags(int);
	int sr(int,bool);
	int sl(int,bool);
	int swap(int);
	int rrc(int,bool);
	int rr(int,bool);
	int rlc(int,bool);
	int rl(int,bool);
	void bit(int,int);
	void setFlags();
	void readFlags();
	void push(int);
	int pop();
	void halt();
	void ei();
	unsigned char fetch();
	void fault(int);
	void handleInterrupt(int);

	unsigned char (*memory_read)(int);
	void (*memory_write)(int,unsigned char);
};
