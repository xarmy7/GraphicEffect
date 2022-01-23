# Graphic Effects - Shadow Mapping & Post-Process

## <h2 id="top">Summary</h2>
- [Introduction](#intro)   
- [Currently implemented](#implements)   
- [Difficulty through the project](#difficulty)  
- [Gallery](#gallery)

## <h2 id="intro">Introduction</h2>
<p>The Graphic Effects project consisted of coding some graphics effects like post-process, gamma correction,... Here we going to concentrate on the shadow mapping and the post-process. The project can be launch in Debug x64.  

> In the engine we used, you will see differents types of scene. Our scene's name is Demo Perso (7th one). In the scene, we have implemented the post-process on a quad (behind the tavern scene) in addition to the shadow mapping.

> You can also change the rendering of the scene in the IMGUI window. You have the choice between different types of rendering like kernel and many others.
</p>

## <h2 id="implements">Currently Implemented</h2>
### Post-Process
- Gray scale
- Kernel effects

<p>

> In the IMGUI window, you will see a red image, this is the depth map texture</p>

### Shadow Mapping
- Depth Map
- Cancel Shadow Acne
- Over sampling
- PCF

<p>

> In the IMGUI window, you can change the position light to see shadows changements. You can see in real time with the shadow map where is the light.</p>

## <h2 id="difficulty">Difficulty through the project</h2>

### Camille's Difficulties:
- understand the technical terms of the subjects
- debug the project
### Maxence's Difficulties:
- Manage differents shaders 

## <h2 id="gallery">Gallery</h2>
 ![WithoutShadow](Resources/WithoutShadow.PNG)  
 ![AcneShadow](Resources/ShadowAcne.PNG)    
 ![WithShadow](Resources/WithShadow.PNG)    
 ![KernelScene](Resources/KernelScene.PNG)    
 ![NegativeScene](Resources/NegativeScene.PNG)    
 ![GrayScaleScene](Resources/GrayScaleScene.PNG)    

[Top of the page](#top)

