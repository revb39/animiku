/************************************************************************
*	AM_Screen.h -- Main interface to AMENGINE							*
*				   Copyright (c) 2014 Justin Byers						*
* ===================================================================== *
* This file is part of AniMiku.											*
*																		*
* AniMiku is free software : you can redistribute it and / or modify	*
* it under the terms of the GNU General Public License as published by	*
* the Free Software Foundation, either version 3 of the License, or		*
* (at your option) any later version.									*
*																		*
* AniMiku is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of		*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the			*
* GNU General Public License for more details.							*
*																		*
* You should have received a copy of the GNU General Public License		*
* along with AniMiku. If not, see <http://www.gnu.org/licenses/>.		*
*************************************************************************/

#ifndef _SCREEN_
#define _SCREEN_

#include "AMTypes.h"
#include "AM_PMD.h"
#include "AM_PMX.h"
#include "AM_Cache.h"
#include "AM_Light.h"
#include "AM_PhysicsDebug.h"
#include "AM_Manager.h"
#include "AM_Settings.h"
#include "AM_Camera.h"

#include <mmsystem.h>

//The screen singleton
#define TheScreen ScreenBase::theScreen
/*Max lines in console window*/
static const WORD MAX_CONSOLE_LINES = 500;

/*Model structure*/
typedef struct _Model {
	AMCache<AM_Model> *model;
	char id[MAX_PATH];
	char* file;
	bool visible;
	bool physVisible;
	bool fadingOut;
	int fCount;
} Model;
/*Map model name to data structure*/
typedef map<string, Model*> ModelList;
/*Map motion name to motion cache*/
typedef map<string, AMCache<AM_Motion>*> MotionList;
struct Display {
	int displayNumber;
	int resolution[2];
	int basePos[2];
};
/*main callback from windows*/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wPAram, LPARAM lParam);

class Screen {
private:
	/*Windows stuff*/
	HWND rndrWindow;
	HINSTANCE hInstance;
	MSG msg;

	/*All the fun stuff for D3D10 rendering*/
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	IDXGISwapChain *swapChain;
	ID3D10Device *d3ddev;
	ID3D10Texture2D *backBuffer;
	ID3D10RenderTargetView *renderTargetView;

	D3D10_INPUT_ELEMENT_DESC layout[5];
	UINT numElements;

	D3D10_VIEWPORT vp;
	D3D10_RASTERIZER_DESC rasterDesc;
	ID3D10RasterizerState *rasterState;

	ID3D10DepthStencilView* DepthStencilView;
	ID3D10Texture2D* DepthStencilBuffer;

	D3D10_TEXTURE2D_DESC depthStencilDesc;
	ID3D10Effect *effect;
	ID3D10EffectTechnique *effectTech;
	ID3D10InputLayout *vertexLayout;

	/*Stuff for shadow map rendering*/
	ID3D10Texture2D *shadowMap;
	ID3D10DepthStencilView *shadowMapDepthView;
	ID3D10ShaderResourceView *shadowMapSRView;
	ID3D10EffectTechnique *shadowMapTech;
	ID3D10EffectShaderResourceVariable *shadow;

	/*Stuff for SSAO differred shading*/
	ID3D10Texture2D *posMap,*normalMap,*depthMap,*defferredDepthStencil, *ssaoMap, *blur1Map;
	ID3D10ShaderResourceView *posMapSRView,*normalMapSRView,*depthMapSRView, *ssaoMapSRView, *blur1MapSRView;
	ID3D10RenderTargetView *posMapRenderTargetView,*normalMapRenderTargetView,*depthMapRenderTargetView, *ssaoMapRenderTargetView, *blur1MapRenderTargetView;
	ID3D10DepthStencilView* defferredDepthStencilView;
	ID3D10ShaderResourceView *randomTexture;
	ID3D10EffectTechnique *ssaoMapTech;
	ID3D10EffectTechnique *ssaoOcclusionTech;
	ID3D10EffectTechnique *blur1Tech;

	/*Pass descriptions*/
	D3D10_PASS_DESC passDesc;
	D3D10_PASS_DESC passDescShadow;
	D3D10_PASS_DESC passDescSSAO;
	D3D10_PASS_DESC passDescSSAOOcclusion;
	D3D10_PASS_DESC passDescBlur1;

	/*Shader variables*/
	ID3D10EffectMatrixVariable *view;
	ID3D10EffectMatrixVariable *projection;
	ID3D10EffectMatrixVariable *lightViewProj;
	ID3D10EffectVectorVariable *cameraPos;
	ID3D10EffectVectorVariable *lightDirec;
	ID3D10EffectScalarVariable *renderingSkel;
	ID3D10EffectScalarVariable *extraBright;
	ID3D10EffectVectorVariable *texelSize;
	ID3D10EffectMatrixVariable *rigidBodyTrans;
	ID3D10EffectScalarVariable *renderingRBody;

	/*The settings object*/
	AMSettings settings;
	/*The light object*/
	AMLight* light;
	/*The engine manager object*/
	AM_Manager* _mgr;
	/*The Bullet debug object*/
	AMPhysicsDebug *_pDebug;
	/*The Bullet Physics object*/
	BulletPhysics physics;

	/*The list of motion data*/
	MotionList _motList;
	/*The list of models*/
	ModelList _mdlList;

	/*D3D initialization failed*/
	bool _badInit;

