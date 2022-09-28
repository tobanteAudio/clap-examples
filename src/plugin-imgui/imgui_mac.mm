// imgui helpers for mac.
// plugin implementations should not need to touch this file.

#ifdef __APPLE__

#include <Cocoa/Cocoa.h>
#include <sys/time.h>

#include "main.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

bool imguiAttach(Plugin *plugin, void *native_display, void *native_window);

bool Plugin::isApiSupported(const char *api, bool is_floating)
{
  return api && !strcmp(api, CLAP_WINDOW_API_COCOA) && !is_floating;
}

bool Plugin::setParent(const clap_window *parentWindow)
{
  return parentWindow && parentWindow->cocoa &&
    imguiAttach(this, NULL, parentWindow->cocoa);
}

void getNativeWindowPosition(void *native_display, void *native_window,
  int&x, int&y, int&w, int&h)
{
  NSView *vw = (NSView*)native_window;
  NSRect vr = [vw convertRect:[vw bounds] toView:nil];
  NSRect wr = [[vw window] convertRectToScreen:vr];
  wr.origin.y = CGDisplayBounds(CGMainDisplayID()).size.height-(wr.origin.y+wr.size.height);
  x = wr.origin.x;
  y = wr.origin.y;
  w = wr.size.width;
  h = wr.size.height;
}

void setNativeParent(void *native_display, void *native_window, GLFWwindow *glfw_win)
{
  NSWindow *par = [(NSView*)native_window window];
  NSWindow *win = (NSWindow*)glfwGetCocoaWindow(glfw_win);
  [par addChildWindow:win ordered:NSWindowAbove];
}

@interface GuiTimer : NSObject
{
@public
  NSTimer *timer;
}
-(void)on_timer:(id)sender;
@end

@implementation GuiTimer
-(void)on_timer:(id)sender
{
  extern void imguiTimerCallback();
  imguiTimerCallback();
}
@end

GuiTimer *timer;

bool createTimer(unsigned int ms)
{
  timer = [GuiTimer new];
  timer->timer = [NSTimer scheduledTimerWithTimeInterval:(double)ms*0.001
    target:timer selector:@selector(on_timer:) userInfo:nil repeats:YES];
  return true;
}

void destroyTimer()
{
  [timer->timer invalidate];
  [timer release];
  timer = NULL;
}

unsigned int getTickCount()
{
  struct timeval tm = {0};
  gettimeofday(&tm, NULL);
  return (unsigned int)(tm.tv_sec*1000 + tm.tv_usec/1000);
}

#endif