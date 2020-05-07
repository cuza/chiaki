/*
 * This file is part of Chiaki.
 *
 * Chiaki is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Chiaki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Chiaki.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CHIAKI_HOST_H
#define CHIAKI_HOST_H

#include <string>
#include <map>
#include <netinet/in.h>
#include <SDL2/SDL.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}


#include <chiaki/log.h>
#include <chiaki/regist.h>
#include <chiaki/discovery.h>
#include <chiaki/opusdecoder.h>
#include <chiaki/controller.h>

#include "exception.h"

class DiscoveryManager;
static void Discovery(ChiakiDiscoveryHost*, void*);
static void Regist(ChiakiRegistEvent*, void*);
static bool Video(uint8_t*, size_t, void*);
static void InitAudio(unsigned int, unsigned int, void*);
static void Audio(int16_t*, size_t, void*);


class Host
{
	private:
		ChiakiLog *log = NULL;
		//video config
		ChiakiVideoResolutionPreset video_resolution = CHIAKI_VIDEO_RESOLUTION_PRESET_720p;
		ChiakiVideoFPSPreset video_fps = CHIAKI_VIDEO_FPS_PRESET_30;
		// info from discovery manager
		int system_version;
		int device_discovery_protocol_version;
		std::string state;
		std::string host_name;
		std::string host_type;
		std::string host_id;
		// user info
		std::string psn_online_id;
		std::string psn_account_id;
		// info from regist/settings
		std::string ap_ssid;
		std::string ap_bssid;
		std::string ap_key;
		std::string ap_name;
		std::string ps4_nickname;
        // mac address = 48 bits
		uint8_t ps4_mac[6];
		char rp_regist_key[CHIAKI_SESSION_AUTH_SIZE];
		uint32_t rp_key_type;
		uint8_t rp_key[0x10];
		// manage stream session
		ChiakiSession session;
		ChiakiOpusDecoder opus_decoder;
		ChiakiConnectVideoProfile video_profile;
		ChiakiControllerState keyboard_state;
		unsigned int audio_buffer_size;
		// share video from main loop and host obj
		AVCodec *codec;
		AVCodecContext *codec_context;
		struct SwsContext * sws_context = NULL;
		// audio vars
		SDL_AudioDeviceID audio_device_id;
		// use friend class and methodes
		// to allow private var access
		friend class DiscoveryManager;
		friend class Settings;
	public:
		// internal state
		bool discovered = false;
		bool registered = false;
		// rp_key_data is true when rp_key, rp_regist_key, rp_key_type
		bool rp_key_data = false;
		std::string host_addr;
		// share picture frame
		// with main function
		AVFrame *pict;
		Host(ChiakiLog *log):log(log) {};
		Host(){};
		static Host * GetOrCreate(ChiakiLog*, std::map<std::string, Host>*, std::string*);
		void InitVideo();
		int Register(std::string pin);
		int Wakeup();
		int ConnectSession();
		void StartSession();
		bool ReadGameKeys(SDL_Event*, ChiakiControllerState*);
		void SendFeedbackState(ChiakiControllerState*);
		// callback methods
		void RegistCB(ChiakiRegistEvent*);
		bool VideoCB(uint8_t*, size_t);
		void InitAudioCB(unsigned int, unsigned int);
		void AudioCB(int16_t*, size_t);
};

#endif
