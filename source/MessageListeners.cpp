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
			if (message->contextMovieUrl.find("HUDMenu") == std::string::npos)
			{
				return;
			}

			switch (a_msg->type) 
			{
			case IUI::API::Message::Type::kStartLoad:
			{
				break;
			}
			case IUI::API::Message::Type::kPreReplace:
			{
				if (auto preReplaceMessage = IUI::API::TranslateAs<IUI::API::PreReplaceMessage>(a_msg))
				{
					std::string pathToOriginal = preReplaceMessage->originalDisplayObject.ToString().c_str();

					if (pathToOriginal == CompassShoutMeterHolder::path)
					{
						if (CompassShoutMeterHolder::InitSingleton(preReplaceMessage->originalDisplayObject))
						{
							logger::debug("CompassShoutMeterHolder instance initialised for {}", CompassShoutMeterHolder::path);
						}
					}
				}
				break;
			}
			case IUI::API::Message::Type::kPostPatch:
			{
				if (auto postPatchMessage = IUI::API::TranslateAs<IUI::API::PostPatchMessage>(a_msg))
				{
					std::string pathToNew = postPatchMessage->newDisplayObject.ToString().c_str();

					if (pathToNew == CompassShoutMeterHolder::path) 
					{
						// We initialised the CompassShoutMeterHolder singleton in the pre-replace step,
						// if not, there has been an error
						if (CompassShoutMeterHolder::GetSingleton()) 
						{
							CompassShoutMeterHolder::UpdateSingleton(postPatchMessage->newDisplayObject);
						}
						else 
						{
							logger::error("CompassShoutMeterHolder instance counterpart not ready for {}", CompassShoutMeterHolder::path);
							logger::flush();
						}
					}
				}
				break;
			}
			case IUI::API::Message::Type::kAbortPatch:
			{
				if (auto abortPatchMessage = IUI::API::TranslateAs<IUI::API::AbortPatchMessage>(a_msg))
				{
					std::string pathToOriginal = abortPatchMessage->originalValue.ToString().c_str();

					if (pathToOriginal == CompassShoutMeterHolder::path)
					{
						logger::error("Aborted replacement of {}", CompassShoutMeterHolder::path);
						logger::flush();
					}
				}
				break;
			}
			case IUI::API::Message::Type::kFinishLoad:
			{
				if (auto finishLoadMessage = IUI::API::TranslateAs<IUI::API::FinishLoadMessage>(a_msg)) 
				{
					Test::InitSingleton(GFxDisplayObject{ finishLoadMessage->contextMovieView, Test::path });
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