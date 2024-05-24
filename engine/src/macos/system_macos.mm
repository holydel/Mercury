

#include "mercury_api.h"

#ifdef MERCURY_PLATFORM_MACOS
#include "mercury_log.h"

#include <dlfcn.h>
#include <unordered_map>

#import <AppKit/AppKit.hpp>
#import <MetalKit/MetalKit.hpp>

#include "../application.h"
#include "../platform.h"
#include "../engine.h"

using namespace std::chrono_literals;
using namespace mercury;

#include "../application.h"
#include "../platform.h"
#include <thread>
#include "cocoa_wrapper.h"

using namespace mercury;

void platform::initialize()
{
}

void platform::shutdown()
{

}

class Renderer {
public:
    explicit Renderer(MTL::Device* device);
    ~Renderer();

    void Draw(MTK::View* view);

private:
    MTL::Device* device{nullptr};
    MTL::CommandQueue* commandQueue{nullptr};
};

Renderer::Renderer(MTL::Device* device) : device{device->retain()}{
    commandQueue = device->newCommandQueue();
}


Renderer::~Renderer() {
    commandQueue->release();
    device->release();
}

void Renderer::Draw(MTK::View *view) {

    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //get the current command buffer object to encode commands for execution in the GPU
    auto* commandBuffer = commandQueue->commandBuffer();
    //get the current render pass descriptor that will be populated with different render targets and their information
    auto* renderPassDescriptor = view->currentRenderPassDescriptor();
    //encodes the renderPass descriptor into actually commands
    auto* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
    //YOU SHALL NOT ENCODE ANYMORE - end encoding
    renderCommandEncoder->endEncoding();
    //tell gpu we got something to draw
    commandBuffer->presentDrawable(view->currentDrawable());
    //this ain't a marriage, commit to the damn draw
    commandBuffer->commit();

    pool->release();

}

class View : public MTK::ViewDelegate {
public:
    explicit View(MTL::Device* device) : renderer{new Renderer(device)} {}
    ~View() override { delete renderer;};

    void drawInMTKView(MTK::View *pView) override {
        gApplication->Update();
        engine::update();
       // renderer->Draw(pView);
    };

private:
    Renderer* renderer{nullptr};
};

class MercuryApp : public NS::ApplicationDelegate {
public:
    MercuryApp() = default;
    MercuryApp(std::string winTitle, CGFloat winWidth, CGFloat winHeight) : title(std::move(winTitle)), width{winWidth}, height{winHeight} {}
    ~MercuryApp() override;

    //This method is called after the application’s main run loop has been started, but before it has processed any events
    void applicationDidFinishLaunching(NS::Notification* pNotification) override;
    //Tells the delegate that the app’s initialization is about to complete.
    void applicationWillFinishLaunching(NS::Notification* pNotification) override;
    //The application sends this message to your delegate when the application’s last window is closed
    bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender) override;

    NS::Menu* createMenuBar();

private:
    std::string title{"Metal Window"};

    CGFloat width{1280};
    CGFloat height{720};

    NS::Window* m_window{nullptr};
    MTK::View* m_view{nullptr};
    MTL::Device* m_device{nullptr};
    View* m_appView{nullptr};
};

MercuryApp::~MercuryApp() {

    gApplication->Shutdown();
    engine::shutdown();

    m_view->release();
    m_window->release();
    m_device->release();
    delete m_appView;
}
CA::MetalLayer *gMainOutput = nullptr;
MTK::View *gMainView = nullptr;

void MercuryApp::applicationDidFinishLaunching(NS::Notification* pNotification) {
    //Returns the device instance Metal selects as the default
    engine::initialize();
    gApplication->Initialize();

    m_device = MTL::CreateSystemDefaultDevice();

    //window frame dimensions
    CGRect frame{{0, 0}, {width, height}};

    //create window object with properties
    m_window = NS::Window::alloc()->init(frame,
                                         NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
                                         NS::BackingStoreBuffered,
                                         false);

    //specialized view that creates, configures, and display Metal objects
    m_view = MTK::View::alloc()->init(frame, m_device);
    //specify color format & clear window color
    m_view->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    m_view->setClearColor(MTL::ClearColor::Make(1.0, 0.0, 0.0, 1.0));

    //construct a view delegate and set it
   m_appView = new View(m_device);
    m_view->setDelegate(m_appView);
    gMainView = m_view;

    //the window retains the new content view and owns it thereafter, view obj is resized to fit precisely within the content area of the window
    m_window->setContentView(m_view);
    m_window->setTitle(NS::String::string(title.c_str(), NS::StringEncoding::UTF8StringEncoding));

    //moves the window
    m_window->makeKeyAndOrderFront(nullptr);

    //get global shared application state and activate it
    auto* appNS = reinterpret_cast<NS::Application*>(pNotification->object());
    appNS->activateIgnoringOtherApps(true);

    gMainOutput = m_view->currentDrawable()->layer();

    printf("layer");

}

void MercuryApp::applicationWillFinishLaunching(NS::Notification *pNotification) {
    NS::Menu* menu = createMenuBar();

    //get global shared application state
    auto* appNS = reinterpret_cast<NS::Application*>(pNotification->object());
    //assign a new menu bar
    appNS->setMainMenu(menu);
    //controls whether and how an app may be activated
    appNS->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);

}

bool MercuryApp::applicationShouldTerminateAfterLastWindowClosed(NS::Application *pSender) {
    return true;
}

