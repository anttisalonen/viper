#include <Ogre.h>
#include <OgreSingleton.h>
 
/* http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Simple+Text+Output */
class TextRenderer : public Ogre::Singleton<TextRenderer>
{
	public:
		TextRenderer();
		~TextRenderer();

		void addTextBox(const std::string& ID,
				const std::string& text,
				Ogre::Real x, Ogre::Real y,
				Ogre::Real width, Ogre::Real height,
				const Ogre::ColourValue& color = Ogre::ColourValue(1.0, 1.0, 1.0, 1.0));

		void removeTextBox(const std::string& ID);

		void setText(const std::string& ID, const std::string& Text);
		void printf(const std::string& ID,  const char *fmt, /* args*/ ...);

	private:
		Ogre::OverlayManager*    _overlayMgr;
		Ogre::Overlay*           _overlay;
		Ogre::OverlayContainer*  _panel;

};

