// Generation du test.c parsé :
// cpp -traditional-cpp -C -P -ftabstop=8 -DTELECOM_TST -DCONS_READ_CHAR -DWITH_MSG test.c test.i
// sed -i '/./,/^$/!d' test.i
// cat test.h test.i

//XXX Assurer que l'oubli d'une option fait planter la compilation
//XXX Verifier l'absence de caracteres non ASCII

/*******************************************************************************
 * Gestion de liste d'arguments de taille variable (printf)
 ******************************************************************************/
typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;
#define va_arg(AP, TYPE)                                                \
 (AP = (__gnuc_va_list) ((char *) (AP) + __va_rounded_size (TYPE)),     \
  *((TYPE *) (void *) ((char *) (AP) - __va_rounded_size (TYPE))))
#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))
#define va_start(AP, LASTARG)                                           \
 (AP = ((__gnuc_va_list) __builtin_next_arg (LASTARG)))
#define va_end(AP)      ((void)0)

/*******************************************************************************
 * Printf macros
 ******************************************************************************/
#define PRINTF_LEFT_JUSTIFY 1
#define PRINTF_SHOW_SIGN 2
#define PRINTF_SPACE_PLUS 4
#define PRINTF_ALTERNATE 8
#define PRINTF_PAD0 16
#define PRINTF_CAPITAL_X 32

#define PRINTF_BUF_LEN 512

/*******************************************************************************
 * Assert : check a condition or fail
 ******************************************************************************/
#define __STRING(x) #x

#define assert(cond) \
((void)((cond) ? 0 : assert_failed(__STRING(cond), __FILE__, __LINE__)))

#define DUMMY_VAL 78

#define TSC_SHIFT 8

#define FREQ_PREC 50

#define NBSEMS 10000

#define TRUE 1
#define FALSE 0

#define NR_PHILO 5
/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */

/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */

/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */

/* We do not support C11 <threads.h>.  */

/*
 * Tests du projet systeme
 *
 * Ce fichier contient du code qui ne depend que des appels systemes
 * ci-dessous. Il n'inclut aucun fichier ".h".
 *
 * Il est possible de placer ce fichier dans le repertoire user pour faire
 * tourner les tests au niveau utilisateur ou, si l'implantation du mode
 * utilisateur ne fonctionne pas, dans le repertoire kernel pour faire
 * tourner les tests au niveau superviseur.
 * Les tests sont separes en 20 fonctions qui testent differentes parties du
 * projet.
 * Aucune modification ne doit etre apportee a ce fichier pour la soutenance.
 *
 * Il existe deux manieres d'appeler les tests :
 *
 * int test_proc(void *arg) :
 * a demarrer comme un processus avec une priorite de 128. Ce processus
 * attend que l'utilisateur saisisse des numeros de tests a executer.
 *
 * int test_run(int n) :
 * a appeler dans un processus de priorite 128 avec en parametre un numero de
 * test a executer.
 *
 * La fonction test_run() vous permet d'appeler facilement un test mais en
 * soutenance il est preferable d'executer test_proc().
 */

// Prototype des appels systeme de la spec
int chprio(int pid, int newprio);
void cons_write(const char *str, unsigned long size);
int cons_read(char *str, unsigned long size);
void cons_echo(int on);
void exit(int retval);
int getpid(void);
int getprio(int pid);
int kill(int pid);
int pcount(int fid, int *count);
int pcreate(int count);
int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);
int psend(int fid, int message);
void clock_settings(unsigned long *quartz, unsigned long *ticks);
unsigned long current_clock(void);
void wait_clock(unsigned long wakeup);
int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg);
int waitpid(int pid, int *retval);

/*
 * Pour la soutenance, devrait afficher la liste des processus actifs, des
 * files de messages utilisees et toute autre info utile sur le noyau.
 */
void sys_info(void);

