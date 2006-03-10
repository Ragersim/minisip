/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include<config.h>

#include<libmutil/Mutex.h>
#include<libmutil/merror.h>

#include<stdio.h>
#include<stdlib.h>
#include<libmutil/massert.h>

// BSD 5.x: malloc.h has been replaced by stdlib.h
// #include<malloc.h>

#if HAVE_PTHREAD_H
#include<pthread.h>
#endif

#include<iostream>
using namespace std;

///See: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/mutex_objects.asp

//TODO: Check return values
Mutex::Mutex(){
	createMutex();
}


//FIXME: Verify and comment this method!
Mutex& Mutex::operator=(const Mutex &){
	//Do not copy the Mutex reference - keep our own.
	return *this;
}

Mutex::Mutex(const Mutex &){
	createMutex();
}

void Mutex::createMutex(){
	handle_ptr = new pthread_mutex_t;
	pthread_mutex_init( (pthread_mutex_t*)handle_ptr, NULL);
}

Mutex::~Mutex(){
	pthread_mutex_destroy((pthread_mutex_t*)handle_ptr);
	delete (pthread_mutex_t*)handle_ptr;
}


void Mutex::lock(){
	int ret=pthread_mutex_lock((pthread_mutex_t*)handle_ptr);
	if (ret!=0){
		merror("pthread_mutex_lock");
		exit(1);
	}
}

void Mutex::unlock(){
	int ret=pthread_mutex_unlock((pthread_mutex_t*)handle_ptr);
	if (ret!=0){
		merror("pthread_mutex_unlock");
		exit(1);
	}
}