	/*
		void initD3D()

		Initialize Direct3D
	*/
	void initD3D();
	/*
		void createSCandDevice()

		Initialize device and swap chain

		Returns:		bool			Initialization sucessfull
	*/
	bool createSCandDevice();
	/*
		void createRT()

		Create render target view. Called by initD3D
	*/
	void createRT();
	/*
		void createVP()

		Create viewport. Called by initD3D
	*/
	void createVP();
	/*
		void createDS()

		Create depth stencil. Called by initD3D
	*/
	void createDS();
	/*
		void createEffect()

		Create effect (shader). Called by initD3D
	*/
	void createEffect();
	/*
		void parseEffect()

		Get pointers to all variables in the shader. Called by initD3D
	*/
	void parseEffect();
	/*
		void createVL()

		Create vertex layout. Called by initD3D
	*/
	void createVL();
	/*
		void createSM()

		Create the shadow map. Called by initD3D
	*/
	void createSM();
	/*
		void createSSAO()

		Create SSAO buffers. Called by initD3D
	*/
	void createSSAO();
	/*
		void loadNoiseTexture()

		Load the noise texture for SSAO. Called by createSSAO
	*/
	void loadNoiseTexture();
	/*
		void processWindowMsg()

		Process window messages
	*/
	void processWindowMsg();
	/*
		void allocConsole()

		Allocate and display a console window
	*/
	void allocConsole();

public:
	Screen();
	~Screen();

	/*The camera object*/
	Camera *cam;
	/*Dimensions in pixels of the screen*/
	int width, height;

	/*
		void init()

		Initialize AMENGINE

		Parameters:			w,h			Width&height of screen. Set to 0 to get from Settings
							hwnd		Window handle to attach D3D to
	*/
	void init(int w, int h, HWND hwnd);
	/*
		void render()

		Render the scene
	*/
	void render();
	/*
		void registerManagerCallback()

		Register an external class as a manager to receive callbacks from AMENGINE

		Parameters:			mgr			Pointer to the external object
	*/
	void registerManagerCallback(AM_Manager* mgr);
	/*
		AMSettings* getSettings()
		
		Get a pointer to the settings object

		Returns:			Pointer to the settings object
	*/
	AMSettings* getSettings();

	
	/*
		void preloadModel()

		Load and parse a model file

		Parameters:			id			ID of the model being loaded
							file		Path to file to load
	*/
	void preloadModel(char* id, char* file);
	/*
		void resetModelPhysics()

		Force all rigid bodies to move to their bones' positions

		Parameters:			id			ID of the model
	*/
	void resetModelPhysics(char* id);
	/*
		void setModelVisible()

		Set visibility of a model

		Parameters:			id			ID of the model
							v			true=visible,false=invisible
	*/
	void setModelVisible(char* id, bool v);
	/*
		bool getModelVisible()

		Get the visibility of a model

		Parameters:			id			ID of the model
		Returns:			bool		Model is visible
	*/
	bool getModelVisible(char* id);
	/*
		void setPhysVisible()

		Set the visibility of the physics debug information

		Parameters:			id			ID of the model
							v			true=visible,false=invisible
	*/
	void setPhysVisible(char* id, bool v);
	/*
		void clearModel()

		Clear and remove a model from the engine

		Parameters:			id			ID of the model
	*/
	void clearModel(char* id);
	/*
		void modelExists()

		Get if a model with a given ID exists

		Parameters:			id			ID to check
		Returns:			bool		The model exists
	*/
	bool modelExists(char* id);
	/*
		void clearModels()

		Clear all models loaded in engine
	*/
	void clearModels();
	/*
		void rotateModel()

		Set the rotation of a model

		Parameters:			id			ID of the model
							x,y,z		XYZ rotation (in degrees) of the model
	*/
	void rotateModel(char* id, float x, float y, float z);
	/*
		void moveModel()

		Set the translation of the model

		Parameters:			id			ID of the model
							x,y,z		XYZ world position of the model
	*/
	void moveModel(char* id, float x, float y, float z);

	/*
		void preloadMotion()

		Load and parse a motion data file

		Parameters:			id			ID of the motion data
							file		Path of file to load
	*/
	void preloadMotion(char* id, char* file);
	/*
		void playMotion()

		Start playback of a motion. Requires motion to be assigned to
		model first.

		Parameters:			mdlID		ID of the model to start playback
	*/
	void playMotion(char* mdlID);
	/*
		void stopMotion()

		Stop playback of a motion. Requires motion to be assigned to
		model first.

		Parameters:			mdlID		ID of the model to stop playback
	*/
	void stopMotion(char* mdlID);
	/*
		void assignMotionToModel()

		Assign a motion to a model. Will reset model to motion start position if
		the model is invisible, the motion is already assigned to this model,
		or force is set true

		Parameters:			mdlID		ID of the model to assign to
							motID		ID of the motion to assign
							force		Force the model to motion start position
	*/
	void assignMotionToModel(char* mdlID, char* motID, bool force);
	/*
		bool motionExists()

		Get if a motion with a given ID exists

		Parameters:			motID		ID to check
		Returns:			bool		The motion exists
	*/
	bool motionExists(char* motID);
	/*
		void clearMotion()

		Clear and remove a motion from the engine

		Parameters:			id			ID of the motion
	*/
	void clearMotion(char* id);
	/*
		void clearMotions()

		Clear all motions loaded
	*/
	void clearMotions();

	/*
		bool motionToAmd()

		Convert a motion file to AMD

		Parameters:			file		Path of file to convert
		Returns:			bool		Conversion sucessfull
	*/
	bool motionToAmd(char* file);
};

/*
	Base screen class
*/
class ScreenBase {
public:
	ScreenBase();
	static Screen theScreen;
};
#endif