//
//  ViewController.m
//  LavaLamp
//
//  Created by Ian Voyce on 16/01/2013.
//  Copyright (c) 2013 Ian Voyce. All rights reserved.
//

#import "ViewController.h"

#include "Blobs.h"


@interface ViewController () {
    GLKMatrix4 _modelViewProjectionMatrix;
    GLKMatrix4 _projectionMatrix;
//    float _rotation;
    
//    GLuint framebuffer;
//    GLuint texture;
//    GLuint colorRenderbuffer;
//    GLuint depthRenderbuffer;
    
    Blobs *_blobs;
}
@property (strong, nonatomic) EAGLContext *context;
//@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormatNone;
  
    self.view.contentScaleFactor = 1.0;
    
    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

-(void)initBlobs {
    NSString *vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    const char *vsource = [[NSString stringWithContentsOfFile:vertShaderPathname
                                                     encoding:NSUTF8StringEncoding
                                                        error:nil] UTF8String];
    NSString *fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    const char *fsource = [[NSString stringWithContentsOfFile:fragShaderPathname
                                                     encoding:NSUTF8StringEncoding
                                                        error:nil] UTF8String];
    
    float width = self.view.bounds.size.width;
    float height = self.view.bounds.size.height;
    
    _blobs = new Blobs(vsource, fsource, width, height);
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    [self initBlobs];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    float w = self.view.bounds.size.width;
    float h = self.view.bounds.size.height;
    _projectionMatrix = GLKMatrix4MakeOrtho(-(w/2), w/2,
                                            -(h/2), h/2,
                                            0.0, 1.0);
    
    GLKMatrix4 baseModelViewMatrix = GLKMatrix4Identity;
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    _modelViewProjectionMatrix = GLKMatrix4Multiply(_projectionMatrix, modelViewMatrix);
    
    _blobs->Tick();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    //glClearColor(1.0f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    _blobs->Draw();
}

@end
