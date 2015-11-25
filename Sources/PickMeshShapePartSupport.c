// PickMeshShapePartSupport.c - QuickDraw 3d routines//// written by Nick Thompson.// modifed by Robert Dierkes for an example of mesh shape part picking.// // �1994-95 Apple computer Inc., All Rights Reserved//#include <QuickDraw.h>#include "PickMeshShapePartSupport.h"#include "QD3DDrawContext.h"#include "QD3DRenderer.h"#include "QD3DShader.h"#include "QD3DCamera.h"#include "QD3DLight.h"#include "QD3DGeometry.h"#include "QD3DGroup.h"#include "QD3DSet.h"#include "QD3DView.h"#include "QD3DMath.h"#include "QD3DAcceleration.h"#define uMath_Sin_Deg(x)		((float)sin((double)Q3Math_DegreesToRadians((x))))#define uMath_Cos_Deg(x)		((float)cos((double)Q3Math_DegreesToRadians((x))))TQ3ViewObject MyNewView(WindowPtr theWindow){	TQ3Status				myStatus;	TQ3ViewObject			myView;	TQ3DrawContextObject	myDrawContext;	TQ3RendererObject		myRenderer;	TQ3CameraObject			myCamera;	TQ3GroupObject			myLights;		myView = Q3View_New();		//	Create and set draw context.	if ((myDrawContext = MyNewDrawContext(theWindow)) == nil)		goto ExitNewView;			if ((myStatus = Q3View_SetDrawContext(myView, myDrawContext)) == kQ3Failure)		goto ExitNewView;	Q3Object_Dispose(myDrawContext);		//	Create and set renderer.		// this would use the wireframe renderer#if 0	myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeWireFrame);	if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure) {		goto ExitNewView;	}	#else	// this would use the interactive software renderer	if ((myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeInteractive)) != nil) {		if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure) {			goto ExitNewView;		}		// these two lines set us up to use the best possible renderer,		// including  hardware if it is installed.		Q3InteractiveRenderer_SetDoubleBufferBypass(myRenderer, kQ3True);								Q3InteractiveRenderer_SetPreferences(myRenderer, kQAVendor_BestChoice, 0);	}	else {		goto ExitNewView;	}#endif	Q3Object_Dispose(myRenderer);		//	Create and set camera.	if ((myCamera = MyNewCamera(theWindow)) == nil)		goto ExitNewView;			if ((myStatus = Q3View_SetCamera(myView, myCamera)) == kQ3Failure)		goto ExitNewView;	Q3Object_Dispose(myCamera);		//	Create and set lights.	if ((myLights = MyNewLights()) == nil)		goto ExitNewView;			if ((myStatus = Q3View_SetLightGroup(myView, myLights)) == kQ3Failure)		goto ExitNewView;			Q3Object_Dispose(myLights);	//	Done!!!	return(myView);	ExitNewView:	//	If any of the above failed, then don't return a view.	return(nil);}//----------------------------------------------------------------------------------TQ3DrawContextObject MyNewDrawContext(WindowPtr theWindow){	TQ3DrawContextData		myDrawContextData;	TQ3MacDrawContextData	myMacDrawContextData;	TQ3ColorARGB			ClearColor;	TQ3DrawContextObject	myDrawContext ;		//	Set the background color.	ClearColor.a = 1.0;	ClearColor.r = 1.0;	ClearColor.g = 1.0;	ClearColor.b = 1.0;		//	Fill in draw context data.	myDrawContextData.clearImageMethod = kQ3ClearMethodWithColor;	myDrawContextData.clearImageColor = ClearColor;	myDrawContextData.paneState = kQ3False;	myDrawContextData.maskState = kQ3False;	myDrawContextData.doubleBufferState = kQ3True; 	myMacDrawContextData.drawContextData = myDrawContextData;		myMacDrawContextData.window =(CGrafPtr) theWindow;		// this is the window associated with the view	myMacDrawContextData.library = kQ3Mac2DLibraryNone;	myMacDrawContextData.viewPort = nil;	myMacDrawContextData.grafPort = nil;		//	Create draw context and return it, if it�s nil the caller must handle	myDrawContext = Q3MacDrawContext_New(&myMacDrawContextData);	return myDrawContext ;}//----------------------------------------------------------------------------------TQ3CameraObject MyNewCamera(WindowPtr theWindow){	TQ3ViewAngleAspectCameraData	perspectiveData;	TQ3CameraObject				camera;		TQ3Point3D 					from 	= { 0.0, 0.0, 3.0 };	TQ3Point3D 					to 		= { 0.0, 0.0, 0.0 };	TQ3Vector3D 				up 		= { 0.0, 1.0, 0.0 };	float 						fieldOfView = 1.0;	float 						hither 		= 0.001;	float 						yon 		= 100;		TQ3Status					returnVal = kQ3Failure ;	perspectiveData.cameraData.placement.cameraLocation 	= from;	perspectiveData.cameraData.placement.pointOfInterest 	= to;	perspectiveData.cameraData.placement.upVector 			= up;	perspectiveData.cameraData.range.hither	= hither;	perspectiveData.cameraData.range.yon 	= yon;	perspectiveData.cameraData.viewPort.origin.x = -1.0;	perspectiveData.cameraData.viewPort.origin.y = 1.0;	perspectiveData.cameraData.viewPort.width = 2.0;	perspectiveData.cameraData.viewPort.height = 2.0;		perspectiveData.fov				= fieldOfView;	perspectiveData.aspectRatioXToY	=		(float)(theWindow->portRect.right - theWindow->portRect.left) / 		(float)(theWindow->portRect.bottom - theWindow->portRect.top);			camera = Q3ViewAngleAspectCamera_New(&perspectiveData);	return camera ;}//----------------------------------------------------------------------------------TQ3GroupObject MyNewLights(){	TQ3GroupPosition		myGroupPosition;	TQ3GroupObject			myLightList;	TQ3LightData			myLightData;	TQ3DirectionalLightData	myDirectionalLightData;	TQ3LightObject			myAmbientLight, myFillLight;	TQ3Vector3D				fillDirection = { 0.0, 0.0, 50.0 };	TQ3ColorRGB				whiteLight = { 1.0, 1.0, 1.0 };		//	Set up light data for ambient light.  This light data will be used for point and fill	//	light also.	myLightData.isOn = kQ3True;	myLightData.color = whiteLight;		//	Create ambient light.	myLightData.brightness = 1.0;	myAmbientLight = Q3AmbientLight_New(&myLightData);	if (myAmbientLight == nil)		goto ExitMyNewLights;		//	Create fill light.	myLightData.brightness = 0.2;	myDirectionalLightData.lightData = myLightData;	myDirectionalLightData.castsShadows = kQ3False;	myDirectionalLightData.direction = fillDirection;	myFillLight = Q3DirectionalLight_New(&myDirectionalLightData);	if (myFillLight == nil)		goto ExitMyNewLights;	//	Create light group and add each of the lights into the group.	myLightList = Q3LightGroup_New();	if (myLightList == nil)		goto ExitMyNewLights;	myGroupPosition = Q3Group_AddObject(myLightList, myAmbientLight);	if (myGroupPosition == 0)		goto ExitMyNewLights;	myGroupPosition = Q3Group_AddObject(myLightList, myFillLight);	if (myGroupPosition == 0)		goto ExitMyNewLights;	Q3Object_Dispose(myAmbientLight);	Q3Object_Dispose(myFillLight);	//	Done!	return(myLightList);	ExitMyNewLights:	//	If any of the above failed, then return nothing!	return(nil);}TQ3GroupObject MyNewModel(){	#define	kNumVertices	5	#define	kAngleMax		360.0	#define	kAngleIncr		(kAngleMax / kNumVertices)	TQ3GroupObject		group;	TQ3GeometryObject	geometry;	unsigned long		i;	float				angle,						radius;	TQ3Vertex3D			vertices[kNumVertices];	TQ3MeshVertex		meshVertices[kNumVertices];	TQ3MeshFace			meshFace;	TQ3ShaderObject		illuminationShader;	group    = NULL;	geometry = NULL;	/* Make a regular group */	if ((group = Q3DisplayGroup_New()) == NULL)		return NULL;	/* Add a Phong illumination shader */	if ((illuminationShader = Q3PhongIllumination_New()) == NULL)		return NULL;	Q3Group_AddObject(group, illuminationShader);	Q3Object_Dispose(illuminationShader);	/* Create a mesh */	geometry = Q3Mesh_New();	/* Create a mesh face */	radius = 1.0;	for (i = 0, angle = kAngleIncr / 4.0;		i < kNumVertices;		i++, angle += kAngleIncr) {		vertices[i].point.x = uMath_Cos_Deg(angle) * radius;		vertices[i].point.y = uMath_Sin_Deg(angle) * radius;		vertices[i].point.z = 0.0;		vertices[i].attributeSet = NULL;		meshVertices[i] = Q3Mesh_VertexNew(geometry, &vertices[i]);			}	meshFace = Q3Mesh_FaceNew(geometry, kNumVertices, meshVertices, NULL);	/* Return the mesh in a group */	Q3Group_AddObject(group, geometry);	Q3Object_Dispose(geometry);	return group;}