static int
strcmp(const char *str1, const char *str2)
{
	while (*str1 == *str2) {
		if (*str1 == 0)
			return 0;
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

static unsigned int
strlen(const char *s)
{
	unsigned long l = 0;
	while (*s++) l++;
	return l;
}

static void
cons_puts(const char *s)
{
	cons_write(s, strlen(s));
}

/*******************************************************************************
static int _printf( char *s, unsigned long n, const char *format, va_list ap )
    La grosse fonction du module. Une version interne du printf, qui imprime
soit sur la sortie standard, soit dans un buffer.
Si s == 0 : on imprime sur la sortie standard.
Si s != 0 : on imprime dans la chaine s sur une taille maximale n.
*******************************************************************************/
struct printf_st {
	/* Flags obtenus a l'interpretation de la chaine de format. */
	int flags;
	int width;
	int precision;
	char modifier;
	int count;

	/* Ce buffer permet de contenir le texte correspondant a l'affichage
	   d'un nombre. */
	char buffer_nombre[20];

	/* Buffer pour l'ecriture a la console. */
	char buffer_ecr[PRINTF_BUF_LEN];
	int pos_ecr;

	char *str;
	unsigned long strl;
};

static void
print(struct printf_st *pf, char c)
{
	while (1) {
		if (pf->str == 0) {
			/* Cas de l'ecriture sur un fichier. */
			if (c == 0)
				return;
		
			if (pf->pos_ecr < PRINTF_BUF_LEN - 1) {
				pf->count++;
				pf->buffer_ecr[pf->pos_ecr++] = c;
			} else {
				pf->buffer_ecr[PRINTF_BUF_LEN - 1] = 0;
				cons_puts(pf->buffer_ecr);
				pf->buffer_ecr[0] = c;
				pf->pos_ecr = 1;
			}
		} else {
			/* Cas de l'ecriture dans un buffer. */
			if ((c != 0) && (pf->strl != 0)) {
				pf->count++;
				*pf->str++ = c;
				pf->strl--;
			}
		}
		if (c != '\n') return;
		c = '\r';
	}
}

/****************************************************************************
 * Pour afficher les "%s".
 ***************************************************************************/
static void
print_string(struct printf_st *pf, char *s) {
	int size = 0;
	char *ptr = s;

	/* Calcule la taille de la partie de la chaine a afficher. */
	if (pf->precision >= 0)
		while ((size < pf->precision) && (*ptr++ != 0))
			size++;
	else
		/* Pas besoin d'aller trop loin dans le calcul de la taille de la
			partie a afficher. Si la valeur est superieure a width, elle ne nous
			sert a rien, bien qu'on va tout afficher. */
		while ((size < pf->width) && (*ptr++ != 0))
			size++;

	if (!(pf->flags & PRINTF_LEFT_JUSTIFY))
		while (pf->width-- > size)
			print(pf, ' ');
	while ((pf->precision-- != 0) && (*s != 0))
		print(pf, *s++);
	while (pf->width-- > size)
		print(pf, ' ');
}

/*******************************************************************************
 * Pour afficher les "%c".
 ******************************************************************************/
static void
print_char(struct printf_st *pf, char c) {
	if (!(pf->flags & PRINTF_LEFT_JUSTIFY))
		while (pf->width-- > 1)
			print(pf, ' ');
	print(pf, c);
	while (pf->width-- > 1)
		print(pf, ' ');
}

/*******************************************************************************
 * Pour afficher les "%x", "%X".
 ******************************************************************************/
static void
print_hexa(struct printf_st *pf, unsigned long i) {
	int pos = 0;
	int n;

	/* On ne met pas le "0x" si le nombre est nul. */
	if (i == 0)
		pf->flags &= ~PRINTF_ALTERNATE;

	/* La pf->precision par defaut pour un entier est 1. */
	if (pf->precision == -1)
		pf->precision = 1;
	else
		pf->flags &= ~PRINTF_PAD0;

	/* On ecrit l'entier dans le buffer. */
	while (i != 0) {
		n = i % 16;
		i = i / 16;

		/* On calcule le chiffre de poids faible. */
		if (n < 10)
			n += '0';
		else if (pf->flags & PRINTF_CAPITAL_X)
			n += 'A' - 10;
		else
			n += 'a' - 10;

		/* On le met en buffer. */
		pf->buffer_nombre[pos++] = n;
	}

	/* On met a jour la precision avec celle que demande le nombre affiche. */
	pf->precision = (pos > pf->precision) ? pos : pf->precision;

	/* Si on doit remplir avec des 0, on modifie la precision en consequence. */
	if ((!(pf->flags & PRINTF_LEFT_JUSTIFY)) && (pf->flags & PRINTF_PAD0)) {
		n = pf->width;

		if ((pf->flags & PRINTF_SHOW_SIGN) || (pf->flags & PRINTF_SPACE_PLUS))
			n--;
		if (pf->flags & PRINTF_ALTERNATE)
			n -= 2;
		pf->precision = (pf->precision > n) ? pf->precision : n;
		n = pf->width;
	} else {
		n = pf->precision;
		if ((pf->flags & PRINTF_SHOW_SIGN) || (pf->flags & PRINTF_SPACE_PLUS))
			n++;
		if (pf->flags & PRINTF_ALTERNATE)
			n += 2;
	}
	/* Ici n = nombre de caracteres != ' ' affiches. */

	/* Doit-on mettre des espaces de remplissage avant le nombre ? */
	if (!(pf->flags & PRINTF_LEFT_JUSTIFY)) {
		while (pf->width-- > n)
			print(pf, ' ');
	}

	/* On place eventuellement le signe. */
	if (pf->flags & PRINTF_SHOW_SIGN)
		print(pf, '+');
	else if (pf->flags & PRINTF_SPACE_PLUS)
		print(pf, ' ');

	/* On ecrit l'eventuel "0x" ou "0X". */
	if (pf->flags & PRINTF_ALTERNATE) {
		print(pf, '0');
		if (pf->flags & PRINTF_CAPITAL_X)
			print(pf, 'X');
		else
			print(pf, 'x');
	}

	/* On met les eventuels 0 de remplissage. */
	while (pf->precision-- > pos)
		print(pf, '0');

	/* On copie le reste du nombre. */
	while (pos-- != 0)
		print(pf, pf->buffer_nombre[pos]);

	/* On met enfin les eventuels espaces de fin. */
	while (pf->width-- > n)
		print(pf, ' ');
}

/*******************************************************************************
 * Pour afficher les "%d", "%i" et "%u". Le signe doit etre '+' ou '-'.
 ******************************************************************************/
static void
print_dec(struct printf_st *pf, unsigned long i, char sign) {
	int pos = 0;
	int n;

	/* La precision par defaut pour un entier est 1. */
	if (pf->precision == -1)
		pf->precision = 1;
	else
		pf->flags &= ~PRINTF_PAD0;

	/* On determine le signe a afficher. */
	if ((sign == '+') && (!(pf->flags & PRINTF_SHOW_SIGN))) {
		if (pf->flags & PRINTF_SPACE_PLUS)
			sign = ' ';
		else
			sign = 0;
	}

	/* On ecrit l'entier dans le buffer. */
	while (i != 0) {
		/* On le met en buffer. */
		pf->buffer_nombre[pos++] = (i % 10) + '0';
		i = i / 10;
	}

	/* On met a jour la precision avec celle que demande le nombre affiche. */
	pf->precision = (pos > pf->precision) ? pos : pf->precision;

	/* Si on doit remplir avec des 0, on modifie la precision en consequence. */
	if ((!(pf->flags & PRINTF_LEFT_JUSTIFY)) && (pf->flags & PRINTF_PAD0)) {
		n = pf->width;

		if (sign != 0)
			n--;
		pf->precision = (pf->precision > n) ? pf->precision : n;
		n = pf->width;
	} else {
		n = pf->precision;
		if (sign != 0)
			n++;
	}
	/* Ici n = nombre de caracteres != ' ' affiches. */

	/* Doit-on mettre des espaces de remplissage avant le nombre ? */
	if (!(pf->flags & PRINTF_LEFT_JUSTIFY)) {
		while (pf->width-- > n)
			print(pf, ' ');
	}

	/* On place eventuellement le signe. */
	if (sign != 0)
		print(pf, sign);

	/* On met les eventuels 0 de remplissage. */
	while (pf->precision-- > pos)
		print(pf, '0');

	/* On copie le reste du nombre. */
	while (pos-- != 0)
		print(pf, pf->buffer_nombre[pos]);

	/* On met enfin les eventuels espaces de fin. */
	while (pf->width-- > n)
		print(pf, ' ');
}

/*******************************************************************************
 *   Pour afficher les "%x", "%X".
 ******************************************************************************/
static void
print_oct(struct printf_st *pf, unsigned int i) {
	int pos = 0;
	int n;

	/* La precision par defaut pour un entier est 1. */
	if (pf->precision == -1)
		pf->precision = 1;
	else
		pf->flags &= ~PRINTF_PAD0;

	/* On ecrit l'entier dans le buffer. */
	while (i != 0) {
		pf->buffer_nombre[pos++] = (i % 8) + '0';
		i = i / 8;
	}

	/* On verifie si on doit mettre un zero en tete. */
	if (pf->flags & PRINTF_ALTERNATE)
		pf->buffer_nombre[pos++] = '0';

	/* On met a jour la precision avec celle que demande le nombre affiche. */
	pf->precision = (pos > pf->precision) ? pos : pf->precision;

	/* Si on doit remplir avec des 0, on modifie la precision en consequence. */
	if ((!(pf->flags & PRINTF_LEFT_JUSTIFY)) && (pf->flags & PRINTF_PAD0)) {
		n = pf->width;

		if ((pf->flags & PRINTF_SHOW_SIGN) || (pf->flags & PRINTF_SPACE_PLUS))
			n--;
		pf->precision = (pf->precision > n) ? pf->precision : n;
		n = pf->width;
	} else {
		n = pf->precision;
		if ((pf->flags & PRINTF_SHOW_SIGN) || (pf->flags & PRINTF_SPACE_PLUS))
			n++;
	}
	/* Ici n = nombre de caracteres != ' ' affiches. */

	/* Doit-on mettre des espaces de remplissage avant le nombre ? */
	if (!(pf->flags & PRINTF_LEFT_JUSTIFY)) {
		while (pf->width-- > n)
			print(pf, ' ');
	}

	/* On place eventuellement le signe. */
	if (pf->flags & PRINTF_SHOW_SIGN)
		print(pf, '+');
	else if (pf->flags & PRINTF_SPACE_PLUS)
		print(pf, ' ');

	/* On met les eventuels 0 de remplissage. */
	while (pf->precision-- > pos)
		print(pf, '0');

	/* On copie le reste du nombre. */
	while (pos-- != 0)
		print(pf, pf->buffer_nombre[pos]);

	/* On met enfin les eventuels espaces de fin. */
	while (pf->width-- > n)
		print(pf, ' ');
}

/*******************************************************************************
 * Pour afficher les "%p".
 ******************************************************************************/
static void
print_pointer(struct printf_st *pf, void *p) {
	if (p == 0) {
		print_string(pf, "(nil)");
	} else {
		pf->flags |= PRINTF_ALTERNATE;
		print_hexa(pf, (unsigned long) p);
	}
}

/*******************************************************************************
 * Voici la fonction "principale".
 ******************************************************************************/
static int
__printf(struct printf_st *pf, const char *format, va_list ap) {
	pf->count = 0;
	while (*format != 0) {
		if (*format == '%') {
			const char *ptr = format + 1;

			/* On lit le champ optionnel flags. */
			pf->flags = 0;
			flags_l:
			switch (*ptr) {
			case '-':
				pf->flags |= PRINTF_LEFT_JUSTIFY;
				ptr++;
				goto flags_l;

			case '+':
				pf->flags |= PRINTF_SHOW_SIGN;
				ptr++;
				goto flags_l;

			case ' ':
				pf->flags |= PRINTF_SPACE_PLUS;
				ptr++;
				goto flags_l;

			case '#':
				pf->flags |= PRINTF_ALTERNATE;
				ptr++;
				goto flags_l;

			case '0':
				pf->flags |= PRINTF_PAD0;
				ptr++;
				goto flags_l;
			}

			/* On lit le champ optionnel width. */
			if (*ptr == '*') {
				pf->width = va_arg(ap, int);
				ptr++;
			} else {
				pf->width = 0;
				while ((*ptr >= '0') && (*ptr <= '9'))
					pf->width =
						pf->width * 10 + (*ptr++) - '0';
			}

			/* On lit le champ optionnel de precision. */
			if (*ptr == '.') {
				ptr++;
				if (*ptr == '*') {
					pf->precision = va_arg(ap, int);
					ptr++;
				} else {
					pf->precision = 0;
					while ((*ptr >= '0')
						&& (*ptr <= '9'))
						pf->precision +=
							pf->precision * 10 +
							(*ptr++) - '0';
				}
			} else
				pf->precision = -1;

			/* On lit le champ optionnel modifier. */
			pf->modifier = 0;
			if ((*ptr == 'h') || (*ptr == 'l')
				|| (*ptr == 'L'))
				pf->modifier = *ptr++;

			/* On lit enfin le champ obligatoire. */
			switch (*ptr) {
			case 'p':
				print_pointer(pf, va_arg(ap, void *));
				break;

			case 'X':
				pf->flags |= PRINTF_CAPITAL_X;
				/* FALLTHROUGH */
			case 'x':
				if (pf->modifier == 'h')
					print_hexa(pf, va_arg(ap, int));
				else if (pf->modifier == 'l')
					print_hexa(pf, va_arg
							(ap, unsigned long));
				else
					print_hexa(pf, va_arg
							(ap, unsigned int));
				break;

			case 'd':
			case 'i':
				{
					int i;

					if (pf->modifier == 'h')
						i = va_arg(ap, int);
					else if (pf->modifier == 'l')
						i = va_arg(ap, long);
					else
						i = va_arg(ap, int);
					if (i < 0)
						print_dec(pf, -i, '-');
					else
						print_dec(pf, i, '+');
					break;
				}

			case 'u':
				{
					int i;

					if (pf->modifier == 'h')
						i = va_arg(ap, int);
					else if (pf->modifier == 'l')
						i = va_arg(ap, long);
					else
						i = va_arg(ap, int);
					if (i < 0)
						print_dec(pf, -i, '-');
					else
						print_dec(pf, i, '+');

					break;
				}

			case 's':
				print_string(pf, va_arg(ap, char *));
				break;

			case 'c':
				print_char(pf, va_arg(ap, int));
				break;

			case '%':
				print(pf, '%');
				break;

			case 'o':
				if (pf->modifier == 'h')
					print_oct(pf, va_arg(ap, int));
				else if (pf->modifier == 'l')
					print_oct(pf, va_arg
							(ap, unsigned long));
				else
					print_oct(pf, va_arg
							(ap, unsigned int));
				break;

			case 'n':
				*va_arg(ap, int *) = pf->count;
				break;

			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 'G':
				pf->flags = 0;
				pf->width = 0;
				pf->precision = -1;
				print_string
					(pf, "<float format not implemented>");
				break;

			default:
				/* Comme le format n'est pas valide, on l'affiche ! */
				while (format < ptr)
					print(pf, *format++);
				ptr--;
			}
			format = ptr + 1;
		} else
			print(pf, *format++);
	}
	return pf->count;
}

static int
_printf(char *s, unsigned long n, const char *format, va_list ap)
{
	struct printf_st pf;
	pf.pos_ecr = 0;
	pf.str = s;
	pf.strl = n;

	if (s != 0) {
		/* Cas du print dans un buffer. */
		if (n) {
			/* On reserve un caractere pour le 0 terminal. */
			n--;
			__printf(&pf, format, ap);
			*s = 0;

			/* On renvoie la taille de la chaine ecrite. */
			return pf.count;
		}

		return 0;
	} else {
		/* Cas du print dans sur la console. */
		__printf(&pf, format, ap);
		pf.buffer_ecr[pf.pos_ecr] = 0;
		cons_puts(pf.buffer_ecr);

		/* On renvoie la taille de la chaine ecrite. */
		return pf.count;
	}
}

/*******************************************************************************
static int printf( const char *format, ... )
*******************************************************************************/
static int
printf(const char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = _printf(0, 0, format, ap);

	va_end(ap);
	return ret;
}

static int
assert_failed(const char *cond, const char *file, int line)
{
	printf("%s:%d: assertion '%s' failed.\n", file, line, cond);
	*(char *)0 = 0;
	exit(-1);
	while (1) ;
}

static void
cons_gets(char *s, unsigned long length)
{
	unsigned long n = 0;
	cons_echo(0);
	while (n < (length-1)) {
	    char c;
	    cons_read(&c, 1);
		if ((c <= 126) && (c >= 32)) {
			s[n] = c;
			cons_write(s + n, 1);
			n++;
		} else if ((c == '\n') || (c == 13)) {
			s[n] = 0;
			cons_write("\n", 1);
			cons_echo(1);
			return;
		} else if ((c == 8) || (c == 127)) {
			if (n > 0) {
				n--;
				printf("%c %c", 8, 8);
			}
		}
	}
	s[n] = 0;
	cons_echo(1);
}

/*******************************************************************************
 * Division 64 bits
 ******************************************************************************/
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

static void
setSeed(uint_fast64_t _s)
{
	_seed = _s;
}

static unsigned long
rand()
{
	return randBits(32);
}

/*******************************************************************************
 * Unmask interrupts for those who are working in kernel mode
 ******************************************************************************/
static void test_it()
{
	__asm__ volatile("pushfl; testl $0x200,(%%esp); jnz 0f; sti; nop; cli; 0: addl $4,%%esp\n":::"memory");
}

/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/
static int
dummy1(void *arg)
{
	printf("1");
	assert((int) arg == DUMMY_VAL);
	return 3;
}

static int
dummy1_2(void *arg)
{
	printf(" 5");
	assert((int) arg == DUMMY_VAL + 1);

	return 4;
}

static void
test1(void)
{
	int pid1;
	int r;
	int rval;

	pid1 = start(dummy1, 4000, 192, "paramRetour", (void *) DUMMY_VAL);
	assert(pid1 > 0);
	printf(" 2");
	r = waitpid(pid1, &rval);
	assert(r == pid1);
	assert(rval == 3);
	printf(" 3");
	pid1 = start(dummy1_2, 4000, 100, "paramRetour", (void *) (DUMMY_VAL + 1));
	assert(pid1 > 0);
	printf(" 4");
	r = waitpid(pid1, &rval);
	assert(r == pid1);
	assert(rval == 4);
	printf(" 6.\n");
}

/*******************************************************************************
 * Test 2
 *
 * kill() de fils suspendu pas demarre
 * waitpid() de ce fils termine par kill()
 * waitpid() de fils termine par exit()
 ******************************************************************************/
static int
dummy2(void *args)
{
	printf(" X");
	return (int)args;
}

static int
dummy2_2(void *args)
{
	printf(" 5");
	exit((int) args);
	assert(0);
	return 0;
}

static void
test2(void)
{
	int rval;
	int r;
	int pid1;
	int val = 45;

	printf("1");
	pid1 = start(dummy2, 4000, 100, "procKill", (void *) val);
	assert(pid1 > 0);
	printf(" 2");
	r = kill(pid1);
	assert(r == 0);
	printf(" 3");
	r = waitpid(pid1, &rval);
	assert(rval == 0);
	assert(r == pid1);
	printf(" 4");
	pid1 = start(dummy2_2, 4000, 192, "procExit", (void *) val);
	assert(pid1 > 0);
	printf(" 6");
	r = waitpid(pid1, &rval);
	assert(rval == val);
	assert(r == pid1);
	assert(waitpid(getpid(), &rval) < 0);
	printf(" 7.\n");
}

/*******************************************************************************
 * Test 3
 *
 * chprio() et ordre de scheduling
 * kill() d'un processus qui devient moins prioritaire
 ******************************************************************************/
static int
proc_prio4(void *arg)
{
	/* arg = priority of this proc. */
	int r;

	assert(getprio(getpid()) == (int) arg);
	printf("1");
	r = chprio(getpid(), 64);
	assert(r == (int) arg);
	printf(" 3");
	return 0;
}

static int
proc_prio5(void *arg)
{
	/* Arg = priority of this proc. */
	int r;

	assert(getprio(getpid()) == (int) arg);
	printf(" 7");
	r = chprio(getpid(), 64);
	assert(r == (int)arg);
	printf("error: I should have been killed\n");
	assert(0);
	return 0;
}

static void
test3(void)
{
	int pid1;
	int p = 192;
	int r;

	assert(getprio(getpid()) == 128);
	pid1 = start(proc_prio4, 4000, p, "prio", (void *) p);
	assert(pid1 > 0);
	printf(" 2");
	r = chprio(getpid(), 32);
	assert(r == 128);
	printf(" 4");
	r = chprio(getpid(), 128);
	assert(r == 32);
	printf(" 5");
	assert(waitpid(pid1, 0) == pid1);
	printf(" 6");

	assert(getprio(getpid()) == 128);
	pid1 = start(proc_prio5, 4000, p, "prio", (void *) p);
	assert(pid1 > 0);
	printf(" 8");
	r = kill(pid1);
	assert(r == 0);
	assert(waitpid(pid1, 0) == pid1);
	printf(" 9");
	r = chprio(getpid(), 32);
	assert(r == 128);
	printf(" 10");
	r = chprio(getpid(), 128);
	assert(r == 32);
	printf(" 11.\n");
}

/*******************************************************************************
 * Test 4
 *
 * Boucles d'attente active (partage de temps)
 * chprio()
 * kill() de processus de faible prio
 * kill() de processus deja mort
 ******************************************************************************/
static const int loop_count0 = 5000;
static const int loop_count1 = 10000;

static int
busy_loop1(void *arg)
{
	(void) arg;
	while (1) {
		int i, j;

		printf(" A");
		for (i=0; i<loop_count1; i++) {
			test_it();
			for (j=0; j<loop_count0; j++);
		}
	}
	return 0;
}

/* assume the process to suspend has a priority == 64 */
static int
busy_loop2(void *arg)
{
	int i;

	for (i = 0; i < 3; i++) {
		int k, j;

		printf(" B");
		for (k=0; k<loop_count1; k++) {
			test_it();
			for (j=0; j<loop_count0; j++);
		}
	}
	i = chprio((int) arg, 16);
	assert(i == 64);
	return 0;
}

static void
test4(void)
{
	int pid1, pid2;
	int r;
	int arg = 0;

	assert(getprio(getpid()) == 128);
	pid1 = start(busy_loop1, 4000, 64, "busy1", (void *) arg);
	assert(pid1 > 0);
	pid2 = start(busy_loop2, 4000, 64, "busy2", (void *) pid1);
	assert(pid2 > 0);
	printf("1 -");
	r = chprio(getpid(), 32);
	assert(r == 128);
	printf(" - 2");
	r = kill(pid1);
	assert(r == 0);
	assert(waitpid(pid1, 0) == pid1);
	r = kill(pid2);
	assert(r < 0); /* kill d'un processus zombie */
	assert(waitpid(pid2, 0) == pid2);
	printf(" 3");
	r = chprio(getpid(), 128);
	assert(r == 32);
	printf(" 4.\n");
}

/*******************************************************************************
 * Test 5
 *
 * Tests de quelques parametres invalides.
 * Certaines interdictions ne sont peut-etre pas dans la spec. Changez les pour
 * faire passer le test correctement.
 ******************************************************************************/
static int
no_run(void *arg)
{
	(void)arg;
	assert(0);
	return 1;
}

static int
waiter(void *arg)
{
	int pid = (int)arg;
	assert(kill(pid) == 0);
	assert(waitpid(pid, 0) < 0);
	return 1;
}

static void
test5(void)
{
	int pid1, pid2;
	int r;

	// Le processus 0 et la priorite 0 sont des parametres invalides
	assert(kill(0) < 0);
	assert(chprio(getpid(), 0) < 0);
	assert(getprio(getpid()) == 128);
	pid1 = start(no_run, 4000, 64, "norun", 0);
	assert(pid1 > 0);
	assert(kill(pid1) == 0);
	assert(kill(pid1) < 0); //pas de kill de zombie
	assert(chprio(pid1, 128) < 0); //changer la priorite d'un zombie
	assert(chprio(pid1, 64) < 0); //changer la priorite d'un zombie
	assert(waitpid(pid1, 0) == pid1);
	assert(waitpid(pid1, 0) < 0);
	pid1 = start(no_run, 4000, 64, "norun", 0);
	assert(pid1 > 0);
	pid2 = start(waiter, 4000, 65, "waiter", (void *)pid1);
	assert(pid2 > 0);
	assert(waitpid(pid2, &r) == pid2);
	assert(r == 1);
	assert(waitpid(pid1, &r) == pid1);
	assert(r == 0);
	printf("ok.\n");
}

/*******************************************************************************
 * Test 6
 *
 * Waitpid multiple.
 * Creation de processus avec differentes tailles de piles.
 ******************************************************************************/
extern int __proc6_1(void *arg);
extern int __proc6_2(void *arg);

__asm__(
".text\n"
".globl __proc6_1\n"
"__proc6_1:\n"
"movl $3,%eax\n"
"ret\n"
".globl __proc6_2\n"
"__proc6_2:\n"
"movl 4(%esp),%eax\n"
"pushl %eax\n"
"popl %eax\n"
"ret\n"
".previous\n"
);

static void
test6(void)
{
	int pid1, pid2, pid3;
	int ret;

	assert(getprio(getpid()) == 128);
	pid1 = start(__proc6_1, 0, 64, "proc6_1", 0);
	assert(pid1 > 0);
	pid2 = start(__proc6_2, 4, 66, "proc6_2", (void*)4);
	assert(pid2 > 0);
	pid3 = start(__proc6_2, 0xffffffff, 65, "proc6_3", (void*)5);
	assert(pid3 < 0);
	pid3 = start(__proc6_2, 8, 65, "proc6_3", (void*)5);
	assert(pid3 > 0);
	assert(waitpid(-1, &ret) == pid2);
	assert(ret == 4);
	assert(waitpid(-1, &ret) == pid3);
	assert(ret == 5);
	assert(waitpid(-1, &ret) == pid1);
	assert(ret == 3);
	assert(waitpid(pid1, 0) < 0);
	assert(waitpid(-1, 0) < 0);
	assert(waitpid(getpid(), 0) < 0);
	printf("ok.\n");
}

/*******************************************************************************
 * Test 7
 *
 * Test de l'horloge (ARR et ACE)
 * Tentative de determination de la frequence du processeur et de la
 * periode de scheduling
 ******************************************************************************/

static int
proc_timer1(void *arg)
{
	unsigned long quartz;
	unsigned long ticks;
	unsigned long dur;
	int i;

	(void) arg;

	clock_settings(&quartz, &ticks);
	dur = (quartz + ticks) / ticks;
	printf(" 2");
	for (i = 4; i < 8; i++) {
		wait_clock(current_clock() + dur);
		printf(" %d", i);
	}
	return 0;
}

static volatile unsigned long timer;

static int
proc_timer(void *arg)
{
	(void)arg;
	while (1) {
		unsigned long t = timer + 1;
		timer = t;
		while (timer == t) test_it();
	}
	while (1);
	return 0;
}

static int
sleep_pr1(void *args)
{
	(void)args;
	wait_clock(current_clock() + 2);
	printf(" not killed !!!");
	assert(0);
	return 1;
}

static void
test7(void)
{
	int pid1, pid2, r;
	unsigned long c0, c, quartz, ticks, dur;

	assert(getprio(getpid()) == 128);
	printf("1");
	pid1 = start(proc_timer1, 4000, 129, "timer", 0);
	assert(pid1 > 0);
	printf(" 3");
	assert(waitpid(-1, 0) == pid1);
	printf(" 8 : ");

	timer = 0;
	pid1 = start(proc_timer, 4000, 127, "timer1", 0);
	pid2 = start(proc_timer, 4000, 127, "timer2", 0);
	assert(pid1 > 0);
	assert(pid2 > 0);
	clock_settings(&quartz, &ticks);
	dur = 2 * quartz / ticks;
	test_it();
	c0 = current_clock();
	do {
		test_it();
		c = current_clock();
	} while (c == c0);
	wait_clock(c + dur);
	assert(kill(pid1) == 0);
	assert(waitpid(pid1, 0) == pid1);
	assert(kill(pid2) == 0);
	assert(waitpid(pid2, 0) == pid2);
	printf("%lu changements de contexte sur %lu tops d'horloge", timer, dur);
	pid1 = start(sleep_pr1, 4000, 192, "sleep", 0);
	assert(pid1 > 0);
	assert(kill(pid1) == 0);
	assert(waitpid(pid1, &r) == pid1);
	assert(r == 0);
	printf(".\n");
}

/*******************************************************************************
 * Test 8
 *
 * Creation de processus se suicidant en boucle. Test de la vitesse de creation
 * de processus.
 ******************************************************************************/
static int
suicide(void *arg)
{
	(void)arg;
	kill(getpid());
	assert(0);
	return 0;
}

static int
suicide_launcher(void *arg)
{
	int pid1;
	(void)arg;
	pid1 = start(suicide, 4000, 192, "suicide_launcher", 0);
	assert(pid1 > 0);
	return pid1;
}

static void
test8(void)
{
	unsigned long long tsc1;
	unsigned long long tsc2;
	int i, r, pid, count;

	assert(getprio(getpid()) == 128);

	/* Le petit-fils va passer zombie avant le fils mais ne pas
	etre attendu par waitpid. Un nettoyage automatique doit etre
	fait. */
	pid = start(suicide_launcher, 4000, 129, "suicide_launcher", 0);
	assert(pid > 0);
	assert(waitpid(pid, &r) == pid);
	assert(chprio(r, 192) < 0);

	count = 0;
	__asm__ __volatile__("rdtsc":"=A"(tsc1));
	do {
		for (i=0; i<10; i++) {
			pid = start(suicide_launcher, 4000, 200, "suicide_launcher", 0);
			assert(pid > 0);
			assert(waitpid(pid, 0) == pid);
		}
		test_it();
		count += i;
		__asm__ __volatile__("rdtsc":"=A"(tsc2));
	} while ((tsc2 - tsc1) < 1000000000);
	printf("%lu cycles/process.\n", (unsigned long)div64(tsc2 - tsc1, 2 * count, 0));
}

/*******************************************************************************
 * Test 9
 *
 * Test de la sauvegarde des registres dans les appels systeme et interruptions
 ******************************************************************************/
static int
nothing(void *arg)
{
	(void)arg;
	return 0;
}

int __err_id = 0;

extern void
__test_error(void)
{
	(void)nothing;
	printf("assembly check failed, id = %d\n", __err_id);
	exit(1);
}

__asm__(
".text\n"
".globl __test_valid_regs1\n"
"__test_valid_regs1:\n"

"pushl %ebp; movl %esp, %ebp; pushal\n"
"movl 8(%ebp),%ebx\n"
"movl 12(%ebp),%edi\n"
"movl 16(%ebp),%esi\n"
"movl %ebp,1f\n"
"movl %esp,2f\n"

"pushl $0\n"
"pushl $3f\n"
"pushl $192\n"
"pushl $4000\n"
"pushl $nothing\n"
"call start\n"
"addl $20,%esp\n"
"movl $1,__err_id\n"
"testl %eax,%eax\n"
"jle 0f\n"
"pushl %eax\n"
"pushl $0\n"
"pushl %eax\n"
"call waitpid\n"
"addl $8,%esp\n"
"popl %ecx\n"
"movl $3,__err_id\n"
"cmpl %ecx,%eax\n"
"jne 0f\n"

"movl $4,__err_id\n"
"cmpl %esp,2f\n"
"jne 0f\n"
"movl $5,__err_id\n"
"cmpl %ebp,1f\n"
"jne 0f\n"
"movl $6,__err_id\n"
"cmpl 8(%ebp),%ebx\n"
"jne 0f\n"
"movl $7,__err_id\n"
"cmpl 12(%ebp),%edi\n"
"jne 0f\n"
"movl $8,__err_id\n"
"cmpl 16(%ebp),%esi\n"
"jne 0f\n"
"popal; leave\n"
"ret\n"
"0: jmp __test_error\n"
"0:\n"
"jmp 0b\n"
".previous\n"
".data\n"
"1: .long 0x12345678\n"
"2: .long 0x87654321\n"
"3: .string \"nothing\"\n"
".previous\n"
);

extern void
__test_valid_regs1(int a1, int a2, int a3);

__asm__(
".text\n"
".globl __test_valid_regs2\n"
"__test_valid_regs2:\n"

"pushl %ebp; movl %esp, %ebp; pushal\n"

"movl 8(%ebp),%eax\n"
"movl 12(%ebp),%ebx\n"
"movl 16(%ebp),%ecx\n"
"movl 20(%ebp),%edx\n"
"movl 24(%ebp),%edi\n"
"movl 28(%ebp),%esi\n"
"movl %ebp,1f\n"
"movl %esp,2f\n"

"movl $1,__err_id\n"
"3: testl $1,__it_ok\n"
"jnz 0f\n"

"3: pushfl\n"
"testl $0x200,(%esp)\n"
"jnz 4f\n"
"sti\n"
"nop\n"
"cli\n"
"4:\n"
"addl $4,%esp\n"
"testl $1,__it_ok\n"
"jz 3b\n"

"movl $2,__err_id\n"
"cmpl %esp,2f\n"
"jne 0f\n"
"movl $3,__err_id\n"
"cmpl %ebp,1f\n"
"jne 0f\n"
"movl $4,__err_id\n"
"cmpl 8(%ebp),%eax\n"
"jne 0f\n"
"movl $5,__err_id\n"
"cmpl 12(%ebp),%ebx\n"
"jne 0f\n"
"movl $6,__err_id\n"
"cmpl 16(%ebp),%ecx\n"
"jne 0f\n"
"movl $7,__err_id\n"
"cmpl 20(%ebp),%edx\n"
"jne 0f\n"
"movl $8,__err_id\n"
"cmpl 24(%ebp),%edi\n"
"jne 0f\n"
"movl $9,__err_id\n"
"cmpl 28(%ebp),%esi\n"
"jne 0f\n"
"popal; leave\n"
"ret\n"
"0: jmp __test_error\n"
"0:\n"
"jmp 0b\n"
".previous\n"
".data\n"
"1: .long 0x12345678\n"
"2: .long 0x87654321\n"
".previous\n"
);

static volatile int __it_ok;

extern void
__test_valid_regs2(int a1, int a2, int a3, int a4, int a5, int a6);

static int
test_regs2(void *arg)
{
	(void)arg;
	__it_ok = 0;
	__test_valid_regs2(rand(), rand(), rand(), rand(), rand(), rand());
	return 0;
}

static void
test9(void)
{
	int i;
	assert(getprio(getpid()) == 128);
	printf("1");
	for (i=0; i<1000; i++) {
		__test_valid_regs1(rand(), rand(), rand());
	}
	printf(" 2");
	for (i=0; i<25; i++) {
		int pid;
		__it_ok = 1;
		pid = start(test_regs2, 4000, 128, "test_regs2", 0);
		assert(pid > 0);
		while (__it_ok) test_it();
		__it_ok = 1;
		assert(waitpid(pid, 0) == pid);
	}
	printf(" 3.\n");
}

static void write(int fid, const char *buf, unsigned long len)
{
	unsigned long i;
	for (i=0; i<len; i++) {
		assert(psend(fid, buf[i]) == 0);
	}
}

static void read(int fid, char *buf, unsigned long len)
{
	unsigned long i;
	for (i=0; i<len; i++) {
		int msg;
		assert(preceive(fid, &msg) == 0);
		buf[i] = msg;
	}
}

/*******************************************************************************
 * Test 10
 *
 * Test d'utilisation d'une file comme espace de stockage temporaire.
 ******************************************************************************/
static void
test10(void)
{
	int fid;
	char *str = "abcde";
	unsigned long len = strlen(str);
	char buf[10];

	printf("1");
	assert((fid = pcreate(5)) >= 0);
	write(fid, str, len);
	printf(" 2");
	read(fid, buf, len);
	buf[len] = 0;
	assert(strcmp(str, buf) == 0);
	assert(pdelete(fid) == 0);
	printf(" 3.\n");
}

/*******************************************************************************
 * Test 11
 *
 * Mutex avec un semaphore, regle de priorite sur le mutex.
 ******************************************************************************/
struct sem {
	int fid;
};

static void
xwait(struct sem *s)
{
	assert(preceive(s->fid, 0) == 0);
}

static void
xsignal(struct sem *s)
{
	int count;
	assert(psend(s->fid, 1) == 0);
	assert(pcount(s->fid, &count) == 0);
	//assert(count == 1); XXX
	assert(count < 2);
}

static void
xscreate(struct sem *s)
{
	assert((s->fid = pcreate(2)) >= 0);
}

static void
xsdelete(struct sem *s)
{
	assert(pdelete(s->fid) == 0);
}

static int in_mutex = 0;

static int
proc_mutex(void *arg)
{
	struct sem *sem = arg;
	int p = getprio(getpid());
	int msg;

	switch (p) {
	case 130:
		msg = 2;
		break;
	case 132:
		msg = 3;
		break;
	case 131:
		msg = 4;
		break;
	case 129:
		msg = 5;
		break;
	default:
		msg = 15;
	}
	printf(" %d", msg);
	xwait(sem);
	printf(" %d", 139 - p);
	assert(!(in_mutex++));
	chprio(getpid(), 16);
	chprio(getpid(), p);
	in_mutex--;
	xsignal(sem);
	return 0;
}

static void
test11(void)
{
	struct sem sem;
	int pid1, pid2, pid3, pid4;

	assert(getprio(getpid()) == 128);
	xscreate(&sem);
	printf("1");
	pid1 = start(proc_mutex, 4000, 130, "proc_mutex", &sem);
	pid2 = start(proc_mutex, 4000, 132, "proc_mutex", &sem);
	pid3 = start(proc_mutex, 4000, 131, "proc_mutex", &sem);
	pid4 = start(proc_mutex, 4000, 129, "proc_mutex", &sem);
	assert(pid1 > 0);
	assert(pid2 > 0);
	assert(pid3 > 0);
	assert(pid4 > 0);
	assert(chprio(getpid(), 160) == 128);
	printf(" 6");
	xsignal(&sem);
	assert(waitpid(-1, 0) == pid2);
	assert(waitpid(-1, 0) == pid3);
	assert(waitpid(-1, 0) == pid1);
	assert(waitpid(-1, 0) == pid4);
	assert(waitpid(-1, 0) < 0);
	assert(chprio(getpid(), 128) == 160);
	xsdelete(&sem);
	printf(" 11.\n");
}

/*******************************************************************************
 * Test 12
 *
 * Tests de rendez-vous sur une file de taille 1.
 ******************************************************************************/
static int
rdv_proc(void *arg)
{
	int fid = (int) arg;
	int msg;
	int count;

	printf(" 2");
	assert(psend(fid, 3) == 0); /* Depose dans le tampon */
	printf(" 3");
	assert((pcount(fid, &count) == 0) && (count == 1));
	assert(psend(fid, 4) == 0); /* Bloque tampon plein */
	printf(" 5");
	assert((pcount(fid, &count) == 0) && (count == 1));
	assert(preceive(fid, &msg) == 0); /* Retire du tampon */
	assert(msg == 4);
	printf(" 6");
	assert(preceive(fid, &msg) == 0); /* Bloque tampon vide. */
	assert(msg == 5);
	printf(" 8");
	assert((pcount(fid, &count) == 0) && (count == 0));
	return 0;
}

static void
test12(void)
{
	int fid;
	int pid;
	int msg;
	int count;

	assert(getprio(getpid()) == 128);
	assert((fid = pcreate(1)) >= 0);
	printf("1");
	pid = start(rdv_proc, 4000, 130, "rdv_proc", (void *)fid);
	assert(pid > 0);
	printf(" 4");
	assert((pcount(fid, &count) == 0) && (count == 2));
	assert(preceive(fid, &msg) == 0); /* Retire du tampon et debloque un emetteur. */
	assert(msg == 3);
	printf(" 7");
	assert((pcount(fid, &count) == 0) && (count == -1));
	assert(psend(fid, 5) == 0); /* Pose dans le tampon. */
	printf(" 9");
	assert(psend(fid, 6) == 0); /* Pose dans le tampon. */
	assert(preceive(fid, &msg) == 0); /* Retire du tampon. */
	assert(msg == 6);
	assert(pdelete(fid) == 0);
	assert(psend(fid, 2) < 0);
	assert(preceive(fid, &msg) < 0);
	assert(waitpid(-1, 0) == pid);
	printf(" 10.\n");
}

/*******************************************************************************
 * Test 13
 *
 * Teste l'ordre entre les processus emetteurs et recepteurs sur une file.
 * Teste le changement de priorite d'un processus bloque sur une file.
 ******************************************************************************/
struct psender {
	int fid;
	const char *data;
};

static int
psender(void *arg)
{
	struct psender *ps = arg;
	int i;
	int n = strlen(ps->data);

	for(i=0; i<n; i++) {
		assert(psend(ps->fid, ps->data[i]) == 0);
	}
	return 0;
}

static int
preceiver(void *arg)
{
	struct psender *ps = arg;
	int i, msg;
	int n = strlen(ps->data);

	for(i=0; i<n; i++) {
		assert(preceive(ps->fid, &msg) == 0);
		assert(msg == ps->data[i]);
	}
	return 0;
}

static void
test13(void)
{
	struct psender ps1, ps2, ps3;
	int pid1, pid2, pid3;
	int fid = pcreate(3);
	int i, msg;

	printf("1");
	assert(getprio(getpid()) == 128);
	assert(fid >= 0);
	ps1.fid = ps2.fid = ps3.fid = fid;
	ps1.data = "abcdehm";
	ps2.data = "il";
	ps3.data = "fgjk";
	pid1 = start(psender, 4000, 131, "psender1", &ps1);
	pid2 = start(psender, 4000, 130, "psender2", &ps2);
	pid3 = start(psender, 4000, 129, "psender3", &ps3);
	for (i=0; i<2; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'a' + i);
	}
	chprio(pid1, 129);
	chprio(pid3, 131);
	for (i=0; i<2; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'c' + i);
	}
	chprio(pid1, 127);
	chprio(pid2, 126);
	chprio(pid3, 125);
	for (i=0; i<6; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'e' + i);
	}
	chprio(pid1, 125);
	chprio(pid3, 127);
	for (i=0; i<3; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'k' + i);
	}
	assert(waitpid(pid3, 0) == pid3); //XXX assert(waitpid(-1, 0) == pid3); ???
	assert(waitpid(-1, 0) == pid2);
	assert(waitpid(-1, 0) == pid1);
	printf(" 2");

	ps1.data = "abej";
	ps2.data = "fi";
	ps3.data = "cdgh";
	pid1 = start(preceiver, 4000, 131, "preceiver1", &ps1);
	pid2 = start(preceiver, 4000, 130, "preceiver2", &ps2);
	pid3 = start(preceiver, 4000, 129, "preceiver3", &ps3);
	for (i='a'; i<='b'; i++) {
		assert(psend(fid, i) == 0);
	}
	chprio(pid1, 129);
	chprio(pid3, 131);
	for (i='c'; i<='d'; i++) {
		assert(psend(fid, i) == 0);
	}
	chprio(pid1, 127);
	chprio(pid2, 126);
	chprio(pid3, 125);
	for (i='e'; i<='j'; i++) {
		assert(psend(fid, i) == 0);
	}
	chprio(pid1, 125);
	chprio(pid3, 127);
	assert(waitpid(-1, 0) == pid3);
	assert(waitpid(-1, 0) == pid2);
	assert(waitpid(-1, 0) == pid1);
	assert(pdelete(fid) == 0);
	printf(" 3.\n");
}