NS::Menu* MercuryApp::createMenuBar() {
    using NS::StringEncoding::UTF8StringEncoding;

    //create a main menu instance
    auto* mainMenu = NS::Menu::alloc()->init();

    //create an app menu instance
    auto* appMenu = NS::Menu::alloc()->init(NS::String::string( "Appname", UTF8StringEncoding ));
    //create a menuItem instance for
    auto* appMenuItem = NS::MenuItem::alloc()->init();

    //ptr to application title name
    auto* appName = NS::RunningApplication::currentApplication()->localizedName();
    //create a string and append a string to it
    NS::String* quitItemName = NS::String::string( "Quit ", UTF8StringEncoding )->stringByAppendingString( appName );

    //callback object pointer that closes application
    SEL quitCallBackFunc = NS::MenuItem::registerActionCallback("appQuit", [](void*,SEL,const NS::Object* pSender){
        auto* appInstance = NS::Application::sharedApplication();
        appInstance->terminate( pSender );
    });

    //add a menu item that invokes a callback when pressing q while command key is pressed
    NS::MenuItem* appQuitItem = appMenu->addItem( quitItemName, quitCallBackFunc, NS::String::string( "q", UTF8StringEncoding ) );
    appQuitItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );
    //add as submenu option for menu
    appMenuItem->setSubmenu( appMenu );


    //create a menu instance
    auto* windowMenu = NS::Menu::alloc()->init( NS::String::string( "Window", UTF8StringEncoding ) );
    //create a menu item instance
    auto* windowMenuItem = NS::MenuItem::alloc()->init();

    //callback object pointer that closes application
    SEL closeWindowCallback = NS::MenuItem::registerActionCallback( "windowClose", [](void*, SEL, const NS::Object*){
        auto* appInstance = NS::Application::sharedApplication();
        appInstance->windows()->object<NS::Window>(0)->close();
    });

    //add a menu item that invokes a callback when pressing w while command key is pressed
    auto* closeWindowItem = windowMenu->addItem( NS::String::string( "Close Window", UTF8StringEncoding ), closeWindowCallback, NS::String::string( "w", UTF8StringEncoding ) );
    closeWindowItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );
    //add as sub menu option for window menu
    windowMenuItem->setSubmenu( windowMenu );

    //add menu items to main menu
    mainMenu->addItem( appMenuItem );
    mainMenu->addItem( windowMenuItem );

    //release memory
    appMenuItem->release();
    windowMenuItem->release();
    appMenu->release();
    windowMenu->release();

    //return ptr holding main menu
    return mainMenu->autorelease();
}

//console main
int main()
{
//initialize autoreleasePool pool
    NS::AutoreleasePool* poolAllocator = NS::AutoreleasePool::alloc()->init();

    //application delegate
    MercuryApp mapp;

    /* every application uses a single instance of NSApplication, to control the main event loop,
     * keep track of app's windows and menus, distribute events to appropriate objects, set up
     * autorelease pools, and receive notification of app-level events. */
    NS::Application* appNS = NS::Application::sharedApplication();

    /* Every NSApplication has a delegate that is notified when app is start or terminated.
     * By setting the delegate and implementing the delegate methods,
     * you customize the behavior of your app without having to subclass NSApplication  */
   appNS->setDelegate(&mapp);
    appNS->run();
    poolAllocator->release();

    return 0;
}


void platform::createMainWindow()
{
    createCocoaWindow();
}

void platform::destroyMainWindow()
{

}


void platform::update()
{
	using namespace std::chrono_literals;

	std::this_thread::sleep_for((1ms));
}

int platform::utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0;// ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, static_cast<int>(strlen(utf8Str)), outBuff, buffLen);
}

int platform::wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0;// ::WideCharToMultiByte(CP_UTF8, 0, wideStr, static_cast<int>(wcslen(wideStr)), outBuff, buffLen, nullptr, nullptr);
}

long platform::interlockedAdd32(volatile long* value, long add)
{
	return __sync_fetch_and_add(value, add);
}

void* platform::alignedMalloc(int size, int align)
{
	return aligned_alloc(size, align);
}

void platform::alignedFree(void* ptr)
{
	free(ptr);
}

void* platform::loadSharedLibrary(const char* utf8libname)
{
	return dlopen(utf8libname, RTLD_NOW);
}

bool platform::unloadSharedLibrary(void* library)
{
	dlclose(library);
	return true;
}

std::unordered_map<void*, void*> ptrToLibHandleMap;

void* platform::getFuncPtrImpl(void* library, const char* funcName)
{
	auto result = dlsym(library, funcName);

	ptrToLibHandleMap[library] = result;
	return result;
}

void platform::fatalFail(const char* reason)
{
	mercury::write_log_message("%s\n", reason);
	//DebugBreak();
}

const char* platform::getSharedLibraryFullFilename(void* libHandle)
{
	static Dl_info dl_info = {};
	dladdr(ptrToLibHandleMap[libHandle], &dl_info);

	return dl_info.dli_fname;
}

void platform::outputToDebugConsole(const char* text)
{
	printf("%s", text);
}

void platform::outputToLogFile(const char* text)
{

}

void* platform::getMainWindowHandle()
{
	return gMainOutput;
}

void* platform::getAppInstanceHandle()
{
	return gMainView;
}



int utf8ToWide(const char* utf8Str, wchar_t* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

int wideToUtf8(const wchar_t* wideStr, char* outBuff, int buffLen)
{
	return 0; //TODO: implement
}

#endif
