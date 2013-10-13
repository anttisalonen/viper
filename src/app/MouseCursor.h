#ifndef APP_MOUSECURSOR_H
#define APP_MOUSECURSOR_H

#include <Ogre.h>

/* http://www.ogre3d.org/tikiwiki/tiki-index.php?page=How+to+show+the+mouse+cursor+without+CEGUI */
class MouseCursor {
	public:
		MouseCursor();
		~MouseCursor(void) {}

		void setImage(const Ogre::String& filename);
		void setWindowDimensions(unsigned int width, unsigned int height);
		void setVisible(bool visible);
		void updatePosition(int x, int y);

	private:
		Ogre::Overlay* mGuiOverlay;
		Ogre::OverlayContainer* mCursorContainer;
		Ogre::TexturePtr mTexture;
		Ogre::MaterialPtr mMaterial;

		Ogre::Real mWindowWidth;
		Ogre::Real mWindowHeight;
};

#endif