/*******************************************************************************
 * Test 14
 *
 * Tests de preset et pdelete
 ******************************************************************************/
static int
psender1(void *arg)
{
	int fid1 = (int)arg;
	int fid2;
	int msg;

	printf(" 2");
	assert(preceive(fid1, &fid2) == 0);
	assert(psend(fid1, fid2) == 0);
	fid2 -= 42;
	assert(psend(fid1, 1) == 0);
	assert(psend(fid1, 2) == 0);
	assert(psend(fid1, 3) == 0);
	assert(psend(fid1, 4) == 0);
	assert(psend(fid1, 5) < 0);
	printf(" 6");
	assert(psend(fid1, 12) < 0);
	printf(" 9");
	assert(psend(fid1, 14) < 0);
	assert(preceive(fid2, &msg) < 0);
	printf(" 12");
	assert(preceive(fid2, &msg) < 0);
	assert(preceive(fid2, &msg) < 0);
	return 0;
}

static int
psender2(void *arg)
{
	int fid1 = (int)arg;
	int fid2;
	int msg;

	printf(" 3");
	assert(preceive(fid1, &fid2) == 0);
	fid2 -= 42;
	assert(psend(fid1, 6) < 0);
	printf(" 5");
	assert(psend(fid1, 7) == 0);
	assert(psend(fid1, 8) == 0);
	assert(psend(fid1, 9) == 0);
	assert(psend(fid1, 10) == 0);
	assert(psend(fid1, 11) < 0);
	printf(" 8");
	assert(psend(fid1, 13) < 0);
	assert((preceive(fid2, &msg) == 0) && (msg == 15));
	assert(preceive(fid2, &msg) < 0);
	printf(" 11");
	assert(preceive(fid2, &msg) < 0);
	assert(preceive(fid2, &msg) < 0);
	return 0;
}

