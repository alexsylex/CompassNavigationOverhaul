#include "utils/Logger.h"

#include "IUI/API.h"

#include "Compass.h"

void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg);

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg)
{
	// If all plugins have been loaded
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) 
	{
		if (SKSE::GetMessagingInterface()->RegisterListener("Infinity UI", InfinityUIMessageListener)) 
		{
			logger::info("Successfully registered for Infinity UI messages!");
		}
		else 
		{
			logger::error("Infinity UI installation not detected. Please, go to ... to get it");
		}
	}
}

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

				if (pathToOriginal == extended::Compass::path)
				{
					if (extended::Compass::InitSingleton(preReplaceMessage->originalDisplayObject))
					{
						logger::debug("Compass instance initialised for {}", extended::Compass::path);
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

				if (pathToNew == extended::Compass::path)
				{
					// We initialised the CompassShoutMeterHolder singleton in the pre-replace step,
					// if not, there has been an error
					if (extended::Compass::GetSingleton())
					{
						extended::Compass::UpdateSingleton(postPatchMessage->newDisplayObject);
					}
					else
					{
						logger::error("Compass instance counterpart not ready for {}", extended::Compass::path);
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

				if (pathToOriginal == extended::Compass::path)
				{
					logger::error("Aborted replacement of {}", extended::Compass::path);
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
