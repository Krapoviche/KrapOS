#define DUMMY_VAL 78
#define NR_PHILO 5

__inline__ static void outb(unsigned char value, unsigned short port)
{
	__asm__ __volatile__("outb %0, %1" : : "a" (value), "Nd" (port));
}

__inline__ static unsigned char inb(unsigned short port)
{
	unsigned char rega;
	__asm__ __volatile__("inb %1,%0" : "=a" (rega) : "Nd" (port));
	return rega;
}

static void test_it()
{
	__asm__ volatile("pushfl; testl $0x200,(%%esp); jnz 0f; sti; nop; cli; 0: addl $4,%%esp\n":::"memory");
}

unsigned long long
div64(unsigned long long x, unsigned long long div, unsigned long long *rem)
{
	unsigned long long mul = 1;
	unsigned long long q;

	if ((div > x) || !div) {
		if (rem) *rem = x;
		return 0;
	}

	while (!((div >> 32) & 0x80000000ULL)) {
		unsigned long long newd = div + div;
		if (newd > x) break;
		div = newd;
		mul += mul;
	}

	q = mul;
	x -= div;
	while (1) {
		mul /= 2;
		div /= 2;
		if (!mul) {
			if (rem) *rem = x;
			return q;
		}
		if (x < div) continue;
		q += mul;
		x -= div;
	}
}
/*******************************************************************************
 * Pseudo random number generator
 ******************************************************************************/
static unsigned long long mul64(unsigned long long x, unsigned long long y)
{
	unsigned long a, b, c, d, e, f, g, h;
	unsigned long long res = 0;
	a = x & 0xffff;
	x >>= 16;
	b = x & 0xffff;
	x >>= 16;
	c = x & 0xffff;
	x >>= 16;
	d = x & 0xffff;
	e = y & 0xffff;
	y >>= 16;
	f = y & 0xffff;
	y >>= 16;
	g = y & 0xffff;
	y >>= 16;
	h = y & 0xffff;
	res = d * e;
	res += c * f;
	res += b * g;
	res += a * h;
	res <<= 16;
	res += c * e;
	res += b * f;
	res += a * g;
	res <<= 16;
	res += b * e;
	res += a * f;
	res <<= 16;
	res += a * e;
	return res;
}

typedef unsigned long long uint_fast64_t;
typedef unsigned long uint_fast32_t;

static const uint_fast64_t _multiplier = 0x5DEECE66DULL;
static const uint_fast64_t _addend = 0xB;
static const uint_fast64_t _mask = (1ULL << 48) - 1;
static uint_fast64_t _seed = 1;

// Assume that 1 <= _bits <= 32
static uint_fast32_t
randBits(int _bits)
{
	uint_fast32_t rbits;
	uint_fast64_t nextseed = (mul64(_seed, _multiplier) + _addend) & _mask;
	_seed = nextseed;
	rbits = nextseed >> 16;
	return rbits >> (32 - _bits);
}

static unsigned long
rand()
{
	return randBits(32);
}

void do_nothing(void){
    test_it();
	rand();
}