static void
test14(void)
{
	int pid1, pid2;
	int fid1 = pcreate(3);
	int fid2 = pcreate(3);
	int msg;

	/* Bravo si vous n'etes pas tombe dans le piege. */
	assert(pcreate(1073741827) < 0);

	printf("1");
	assert(getprio(getpid()) == 128);
	assert(fid1 >= 0);
	assert(psend(fid1, fid2 + 42) == 0);
	pid1 = start(psender1, 4000, 131, "psender1", (void *)fid1);
	pid2 = start(psender2, 4000, 130, "psender2", (void *)fid1);
	assert((preceive(fid1, &msg) == 0) && (msg == 1));
	assert(chprio(pid2, 132) == 130);
	printf(" 4");
	assert(preset(fid1) == 0);
	assert((preceive(fid1, &msg) == 0) && (msg == 7));
	printf(" 7");
	assert(pdelete(fid1) == 0);
	printf(" 10");
	assert(psend(fid2, 15) == 0);
	assert(preset(fid2) == 0);
	printf(" 13");
	assert(pdelete(fid2) == 0);
	assert(pdelete(fid2) < 0);
	assert(waitpid(pid2, 0) == pid2); //XXX assert(waitpid(-1, 0) == pid2); ???
	assert(waitpid(-1, 0) == pid1);
	printf(".\n");
}

