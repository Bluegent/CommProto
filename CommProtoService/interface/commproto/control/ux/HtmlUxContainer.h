#ifndef HTML_UX_CONTAINER_H
#define HTML_UX_CONTAINER_H

namespace commproto
{
	namespace control
	{
		namespace ux
		{

			class HtmlUxContainer : public UxContainer
			{
			public:
				HtmlUxContainer(const std::string & html_) : html{ html_ } {}
				std::string html;
			};
		}
	}
}


#endif