#ifndef ENDPOINT_UIFACTORY_H
#define ENDPOINT_UIFACTORY_H
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/endpoint/Button.h>
#include <commproto/sockets/Socket.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/control/endpoint/Toggle.h>
#include <commproto/control/endpoint/Label.h>
#include <commproto/control/endpoint/Slider.h>

namespace commproto
{
	namespace control
	{
		namespace endpoint 
		{
			class UIFactory
			{
			public:
				UIFactory(const std::string & name, const messages::TypeMapperHandle& mapper , const sockets::SocketHandle& socket);
				ButtonHandle makeButton(const std::string& name, const ButtonAction& action) const;
				SliderHandle makeSlider(const std::string& name, const SliderAction& action, const std::string & unitOfMeasure = std::string{}) const;
				ToggleHandle makeToggle(const std::string& name, const ToggleAction & action, const bool defaultState = false) const;
				LabelHandle makeLabel(const std::string& name, const std::string & text) const;
				NotificationHandle makeNotification(const std::string& name) const;
				UIControllerHandle makeController() const;
			private:
				UIControllerHandle controller;
			};
		}

	}
}


#endif // ENDPOINT_UIFACTORY_H