/*******************************************************************************
 * Test 15
 *
 * Tuer des processus en attente sur file
 ******************************************************************************/
static int
pmsg1(void *arg)
{
	int fid1 = (int)arg;

	printf(" 2");
	assert(psend(fid1, 1) == 0);
	assert(psend(fid1, 2) == 0);
	assert(psend(fid1, 3) == 0);
	assert(psend(fid1, 4) == 0);
	assert(psend(fid1, 5) == 457);
	return 1;
}

static int
pmsg2(void *arg)
{
	int fid1 = (int)arg;

	printf(" 3");
	assert(psend(fid1, 6) == 0);
	assert(psend(fid1, 7) == 457);
	return 1;
}

static void
test15(void)
{
	int pid1, pid2, fid1;
	int msg;
	int count = 1;
	int r = 1;

	assert((fid1 = pcreate(3)) >= 0);
	printf("1");
	assert(getprio(getpid()) == 128);
	pid1 = start(pmsg1, 4000, 131, "pmsg1", (void *)fid1);
	assert(pid1 > 0);
	pid2 = start(pmsg2, 4000, 130, "pmsg2", (void *)fid1);
	assert(pid2 > 0);

	assert((preceive(fid1, &msg) == 0) && (msg == 1));
	assert(kill(pid1) == 0);
	assert(kill(pid1) < 0);
	assert((preceive(fid1, &msg) == 0) && (msg == 2));
	assert(kill(pid2) == 0);
	assert(kill(pid2) < 0);
	assert(preceive(fid1, &msg) == 0);
	assert(msg == 3);
	assert(preceive(fid1, &msg) == 0);
	assert(msg == 4);
	assert(preceive(fid1, &msg) == 0);
	assert(msg == 6);
	assert(pcount(fid1, &count) == 0);
	assert(count == 0);
	assert(waitpid(pid1, &r) == pid1);
	assert(r == 0);
	r = 1;
	assert(waitpid(-1, &r) == pid2);
	assert(r == 0);
	assert(pdelete(fid1) == 0);
	assert(pdelete(fid1) < 0);
	printf(" 4.\n");
}

