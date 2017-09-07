#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include <XPLMMenus.h>
#include <XPLMUtilities.h>
#include <XPWidgets.h>
#include <XPStandardWidgets.h>

using namespace std;

// Global vars
XPLMDataRef latitude, longitude, elevation,	groundSpeed, gForcesX, gForcesY, gForcesZ;
XPWidgetID mainWindow, txtTest, btnOK;

char saveFileName[100] = "xCampaignSaveFile.txt";

// Functions/callback Declarations
float getFlightDataLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastLoop, int inCounter, void *inRefcon);
void createMainWindow();
int mainWindowCallback(XPWidgetMessage msg, XPWidgetID, intptr_t, intptr_t);
void closeWidgetWindow(XPWidgetID);
bool fileCheck();


#pragma region SDK Stuff
PLUGIN_API int XPluginStart(char * outName, char * outSig,	char * outDesc)  // Runs on sim startup, only runs once
{
	// Fill in the passed in buffers to describe our plugin to the plugin-system.
	strcpy(outName, "xplaneCampaign");
	strcpy(outSig, "xplaneCampaign");
	strcpy(outDesc, "A plugin that attempts to create a sort of campaign system to the sim.");

	#pragma region datarefs
	// the data refs we will be using
	latitude = XPLMFindDataRef("sim/flightmodel/position/latitude");
	longitude = XPLMFindDataRef("sim/flightmodel/position/longitude");
	elevation = XPLMFindDataRef("sim/flightmodel/position/elevation");
	groundSpeed = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	gForcesX = XPLMFindDataRef("sim/flightmodel/forces/g_axil"); // these two might be mixed up
	gForcesY = XPLMFindDataRef("sim/flightmodel/forces/g_side"); // ^^
	#pragma endregion datarefs

	createMainWindow();

	// our loop that is run so many times per second, that rate is decided by the return value of the callback
	// commented out while testing
	// XPLMRegisterFlightLoopCallback(getFlightDataLoopCallback, 1.0, NULL);


	return 1; // must return 1 to indicate successful initialization, otherwise we will not be called back again
	// the part about not getting called back again doesn't make sense to me, as it appears xpluginstart is only run once
}

PLUGIN_API void	XPluginStop(void) // Cleanup routine deallocates our window.
{
	closeWidgetWindow(mainWindow);
}

PLUGIN_API void XPluginDisable(void) // don't need to do anything here, but it is required
{
}

PLUGIN_API int XPluginEnable(void) // We don't do any enable-specific initialization, but we must return 1 to indicate that we may be enabled at this time.
{
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho, int inMessage, void * inParam)  // don't need to do anything here, but it is required
{
}
#pragma endregion SDK Stuff


#pragma region Callbacks
// this loop runs continuosly
float getFlightDataLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastLoop, int inCounter, void *inRefcon)
{
	// float elapsed = XPLMGetElapsedTime(); // time alapsed in sim
	float lat = XPLMGetDataf(latitude); // planes location latitude
	float lon = XPLMGetDataf(longitude); // planes location longitude
	float elev = XPLMGetDataf(elevation); // planes elevation
	float grndSpd = XPLMGetDataf(groundSpeed); // ground speed of plane
	float gfx = XPLMGetDataf(gForcesX); // g forces on the X axis
	float gfy = XPLMGetDataf(gForcesY); // g forces on the Y axis
	float gfZ = XPLMGetDataf(gForcesZ); // g forces on the Z axis

	createMainWindow();
	// XPLMDebugString("Log test!\n");  // prints string to log.txt, found in xplane base dir
	
	return 0.5; // this value is how often the loop runs, in seconds, a negative number should loop by frames instead
}

int mainWindowCallback(XPWidgetMessage msg, XPWidgetID widget, intptr_t param1, intptr_t param2) {
	if (msg == xpMessage_CloseButtonPushed) {
		closeWidgetWindow(mainWindow);
		return 1;
	}
	else if (msg == xpMsg_PushButtonPressed) {
		XPLMDebugString("Ok button pushed.");
	}

	return 0;
}
#pragma endregion Callbacks

#pragma region My Functions
void createMainWindow() {
	int x = 100, y = 700, w = 300, h = 195;
	int winRight = x + w,
		winBottom = y - h;

	// actual window
	mainWindow = XPCreateWidget(x, y, winRight, winBottom, 1, "xCampaign", 1, NULL, xpMainWindowStyle_Translucent);
	XPSetWidgetProperty(mainWindow, xpProperty_MainWindowHasCloseBoxes, 1); // adds button to close window
	XPAddWidgetCallback(mainWindow, mainWindowCallback);
	
	// text box
	XPCreateWidget(x + 20, y - 50, x + 100, y - 65, 1, "Test:", 0, mainWindow, xpWidgetClass_Caption);
	txtTest = XPCreateWidget(x + 100, y - 50, winRight - 20, y - 65, 1, "test", 0, mainWindow, xpWidgetClass_TextField);
	XPSetWidgetProperty(txtTest, xpProperty_MaxCharacters, 50); // set max characters if needed

	// ok button
	btnOK = XPCreateWidget(x + 70, y - 160, x + 140, y - 180, 1, "OK", 0, mainWindow, xpWidgetClass_Button);
}

void closeWidgetWindow(XPWidgetID windowName) {
	XPDestroyWidget(windowName, 1);
}

bool fileCheck() {
	ifstream checkedFile(saveFileName);
	if (checkedFile.good()) {
		XPLMDebugString("Save file found!\n");

		return true;
	}
	else {
		XPLMDebugString("Save file not found, or something else went wrong.\nCreating file now.\n");
		try
		{
			ofstream createdFile(saveFileName);
			XPLMDebugString("File created successfully.\n");

			return true;
		}
		catch (const std::exception&)
		{
			XPLMDebugString("Something went wrong, unable to create file.\n");

			return false;
		}
	}

	return false;
}
#pragma endregion My Functions
