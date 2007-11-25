/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2005-2007  Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/UDPSocket.h>
#include"SipTransportDtlsUdp.h"

static std::list<std::string> pluginList;
static int initialized;

using namespace std;

extern "C" LIBMSIP_API
std::list<std::string> *mdtlsudp_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMSIP_API
MPlugin * mdtlsudp_LTX_getPlugin( MRef<Library*> lib ){
	return new SipTransportDtlsUdp( lib );
}


SipTransportDtlsUdp::SipTransportDtlsUdp( MRef<Library*> lib ) : SipTransport( lib ){
}

SipTransportDtlsUdp::~SipTransportDtlsUdp(){
}



/**
 *
 * @param externalPort  If the application wishes to override what port
 *   should be reported for the socket it is possible to specify such a port
 *   number here. This can be used for example to implement support for
 *   passing NATs with the help of a STUN server.
 */
MRef<SipSocketServer *> SipTransportDtlsUdp::createServer( MRef<SipSocketReceiver*> receiver, bool ipv6, const string &ipString, int32_t prefPort, MRef<CertificateSet *> cert_db, MRef<CertificateChain *> certChain  )
{
	int32_t port = prefPort;
	int triesLeft=10;
	MRef<SipSocketServer *> server;
	bool fail;

	do {
		fail=false;
		try {

			MRef<DatagramSocket *> sock = new UDPSocket( port, ipv6 );
			MRef<DatagramSocket *> dsock =
				DTLSSocket::create( sock, certChain->getFirst(), cert_db );
			server = new DatagramSocketServer( receiver, dsock );
			server->setExternalIp( ipString );


		} catch(const BindFailed &bf){
			fail=true;

			// If the port is already in use, try random port number in the range 2048 to 63488
			port = rand()%(0xFFFF-4096) + 2048; 
			triesLeft--;
			if (!triesLeft)
				throw;

		}
	}while(fail);

	return server;
}

uint32_t SipTransportDtlsUdp::getVersion() const{
	return 0x00000001;
}