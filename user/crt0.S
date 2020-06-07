/*
 * crt0.S
 *
 * Copyright (C) 2005 by Simon Nieuviarts
 *
 * User code entry point.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

	.text
	.global	entry
entry:
	/* Call the first user process */
	jmp	user_start

/* Unimplemented functions.
You have to implement them it in ".c" files. As these symbols are marked as
"weak", they will no more be used as soon as you write your implementations.
*/
	.weak user_start
user_start:
	xorl	%eax,%eax
	ret

#define UNIMPLEMENTED(X) \
	.weak X			;\
X##:\
	ret

UNIMPLEMENTED(console_putbytes)
	
UNIMPLEMENTED(cons_write)
UNIMPLEMENTED(cons_read)
UNIMPLEMENTED(exit)
UNIMPLEMENTED(start)
UNIMPLEMENTED(waitpid)
UNIMPLEMENTED(kill)
UNIMPLEMENTED(getpid)
UNIMPLEMENTED(getprio)
UNIMPLEMENTED(chprio)
UNIMPLEMENTED(screate)
UNIMPLEMENTED(scount)
UNIMPLEMENTED(signal)
UNIMPLEMENTED(signaln)
UNIMPLEMENTED(wait)
UNIMPLEMENTED(sreset)
UNIMPLEMENTED(sdelete)
UNIMPLEMENTED(try_wait)
UNIMPLEMENTED(sys_info)
UNIMPLEMENTED(pcreate)
UNIMPLEMENTED(psend)
UNIMPLEMENTED(pdelete)
UNIMPLEMENTED(pcount)
UNIMPLEMENTED(preceive)
UNIMPLEMENTED(preset)
UNIMPLEMENTED(clock_settings)
UNIMPLEMENTED(wait_clock)
UNIMPLEMENTED(current_clock)
UNIMPLEMENTED(cons_echo)




