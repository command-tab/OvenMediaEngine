//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2020 AirenSoft. All rights reserved.
//
//==============================================================================
#include "record.h"
#include "common.h"


namespace api
{
	namespace conv
	{
		static void SetRecordStreamTracks(Json::Value &parent_object, const char *key, const std::map<int32_t, std::shared_ptr<MediaTrack>> &tracks, Optional optional)
		{
			CONVERTER_RETURN_IF(false, Json::arrayValue);

			for (auto &item : tracks)
			{
				auto &track = item.second;

				object.append(track->GetId());
			}
		}		

		static void SetRecordStream(Json::Value &parent_object, const char *key, const info::Stream &stream, Optional optional)
		{
			CONVERTER_RETURN_IF(false, Json::objectValue);

			SetString(object, "name", stream.GetName(), Optional::False);
			SetRecordStreamTracks(object, "tracks", stream.GetTracks(), Optional::False);
		}
				
		// Example of Record Start
		// ----------------------
		// 	{
		// 		"id": "<CustomId | AutoGeneratedId>",
		// 		"stream" : {
		// 			"name" : "<OutputStreamName>",
		// 			"tracks" : [ 101, 102 ]
		// 		}
		// 	}

		// Example of Record Stop
		// ----------------------
		// 	{
		// 		"id": "<CustomId | AutoGeneratedId>"
		// 	}
		std::shared_ptr<info::Record> RecordFromJson(const Json::Value &json_body)
		{
			auto record = std::make_shared<info::Record>();

			// <Optional>
			auto json_id = json_body["id"];
			if(json_id.empty() == false && json_id.isString() == true)
			{
				record->SetId(json_id.asString().c_str());			
			}

			// <Optional>
			auto json_stream = json_body["stream"];
			if(json_stream.empty() == false || json_stream.isObject() == true)
			{
				info::Stream stream(StreamSourceType::Ovt);

				// <Required>
				auto json_stream_name = json_stream["name"];
				if(json_stream_name.empty())
				{
					return nullptr;
				}
				
				stream.SetName(json_stream_name.asString().c_str());

				// <Optional>
				auto json_stream_tracks = json_stream["tracks"];
				if(json_stream_tracks.empty() == false && json_stream_tracks.isArray() == true)
				{
					for(uint32_t i=0 ; i<json_stream_tracks.size() ; i++)
					{
						if(json_stream_tracks[i].isInt())
						{
							auto media_track = std::make_shared<MediaTrack>();
							media_track->SetId(json_stream_tracks[i].asInt());
							
							stream.AddTrack(media_track);
						}
					}
				}	

				record->SetStream(stream);
			}

			return std::move(record);
		}

		Json::Value JsonFromRecord(const std::shared_ptr<info::Record> &record)
		{
			Json::Value response(Json::ValueType::objectValue);
		
			SetString(response, "id", record->GetId(), Optional::False);
			SetString(response, "vhost", record->GetVhost(), Optional::False);
			SetString(response, "app", record->GetApplication(), Optional::False);
			SetRecordStream(response, "stream", record->GetStream(), Optional::False);
			SetString(response, "state", record->GetStateString(), Optional::False);
			SetString(response, "filePath", record->GetFilePath(), Optional::False);
			SetString(response, "fileTmpPath", record->GetTmpPath(), Optional::False);
			SetString(response, "fileInfoPath", record->GetFileInfoPath(), Optional::False);
			SetInt64(response, "recordBytes", record->GetRecordBytes());
			SetInt64(response, "recordTime", record->GetRecordTime());
			SetInt64(response, "totalRecordBytes", record->GetRecordTotalBytes());
			SetInt64(response, "totalRecordTime", record->GetRecordTotalTime());			
			SetInt(response, "sequence", record->GetSequence());
			SetTimestamp(response, "startTime", record->GetRecordStartTime());
			SetTimestamp(response, "finishTime", record->GetRecordStopTime());
			SetTimestamp(response, "createdTime", record->GetCreatedTime());

			return std::move(response);
		}	

	}  // namespace conv
}  // namespace api