/*******************************************************************************
 * Test 16
 *
 * Test sur des files de diverses tailles et test d'endurance
 ******************************************************************************/
struct tst16 {
	int count;
	int fid;
};

static int
proc_16_1(void *arg)
{
	struct tst16 *p = arg;
	int i, msg;
	for (i=0; i<=p->count; i++) {
		assert(preceive(p->fid, &msg) == 0);
		assert(msg == i);
		test_it();
	}
	return 0;
}

static int
proc_16_2(void *arg)
{
	struct tst16 *p = arg;
	int i, msg;
	for (i=0; i<p->count; i++) {
		assert(preceive(p->fid, &msg) == 0);
		test_it();
	}
	return 0;
}

static int
proc_16_3(void *arg)
{
	struct tst16 *p = arg;
	int i;
	for (i=0; i<p->count; i++) {
		assert(psend(p->fid, i) == 0);
		test_it();
	}
	return 0;
}

static void
test16(void)
{
	int i, count, fid, pid;
	struct tst16 p;
	int procs = 10;
	int pids[2*procs];

	assert(getprio(getpid()) == 128);
	for (count=1; count<=100; count++) {
		fid = pcreate(count);
		assert(fid >= 0);
		p.count = count;
		p.fid = fid;
		pid = start(proc_16_1, 2000, 128, "proc_16_1", &p);
		assert(pid > 0);
		for (i=0; i<=count; i++) {
			assert(psend(fid, i) == 0);
			test_it();
		}
		assert(waitpid(pid, 0) == pid);
		assert(pdelete(fid) == 0);
	}

	p.count = 20000;
	fid = pcreate(50);
	assert(fid >= 0);
	p.fid = fid;
	for (i=0; i<procs; i++) {
		pid = start(proc_16_2, 2000, 127, "proc_16_2", &p);
		assert(pid > 0);
		pids[i] = pid;
	}
	for (i=0; i<procs; i++) {
		pid = start(proc_16_3, 2000, 127, "proc_16_3", &p);
		assert(pid > 0);
		pids[procs + i] = pid;
	}
	for (i=0; i<2*procs; i++) {
		assert(waitpid(pids[i], 0) == pids[i]);
	}
	assert(pcount(fid, &count) == 0);
	assert(count == 0);
	assert(pdelete(fid) == 0);
	printf("ok.\n");
}

