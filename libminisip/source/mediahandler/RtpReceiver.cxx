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


/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<libminisip/RtpReceiver.h>

#include<config.h>

#include<libminisip/MediaStream.h>
#include<libminisip/IpProvider.h>
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/UDPSocket.h>
#include<libmutil/Thread.h>
#include<libminisip/SRtpPacket.h>
#include<libminisip/Codec.h>
#include<iostream>

#include<stdio.h>
#include<sys/types.h>

#ifdef WIN32
#include<winsock2.h>
#endif

#ifdef _MSC_VER

#else
#include<sys/time.h>
#include<unistd.h>
#endif

using namespace std;

RtpReceiver::RtpReceiver( MRef<IpProvider *> ipProvider){
	try{
		socket = new UDPSocket();
	}
	catch( NetworkException * exc ){
		// FIXME: do something nice
		merr << "Minisip could not create a UDP socket!" << end;
		merr << "Check your network settings." << end;
		exit( 1 );
	}
                
        externalPort = ipProvider->getExternalPort( socket );

	kill = false;

	thread = new Thread(this);
}

RtpReceiver::~RtpReceiver(){
	kill = true;
        thread->join();

        delete thread;

	socket->close();
}

void RtpReceiver::registerMediaStream( MRef<MediaStreamReceiver *> mediaStream ){
	mediaStreamsLock.lock();
	mediaStreams.push_back( mediaStream );
	mediaStreamsLock.unlock();
}

void RtpReceiver::unregisterMediaStream( MRef<MediaStreamReceiver *> mediaStream ){
			    cerr << "Before taking lock" << endl;
	mediaStreamsLock.lock();
			    cerr << "After taking lock" << endl;
	mediaStreams.remove( mediaStream );
	mediaStreamsLock.unlock();
}

uint16_t RtpReceiver::getPort(){
	return externalPort;
}

MRef<UDPSocket *> RtpReceiver::getSocket(){
	return socket;
}
			
void RtpReceiver::run(){
	SRtpPacket * packet;
	
	while( !kill ){
		list< MRef<MediaStreamReceiver *> >::iterator i;
		fd_set rfds;
		struct timeval tv;
		int ret = -1;

		FD_ZERO( &rfds );
		FD_SET( socket->getFd(), &rfds );

		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		
		while( ret < 0 ){
			ret = select( socket->getFd() + 1, &rfds, NULL, NULL, &tv );
			/*if( ret < 0 ){
				FIXME: do something
			}*/

		}

		if( ret == 0 /* timeout */ ){
			continue;
		}

		try{
			packet = SRtpPacket::readPacket( **socket );
		}

		catch (NetworkException * exc ){
			delete exc;
			continue;
		}

		if( packet == NULL ){
			continue;
		}

		mediaStreamsLock.lock();
		
		for( i = mediaStreams.begin(); i != mediaStreams.end(); i++ ){
				// pn501 Rewritten to account for multiple codecs
			// pn501 Added "Current"
			//if( (*i)->getCurrentRtpPayloadType() == packet->getHeader().getPayloadType() ){
			//	(*i)->handleRtpPacket( packet );
			//}
			// pn501 New version:
			std::list<uint8_t> list = (*i)->getAllRtpPayloadTypes();
			std::list<uint8_t>::iterator iList;
			for( iList = list.begin(); iList != list.end(); iList ++ ){
				uint8_t n = packet->getHeader().getPayloadType();
				if ( (*iList) == packet->getHeader().getPayloadType() ) {
					(*i)->handleRtpPacket( packet );
					break;
				}
			}
		}
		
		mediaStreamsLock.unlock();

//		delete packet;
	}
}