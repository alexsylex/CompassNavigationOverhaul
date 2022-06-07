#include "utils/Logger.h"

#include "IUI/API.h"

#include "CompassShoutMeterHolder.h"

namespace HCN
{
	void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg)
	{
		if (!a_msg || std::string_view(a_msg->sender) != "Infinity UI") 
		{
			return;
		}

		if (auto message = IUI::API::TranslateAs<IUI::API::Message>(a_msg)) 
		{
			if (message->movieUrl.find("HUDMenu") == std::string::npos)
			{
				return;
			}

			switch (a_msg->type) 
			{
			case IUI::API::kPreLoad:
			{
				if (auto preLoadMessage = IUI::API::TranslateAs<IUI::API::PreLoadMessage>(a_msg))
				{
					std::string pathToOriginal = preLoadMessage->originalDisplayObject.ToString().c_str();

					if (pathToOriginal == "_level0.HUDMovieBaseInstance.CompassShoutMeterHolder")
					{
						CompassShoutMeterHolder::InitSingleton(preLoadMessage->originalDisplayObject);
					}
				}
				break;
			}
			case IUI::API::Status::kPostLoad:
			{
				if (auto postLoadMessage = IUI::API::TranslateAs<IUI::API::PostLoadMessage>(a_msg))
				{
					if (CompassShoutMeterHolder::GetSingleton()) 
					{
						std::string pathToOriginal = CompassShoutMeterHolder::GetSingleton()->GetPathToMember();
						std::string pathToNew = postLoadMessage->newDisplayObject.ToString().c_str();

						if (pathToOriginal == pathToNew) 
						{
							CompassShoutMeterHolder::UpdateSingleton(postLoadMessage->newDisplayObject);
						}
					}
				}
				break;
			}
			case IUI::API::Status::kAbortLoad:
			{
				if (auto abortLoadMessage = IUI::API::TranslateAs<IUI::API::AbortLoadMessage>(a_msg))
				{
					logger::warn("Aborted replacement of {}", abortLoadMessage->originalValue.ToString());
					logger::flush();
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	// If all plugins have been loaded
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) 
	{
		if (SKSE::GetMessagingInterface()->RegisterListener("Infinity UI", HCN::InfinityUIMessageListener)) 
		{
			logger::info("Successfully registered for Infinity UI messages!");
		}
		else 
		{
			logger::error("Infinity UI installation not detected. Please, go to ... to get it");
		}
	}
}