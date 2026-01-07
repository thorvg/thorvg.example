#include "Example.h"
#ifdef TVGEXAMPLE_GLES_SUPPORTED
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#include <windows.h>
static HDC gHdc = nullptr;
#endif

static SDL_GLContext getCurrentContext() {
#if defined(TVGEXAMPLE_GLES_SUPPORTED)
  auto fn = (SDL_GLContext(*)())SDL_GL_GetProcAddress("eglGetCurrentContext");
  return fn ? fn() : SDL_GL_GetCurrentContext();
#elif defined(_WIN32) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
  return (SDL_GLContext)wglGetCurrentContext();
#else
  return SDL_GL_GetCurrentContext();
#endif
}

static bool makeCurrent(SDL_Window* win, SDL_GLContext ctx) {
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__) && !defined(TVGEXAMPLE_GLES_SUPPORTED)
  if (!gHdc) {
    SDL_SysWMinfo info; SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(win, &info)) gHdc = GetDC(info.info.win.window);
  }
  return wglMakeCurrent(gHdc, (HGLRC)ctx);
#else
  return SDL_GL_MakeCurrent(win, ctx) == 0;
#endif
}

int main(int argc, char** argv) {
  (void)argc; (void)argv;
  if (!tvgexam::verify(tvg::Initializer::init(0))) return 0;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) { tvg::Initializer::term(); return 0; }

  uint32_t w = 800, h = 800;
#ifdef TVGEXAMPLE_GLES_SUPPORTED
  SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  auto window = SDL_CreateWindow("Context", SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  auto tvgCtx = window ? SDL_GL_CreateContext(window) : nullptr;
  auto otherCtx = tvgCtx ? SDL_GL_CreateContext(window) : nullptr;
  if (!otherCtx || !makeCurrent(window, tvgCtx)) {
    if (otherCtx) SDL_GL_DeleteContext(otherCtx);
    if (tvgCtx) SDL_GL_DeleteContext(tvgCtx);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit(); tvg::Initializer::term();
    return 0;
  }

#if defined(TVGEXAMPLE_GLES_SUPPORTED)
  auto targetCtx = getCurrentContext();
  void* display = eglGetCurrentDisplay();
  void* surface = eglGetCurrentSurface(EGL_DRAW);
#elif defined(_WIN32) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
  auto targetCtx = getCurrentContext();
  void* display = nullptr;
  void* surface = gHdc;
#else
  auto targetCtx = getCurrentContext();
  void* display = nullptr;
  void* surface = nullptr;
#endif
  auto canvas = tvg::GlCanvas::gen();
  if (!canvas || !tvgexam::verify(canvas->target(display, surface, targetCtx, 0, w, h, tvg::ColorSpace::ABGR8888S))) {
    delete canvas;
    SDL_GL_DeleteContext(otherCtx); SDL_GL_DeleteContext(tvgCtx);
    SDL_DestroyWindow(window); SDL_Quit(); tvg::Initializer::term();
    return 0;
  }

  auto ring = tvg::Shape::gen();
  ring->appendCircle(0, 0, 260, 260);
  ring->fill(0, 0, 0, 0);
  ring->strokeFill(255, 255, 255, 255);
  ring->strokeWidth(8);
  ring->translate(w * 0.5f, h * 0.5f);
  canvas->push(ring);

  auto needle = tvg::Shape::gen();
  needle->appendRect(-8, -200, 16, 140);
  needle->fill(255, 140, 0, 220);
  needle->translate(w * 0.5f, h * 0.5f);
  canvas->push(needle);

  canvas->draw(); canvas->sync();
  SDL_GL_SwapWindow(window);
  makeCurrent(window, otherCtx);

  SDL_Event ev;
  bool running = true;
  auto t0 = SDL_GetTicks();
  while (running) {
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) running = false;
      if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_ESCAPE) running = false;
    }
    makeCurrent(window, otherCtx);
    float t = (SDL_GetTicks() - t0) * 0.001f;
    glViewport(0, 0, w, h);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.15f + 0.1f*sinf(t*0.9f), 0.18f + 0.1f*sinf(t*1.1f+2), 0.22f + 0.1f*sinf(t*1.3f+4), 1);
    glClear(GL_COLOR_BUFFER_BIT);

    needle->rotate(t * 90);
    canvas->update(); canvas->draw(false); canvas->sync();

    if (getCurrentContext() != targetCtx) break;
    SDL_GL_SwapWindow(window);
  }

  delete canvas;
  SDL_GL_DeleteContext(otherCtx); SDL_GL_DeleteContext(tvgCtx);
  SDL_DestroyWindow(window); SDL_Quit(); tvg::Initializer::term();
  return 0;
}
