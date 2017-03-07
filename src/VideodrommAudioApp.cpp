#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Log
#include "VDLog.h"
// UI
#include "VDUIAudio.h"
// Websockets
#include "VDUIWebsockets.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

class VideodrommAudioApp : public App {

public:
	void setup() override;
	void mouseMove(MouseEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;
	void fileDrop(FileDropEvent event) override;
	void update() override;
	void draw() override;
	void cleanup() override;
private:
	// Settings
	VDSettingsRef				mVDSettings;
	// Session
	VDSessionRef				mVDSession;
	// Log
	VDLogRef					mVDLog;
	// UIAudio
	VDUIAudioRef				mUIAudio;

	// UIWebsockets
	VDUIWebsocketsRef			mUIWebsockets;
	// handle resizing for imgui
	void						resizeWindow();
	bool						mIsResizing;
	// imgui
	float						color[4];
	float						backcolor[4];
	int							playheadPositions[12];
	int							speeds[12];

	float						f = 0.0f;
	char						buf[64];
	unsigned int				i, j;

	bool						mouseGlobal;

	string						mError;
	// fbo
	bool						mIsShutDown;
};


void VideodrommAudioApp::setup()
{
	// Settings
	mVDSettings = VDSettings::create();
	// Session
	mVDSession = VDSession::create(mVDSettings);
	mVDSession->getWindowsResolution();
	// UIAudio
	mUIAudio = VDUIAudio::create(mVDSettings, mVDSession);
	// UIWebsockets
	mUIWebsockets = VDUIWebsockets::create(mVDSettings, mVDSession);

	// set ui window and io events callbacks 
	ui::connectWindow(getWindow());
	ui::initialize();

	// windows
	mIsShutDown = false;
	// fps limiting
	setFrameRate(10.0f);
}

void VideodrommAudioApp::update()
{
	mVDSession->update(1);
}
void VideodrommAudioApp::cleanup()
{
	if (!mIsShutDown)
	{
		mIsShutDown = true;
		CI_LOG_V("shutdown");
		ui::disconnectWindow(getWindow());
		ui::Shutdown();
		// save settings
		mVDSettings->save();
		mVDSession->save();
		quit();
	}
}
void VideodrommAudioApp::mouseMove(MouseEvent event)
{
	if (!mVDSession->handleMouseMove(event)) {
		// let your application perform its mouseMove handling here
	}
}
void VideodrommAudioApp::mouseDown(MouseEvent event)
{
	if (!mVDSession->handleMouseDown(event)) {
		// let your application perform its mouseDown handling here
	}
}
void VideodrommAudioApp::mouseDrag(MouseEvent event)
{
	if (!mVDSession->handleMouseDrag(event)) {
		// let your application perform its mouseDrag handling here
	}	
}
void VideodrommAudioApp::mouseUp(MouseEvent event)
{
	if (!mVDSession->handleMouseUp(event)) {
		// let your application perform its mouseUp handling here
	}
}

void VideodrommAudioApp::keyDown(KeyEvent event)
{
	if (!mVDSession->handleKeyDown(event)) {
		switch (event.getCode()) {
		case KeyEvent::KEY_ESCAPE:
			// quit the application
			quit();
			break;

		}
	}
}
void VideodrommAudioApp::keyUp(KeyEvent event)
{
	if (!mVDSession->handleKeyUp(event)) {
	}
}
void VideodrommAudioApp::resizeWindow()
{
	mVDSession->resize();
}
void VideodrommAudioApp::fileDrop(FileDropEvent event)
{
	mVDSession->fileDrop(event);
}
void VideodrommAudioApp::draw()
{
	gl::clear(Color::black());

	getWindow()->setTitle(toString((int)getAverageFps()) + " fps Videodromm");

	// imgui
	if (!mVDSettings->mCursorVisible) return;

	mVDSettings->uiMargin = 3;
	mUIAudio->Run("Audio");
	// Websockets
	mVDSettings->uiMargin += mVDSettings->uiLargeW + mVDSettings->uiMargin;
	mUIWebsockets->Run("Websockets");
}


CINDER_APP(VideodrommAudioApp, RendererGl)
