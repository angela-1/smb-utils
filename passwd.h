/* Generate a 5 chars random password.
   Copyright (C) 2014 ZHAO Peng <shui.yunduo@gmail.com>
   
   This file is part of Samba utils.

   Samba utils is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Samba utils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>. */








/* Limit the length of password. */
#define PASSWD_LEN 5



/* Interface of generate password. */
extern char *gen_passwd (char *passwd, int seed);