/*******************************************************************************
 * Test 17
 *
 * On teste des limites de capacite
 ******************************************************************************/
static int ids[1200];

static const int heap_len = 15 << 20;

static int
proc_return(void *arg)
{
	return (int)arg;
}

static void
test17(void)
{
	int i, n, nx;
	int l = sizeof(ids) / sizeof(int);
	int count;
	int prio;
	unsigned long ssize;

	n = 0;
	while (1) {
		int fid = pcreate(1);
		if (fid < 0) break;
		ids[n++] = fid;
		if (n == l) {
			assert(!"Maximum number of queues too high !");
		}
		test_it();
	}
	for (i=0; i<n; i++) {
		assert(pdelete(ids[i]) == 0);
		test_it();
	}
	for (i=0; i<n; i++) {
		int fid = pcreate(1);
		assert(fid >= 0);
		ids[i] = fid;
		test_it();
	}
	assert(pcreate(1) < 0);
	for (i=0; i<n; i++) {
		assert(pdelete(ids[i]) == 0);
		test_it();
	}
	printf("%d", n);

	for (i=0; i<n; i++) {
		int fid = pcreate(1);
		assert(fid >= 0);
		assert(psend(fid, i) == 0);
		ids[i] = fid;
		test_it();
	}
	assert(pcreate(1) < 0);
	for (i=0; i<n; i++) {
		int msg;
		assert(preceive(ids[i], &msg) == 0);
		assert(msg == i);
		assert(pdelete(ids[i]) == 0);
		test_it();
	}

	count = heap_len / sizeof(int);
	count /= n - 1;
	nx = 0;
	while (nx < n) {
		int fid = pcreate(count);
		if (fid < 0) break;
		ids[nx++] = fid;
		test_it();
	}
	assert(nx < n);
	for (i=0; i<nx; i++) {
		assert(pdelete(ids[i]) == 0);
		test_it();
	}
	printf(" > %d", nx);

	prio = getprio(getpid());
	assert(prio == 128);
	n = 0;
	while (1) {
		int pid = start(no_run, 2000, 127, "no_run", 0);
		if (pid < 0) break;
		ids[n++] = pid;
		if (n == l) {
			assert(!"Maximum number of processes too high !");
		}
		test_it();
	}
	for (i=0; i<n; i++) {
		assert(kill(ids[i]) == 0);
		assert(waitpid(ids[i], 0) == ids[i]);
		test_it();
	}
	for (i=0; i<n; i++) {
		int pid = start(proc_return, 2000, 129, "return", (void *)i);
		assert(pid > 0);
		ids[i] = pid;
		test_it();
	}
	for (i=0; i<n; i++) {
		int retval;
		assert(waitpid(ids[i], &retval) == ids[i]);
		assert(retval == i);
		test_it();
	}
	printf(", %d", n);

	ssize = heap_len;
	ssize /= n - 1;
	nx = 0;
	while (nx < n) {
		int pid = start(proc_return, ssize, 127, "return", (void *)nx);
		if (pid < 0) break;
		ids[nx++] = pid;
		test_it();
	}
	assert(nx < n);
	for (i=0; i<nx; i++) {
		int retval;
		assert(waitpid(ids[i], &retval) == ids[i]);
		assert(retval == i);
		test_it();
	}
	printf(" > %d", nx);

	printf(".\n");
}

/*******************************************************************************
 * Test 18
 *
 * Amusement : piratage !
 ******************************************************************************/
static char callhack[] = { 0xcd, 0x32, 0xc3 };

__asm__(
".text\n"
".globl __hacking\n"
"__hacking:\n"

"pushal\n"
"pushl %ds\n"
"pushl %es\n"
"movl $0x18,%eax\n"
"movl %eax,%ds\n"
"movl %eax,%es\n"
"cld\n"
"call __hacking_c\n"
"popl %es\n"
"popl %ds\n"
"popal\n"
"iret\n"
);

extern void
__hacking(void);

static __inline__ void 
outb(unsigned char value, unsigned short port)
{
	__asm__ __volatile__("outb %0, %1" : : "a" (value), "Nd" (port));
}

static __inline__ unsigned char 
inb(unsigned short port)
{
	unsigned char rega;
	__asm__ __volatile__("inb %1,%0" : "=a" (rega) : "Nd" (port));
	return rega;
}

static int
getpos(void)
{
	int pos;
	outb(0x0f, 0x3d4);
	pos = inb(0x3d4 + 1);
	outb(0x0e, 0x3d4);
	pos += inb(0x3d4 + 1) << 8;
	return pos;
}

static int firsttime = 1;

void
__hacking_c(void)
{
	static int pos;
	if (firsttime) {
		firsttime = 0;
		pos = getpos();
	} else {
		int pos2 = getpos();
		char *str = "          Kernel hacked ! :P          ";
		short *ptr = (short *)0xb8000;
		int p = pos;
		while (p > pos2) p-= 80;
		if ((p < 0) || (p >= 80 * 24)) p = 80 * 23;
		ptr += p;
		while (*str) {
			*ptr++ = ((128 + 4 * 16 + 15) << 8) + *str++;
		}
	}
}

static void
do_hack(void)
{
	firsttime = 1;
	((void (*)(void))callhack)();
	printf("nok.\n");
	((void (*)(void))callhack)();
}

static int
proc_18_1(void *args)
{
	printf("1 ");
	return (int)args;
}

static int
proc_18_2(void *args)
{
	printf("2 ");
	return (int)args;
}

static void
test18(void)
{
	unsigned long a = (unsigned long)__hacking;
	unsigned long a1 = 0x100000 + (a & 0xffff);
	unsigned long a2 = (a & 0xffff0000) + 0xee00;
	int pid1, pid2;
	int cs;

	__asm__ volatile ("movl %%cs,%%eax":"=a" (cs));
	if ((cs & 3) == 0) {
		printf("This test can not work at kernel level.\n");
		return;
	}
	pid1 = start(proc_18_1, 4000, 127, "proc_18_1", (void *)a1);
	pid2 = start(proc_18_2, 4000, 126, "proc_18_2", (void *)a2);
	assert(pid1 > 0);
	assert(pid2 > 0);
	if ((waitpid(pid1, (int *)0x1190) == pid1) &&
		(waitpid(pid2, (int *)0x1194) == pid2)) {
		do_hack();
		return;
	}
	waitpid(-1, 0);
	waitpid(-1, 0);
	cons_write((char *)0x100000, 50);
	assert(start(dummy2, 4000, 100, (void *)0x100000, 0) < 0);
	printf("3.\n");
}

