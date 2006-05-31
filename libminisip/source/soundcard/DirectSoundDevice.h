/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Cesc Santasusana <c e s c DOT s a n t a [AT} g m a i l DOT c o m>
*/

#ifndef DIRECT_SOUND_DEVICE_H
#define DIRECT_SOUND_DEVICE_H

#include<libminisip/libminisip_config.h>

#include<libminisip/soundcard/SoundDevice.h>

#include<dsound.h>

//#define NUM_PLAY_NOTIFICATIONS  16

class DirectSoundDevice : public SoundDevice{
	public:
		DirectSoundDevice( std::string fileName );

		virtual ~DirectSoundDevice();

		virtual int openRecord( int samplingRate, int nChannels, int format );
		virtual int openPlayback( int samplingRate, int nChannels, int format );
		
		virtual int closeRecord();
		virtual int closePlayback();

		virtual int readFromDevice( byte_t * buffer, uint32_t nSamples );
		virtual int writeToDevice( byte_t * buffer, uint32_t nSamples );

		virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples );
		virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples );
		
		virtual void sync();

	private:

		HRESULT setCaptureNotificationPoints(LPDIRECTSOUNDCAPTUREBUFFER8 pDSCB);

		LPDIRECTSOUNDFULLDUPLEX dsDuplexInterfaceHandle;
			
		LPDIRECTSOUNDBUFFER8        outputBufferHandle; //secondary directsound buffer
		LPDIRECTSOUNDCAPTUREBUFFER8        inputBufferHandle; 
		
#define cEvents  2
		HANDLE     inputSoundEvent[cEvents];	
		
};

#endif