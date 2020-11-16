#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class %APP_NAME% : public App {
  public:
	void mouseDrag( MouseEvent event ) override;

	void keyDown( KeyEvent event ) override;

	void draw() override;

  private:
	std::vector<vec2> mPoints;
};

void prepareSettings( %APP_NAME%::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void %APP_NAME%::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

void %APP_NAME%::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == KeyEvent::KEY_SPACE ) {
		mPoints.clear();
	}
	else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
	}
}

void %APP_NAME%::draw()
{
	gl::clear( Color::gray( 0.1f ) );

	gl::color( 1.0f, 0.5f, 0.25f );

	gl::begin( GL_LINE_STRIP );
	for( const vec2 &point : mPoints ) {
		gl::vertex( point );
	}
	gl::end();
}

CINDER_APP( %APP_NAME%, RendererGl, prepareSettings )