/*******************************************************************************
 * Test 19
 *
 * Quelques processus lisent sur la console et transmettent leur terminaison
 * via une file.
 ******************************************************************************/
static int
cons_reader(void *arg)
{
	int fid = (int)arg;
	char c;
	cons_read(&c, 1);
	assert(psend(fid, 1) == 0);
	printf(" %d (%c)", 134 - getprio(getpid()), c);
	return 0;
}

static void
test19(void)
{
	int fid = pcreate(10);
	int pid1, pid2, pid3, pid4;
	int param;
	int count;

	assert(getprio(getpid()) == 128);
	assert(fid >= 0);
	cons_echo(0);
	pid1 = start(cons_reader, 4000, 130, "cons_reader", (void *)fid);
	assert(pid1 > 0);
	pid2 = start(cons_reader, 4000, 132, "cons_reader", (void *)fid);
	assert(pid2 > 0);
	pid3 = start(cons_reader, 4000, 131, "cons_reader", (void *)fid);
	assert(pid3 > 0);
	pid4 = start(cons_reader, 4000, 129, "cons_reader", (void *)fid);
	assert(pid4 > 0);
	printf("1");
	param = 4;
	while (param > 0) {
		unsigned long long t1, t2;
		int msg = 0;
		printf(".");
		__asm__ __volatile__("rdtsc":"=A"(t1));
		do {
			test_it();
			__asm__ __volatile__("rdtsc":"=A"(t2));
		} while ((t2 - t1) < 200000000);
		assert(psend(fid, 0) == 0);
		param++;
		do {
			assert(preceive(fid, &msg) == 0);
			param--;
		} while (msg);
	}
	assert(waitpid(pid2, 0) == pid2);
	assert(waitpid(pid3, 0) == pid3);
	assert(waitpid(pid1, 0) == pid1);
	assert(waitpid(pid4, 0) == pid4);
	cons_echo(1);
	assert(pcount(fid, &count) == 0);
	assert(count == 0);
	assert(pdelete(fid) == 0);
	printf(" 6.\n");
}

/*******************************************************************************
 * Test 20
 *
 * Le repas des philosophes.
 ******************************************************************************/
static char f[NR_PHILO]; /* tableau des fourchettes, contient soit 1 soit 0 selon si elle
			    est utilisee ou non */

static char bloque[NR_PHILO]; /* memorise l'etat du philosophe, contient 1 ou 0 selon que le philosophe
				 est en attente d'une fourchette ou non */

static struct sem mutex_philo; /* exclusion mutuelle */
static struct sem s[NR_PHILO]; /* un semaphore par philosophe */
static int etat[NR_PHILO];

static void
affiche_etat()
{
	int i;
	printf("%c", 13);
	for (i=0; i<NR_PHILO; i++) {
		unsigned long c;
		switch (etat[i]) {
		case 'm':
			c = 2;
			break;
		default:
			c = 4;
		}
		assert(c %2 == 0); // utilisation de c pour le compilo
		printf("%c", etat[i]);
	}
}

static void
waitloop(void)
{
	int j;
	for (j = 0; j < 5000; j++) {
		int l;
		test_it();
		for (l = 0; l < 5000; l++);
	}
}

static void
penser(long i)
{
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = 'p';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
	waitloop();
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = '-';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
}

static void
manger(long i)
{
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = 'm';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
	waitloop();
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = '-';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
}

static int
test(int i)
{
	/* les fourchettes du philosophe i sont elles libres ? */
	return ((!f[i] && (!f[(i + 1) % NR_PHILO])));
}

static void
prendre_fourchettes(int i)
{
	/* le philosophe i prend des fourchettes */

	xwait(&mutex_philo); /* Debut SC */

	if (test(i)) {  /* on tente de prendre 2 fourchette */
		f[i] = 1;
		f[(i + 1) % NR_PHILO] = 1;
		xsignal(&s[i]);
	} else
		bloque[i] = 1;

	xsignal(&mutex_philo); /* FIN SC */
	xwait(&s[i]); /* on attend au cas o on ne puisse pas prendre 2 fourchettes */
}

static void
poser_fourchettes(int i)
{

	xwait(&mutex_philo); /* DEBUT SC */

	if ((bloque[(i + NR_PHILO - 1) % NR_PHILO]) && (!f[(i + NR_PHILO - 1) % NR_PHILO])) {
		f[(i + NR_PHILO - 1) % NR_PHILO] = 1;
		bloque[(i + NR_PHILO - 1) % NR_PHILO] = 0;
		xsignal(&s[(i + NR_PHILO - 1) % NR_PHILO]);
	} else
		f[i] = 0;

	if ((bloque[(i + 1) % NR_PHILO]) && (!f[(i + 2) % NR_PHILO])) {
		f[(i + 2) % NR_PHILO] = 1;
		bloque[(i + 1) % NR_PHILO] = 0;
		xsignal(&s[(i + 1) % NR_PHILO]);
	} else
		f[(i + 1) % NR_PHILO] = 0;

	xsignal(&mutex_philo); /* Fin SC */
}

static int
philosophe(void *arg)
{
	/* comportement d'un seul philosophe */
	int i = (int) arg;
	int k;

	for (k = 0; k < 6; k++) {
		prendre_fourchettes(i); /* prend 2 fourchettes ou se bloque */
		manger(i); /* le philosophe mange */
		poser_fourchettes(i); /* pose 2 fourchettes */
		penser(i); /* le philosophe pense */
	}
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = '-';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
	return 0;
}

static int
launch_philo()
{

	int i, pid;

	for (i = 0; i < NR_PHILO; i++) {
		pid = start(philosophe, 4000, 192, "philosophe", (void *) i);
		assert(pid > 0);
	}
	return 0;

}

static void
test20(void)
{
	int j, pid;

	xscreate(&mutex_philo); /* semaphore d'exclusion mutuelle */
	xsignal(&mutex_philo);
	for (j = 0; j < NR_PHILO; j++) {
		xscreate(s + j); /* semaphore de bloquage des philosophes */
		f[j] = 0;
		bloque[j] = 0;
		etat[j] = '-';
	}

	printf("\n");
	pid = start(launch_philo, 4000, 193, "Lanceur philosophes", 0);
	assert(pid > 0);
	assert(waitpid(pid, 0) == pid);
	printf("\n");
	xsdelete(&mutex_philo);
	for (j = 0; j < NR_PHILO; j++) {
		xsdelete(s + j);
	}
}

/*******************************************************************************
 * Fin des tests
 ******************************************************************************/

static void auto_test(void);

static void
quit(void)
{
	exit(0);
}

static struct {
	const char *name;
	void (*f) (void);
} commands[] = {
	{"1", test1},
	{"2", test2},
	{"3", test3},
	{"4", test4},
	{"5", test5},
	{"6", test6},
	{"7", test7},
	{"8", test8},
	{"9", test9},
	{"10", test10},
	{"11", test11},
	{"12", test12},
	{"13", test13},
	{"14", test14},
	{"15", test15},
	{"16", test16},
	{"17", test17},
	{"18", test18},
	{"19", test19},
	{"20", test20},
	{"si", sys_info},
	{"a", auto_test},
	{"auto", auto_test},
	{"q", quit},
	{"quit", quit},
	{"exit", quit},
	{0, 0},
};

static void
auto_test(void)
{
	int i = 0;

	while (commands[i].f != sys_info) {
		printf("Test %s : ", commands[i].name);
		commands[i++].f();
	}
}

int
test_run(int n)
{
	assert(getprio(getpid()) == 128);
	if ((n < 1) || (n > 20)) {
		printf("%d: unknown test\n", n);
	} else {
		commands[n - 1].f();
	}
	return 0;
}

int
test_proc(void *arg)
{
	char buffer[20];

	(void)arg;
	assert(getprio(getpid()) == 128);

	unsigned long flags;
	unsigned long cs;
	unsigned long long seed;

	__asm__ volatile("pushfl; popl %0; movl %%cs,%1\n":"=r" (flags), "=r" (cs) ::"memory");
	printf("EFLAGS = %#lx, CS = %#lx\n", flags, cs);

	__asm__ __volatile__("rdtsc":"=A"(seed));
	setSeed(seed);

	while (1) {
		int i = 0;
		printf("Test (1-20, auto) : ");
		cons_gets(buffer, 20);
		while (commands[i].name && strcmp(commands[i].name, buffer)) i++;
		if (!commands[i].name) {
			printf("%s: unknown test\n", buffer);
		} else {
			commands[i].f();
		}
	}
}
