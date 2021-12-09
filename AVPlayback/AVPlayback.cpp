/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2019 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include <string>

#include "AVPlayback.h"
#include "avplayback.hpp"
#include "manager.hpp"

#include "utils.h"
#include "dsUtl.h"
#include "dsError.h"
#include "dsMgr.h"
//methods
#define AVPLAYBACK_METHOD_GET_VIDEO_ASPECT_RATIO  "getVideoAspectRatio"
//events
#define AVPLAYBACK_EVENT_ON_VIDEO_ASPECT_RATIO_CHANGED "videoAspectRatioChanged"
namespace WPEFramework
{
    namespace Plugin
    {
        SERVICE_REGISTRATION(AVPlayback, 1, 0);

        AVPlayback* AVPlayback::_instance = nullptr;

        AVPlayback::AVPlayback()
        : AbstractPlugin()
        {
            AVPlayback::_instance = this;

            InitializeIARM();
            device::Manager::Initialize();
          
            registerMethod(AVPLAYBACK_METHOD_GET_VIDEO_ASPECT_RATIO, &AVPlayback::getVideoAspectRatio, this);
        }

        AVPlayback::~AVPlayback()
        {
        }

        void AVPlayback::Deinitialize(PluginHost::IShell* /* service */)
        {
            AVPlayback::_instance = nullptr;
            //No need to run device::Manager::DeInitialize for individual plugin. As it is a singleton instance
            //and shared among all wpeframework plugins
            DeinitializeIARM();
        }

        void AVPlayback::InitializeIARM()
        {
            Utils::IARM::init();
            if (Utils::IARM::init())
            {
                IARM_Result_t res;
                IARM_CHECK( IARM_Bus_RegisterEventHandler(IARM_BUS_DSMGR_NAME,IARM_BUS_DSMGR_EVENT_AVPLAYBACK_VIDEO_ASPECT_RATIO_CHANGE, 
                                                          dsvideoAspectRatioChangedEventHandler ));
            }

        }

        void AVPlayback::DeinitializeIARM()
        {
            if (Utils::IARM::isConnected())
            {
                IARM_Result_t res;
                IARM_CHECK( IARM_Bus_UnRegisterEventHandler(IARM_BUS_DSMGR_NAME,IARM_BUS_DSMGR_EVENT_AVPLAYBACK_VIDEO_ASPECT_RATIO_CHANGE) );
            }

        }

        // Registered methods begin
        uint32_t AVPlayback::getVideoAspectRatio(const JsonObject& parameters, JsonObject& response)
        {
            LOGINFOMETHOD();
            JsonObject dispModeObj;
            JsonArray dispOptions ;
            bool success = false;
            dispOptions.Add("UNKNOWN"); dispOptions.Add("4:3"); dispOptions.Add("16:9");
            dispModeObj["supportedVideoAspectRatio"] = dispOptions;
            dispModeObj["currentVideoAspectRatio"] = "UNKNOWN";
            
            uint8_t aspectRatio = device::AVPlayback::getInstance().getVideoAspectRatio();
            switch(aspectRatio)
            {
              case  dsAVPLAYBACK_VIDEO_ASPECT_RATIO_UNKNOWN:   
                dispModeObj["currentVideoAspectRatio"] = "UNKNOWN";
                break;
             
              case dsAVPLAYBACK_VIDEO_ASPECT_RATIO_3X4:
                dispModeObj["currentVideoAspectRatio"] = "4:3";
                break;
             
              case dsAVPLAYBACK_VIDEO_ASPECT_RATIO_16x9:
                dispModeObj["currentVideoAspectRatio"] = "16:9";
                break;

              default:
                dispModeObj["currentVideoAspectRatio"] = "UNKNOWN";
                break;
             }
             response["AspectRatio:"] = dispModeObj;

            success = true;
            returnResponse(success);
        }
 
        //End methods
                /**
         * @brief This function is used to translate HDMI In Composite input 
         * videoAspectRatioChanged event.
         *
         * @param[Aspect_ratio] Aspect ratio 
         */
        void AVPlayback::videoAspectRatioChanged(dsAVPlaybackAspectRatio_t AspectRatio)
        {
            LOGWARN("videoAspectRatioChanged [%d]", AspectRatio);

            JsonObject params;

            if(AspectRatio == dsAVPLAYBACK_VIDEO_ASPECT_RATIO_3X4) {
                params["aspectratio"] = "3:4";
            }
            else if(AspectRatio == dsAVPLAYBACK_VIDEO_ASPECT_RATIO_16x9){
                params["aspectratio"] = "16:9";
            }
            else {
               params["aspectratio"] = "UNKNOWN";
            }

            sendNotify(AVPLAYBACK_EVENT_ON_VIDEO_ASPECT_RATIO_CHANGED, params);
        }

        void AVPlayback::dsvideoAspectRatioChangedEventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
        {
           if(!AVPlayback::_instance)
                return;

            if (IARM_BUS_DSMGR_EVENT_AVPLAYBACK_VIDEO_ASPECT_RATIO_CHANGE == eventId)
            {
                IARM_Bus_DSMgr_EventData_t *eventData = (IARM_Bus_DSMgr_EventData_t *)data;
                dsAVPlaybackAspectRatio_t avPlayback_aspectRatio = eventData->data.AVPlaybackAspectRatio.aspectRatio;
                LOGWARN("Received IARM_BUS_DSMGR_EVENT_VIDEO_ASPECT_RATIO_CHANGE event  Aspect_ratio: %d ",avPlayback_aspectRatio);

                AVPlayback::_instance->videoAspectRatioChanged(avPlayback_aspectRatio);

            }

        }

    } // namespace Plugin
} // namespace WPEFramework

